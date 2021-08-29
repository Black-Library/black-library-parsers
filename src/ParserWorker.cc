/**
 * ParserWorker.cc
 */

#include <iostream>
#include <sstream>

#include <FileOperations.h>

#include <ParserWorker.h>

#include <ParserRR.h>

namespace black_library {

namespace core {

namespace parsers {

namespace BlackLibraryCommon = black_library::core::common;

ParserWorker::ParserWorker(const std::shared_ptr<ParserFactory> parser_factory, const std::string &storage_dir, parser_t parser_type, size_t num_parsers) :
    pool_(num_parsers),
    job_queue_(),
    pool_results_(),
    pool_erases_(),
    progress_number_callback_(),
    job_status_callback_(),
    notify_callback_(),
    storage_dir_(storage_dir),
    parser_factory_(parser_factory),
    parser_type_(parser_type),
    done_(false)
{
    std::cout << "Initialize ParserWorker: " << GetParserName(parser_type) << " with pool size: " << pool_.GetSize() << std::endl;
}

int ParserWorker::Run()
{
    done_ = false;

    while (!done_)
    {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

        RunOnce();

        if (done_)
            break;

        std::this_thread::sleep_until(deadline);
    }

    return 0;
}

int ParserWorker::RunOnce()
{
    const std::lock_guard<std::mutex> lock(mutex_);

    // clear pool_erases
    while (!pool_erases_.empty())
    {
        pool_erases_.pop();
    }

    // check if futures list is ready
    for (size_t i = 0; i < pool_results_.size(); ++i)
    {
        auto & result = pool_results_[i];

        // check if future is ready
        if (!result.valid())
            continue;

        if (result.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            ParserJobResult job_result = result.get();

            if (job_result.has_error)
            {
                std::cout << "ParserWorker: " << GetParserName(parser_type_) << " error: " << job_result.debug_string << std::endl;
                pool_erases_.push(i);
            }
            else
            {
                std::cout << job_result.debug_string << std::endl;
                if (notify_callback_)
                    notify_callback_(job_result);
                pool_erases_.push(i);
            }
        }
    }

    while (!pool_erases_.empty())
    {
        size_t pos = pool_erases_.top();
        pool_erases_.pop();
        pool_results_.erase(pool_results_.begin() + pos);
    }

    if (job_queue_.empty())
        return 0;

    auto parser_job = job_queue_.pop();

    pool_results_.emplace_back(
        pool_.enqueue([this, parser_job]()
        {
            std::stringstream ss;
            ParserJobResult job_result;
            std::atomic_bool parser_error;

            if (done_)
                return job_result;

            parser_error = false;

            job_result.metadata.url = parser_job.url;
            job_result.metadata.uuid = parser_job.uuid;

            auto factory_result = parser_factory_->GetParserByUrl(parser_job.url);

            if (factory_result.has_error)
            {
                ss << "Factory Error: " << factory_result.debug_string << std::endl;
                job_result.debug_string = ss.str();
                return job_result;
            }

            auto parser = factory_result.parser_result;

            std::string local_file_path = storage_dir_ + '/' + parser_job.uuid + '/';

            parser->SetLocalFilePath(local_file_path);

            if (!BlackLibraryCommon::CheckFilePermission(storage_dir_))
            {
                ss << "Error: ParserWorker " << GetParserName(parser_type_) << " could not access storage directory: " << storage_dir_ << std::endl;
                job_result.debug_string = ss.str();
                return job_result;
            }

            if (!BlackLibraryCommon::MakeDirectories(local_file_path))
            {
                ss << "Error: ParserWorker " << GetParserName(parser_type_) << " could make local file path directory: " << local_file_path << std::endl;
                job_result.debug_string = ss.str();
                return job_result;
            }

            if (!BlackLibraryCommon::CheckFilePermission(local_file_path))
            {
                ss << "Error: ParserWorker " << GetParserName(parser_type_) << " could not access uuid directory: " << local_file_path << std::endl;
                job_result.debug_string = ss.str();
                return job_result;
            }

            ss << "Starting parser: " << GetParserName(parser->GetParserType()) << ": job: " << parser_job <<  std::endl;

            std::thread t([this, parser, &parser_job, &parser_error](){

                while (!done_ && !parser->GetDone() && !parser_error)
                {
                    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

                    if (done_)
                        break;

                    std::this_thread::sleep_until(deadline);
                }

                std::cout << GetParserName(parser->GetParserType()) << ": job: " << parser_job << " done" << std::endl;

                parser->Stop();
            });

            if (job_status_callback_)
                job_status_callback_(parser_job, job_status_t::JOB_WORKING);

            if (progress_number_callback_)
                parser->RegisterProgressNumberCallback(progress_number_callback_);

            auto parser_result = parser->Parse(parser_job);

            if (parser_result.has_error)
            {
                parser_error = true;
                if (job_status_callback_)
                    job_status_callback_(parser_job, job_status_t::JOB_ERROR);
            }
            else
            {
                if (job_status_callback_)
                    job_status_callback_(parser_job, job_status_t::JOB_FINISHED);
            }

            t.join();

            ss << "Stopping parser: " << GetParserName(parser->GetParserType()) << ": job: " << parser_job <<  std::endl;

            job_result.metadata = parser_result.metadata;
            job_result.start_number = parser_job.start_number;
            job_result.end_number = parser_job.end_number;
            job_result.debug_string = ss.str();

            if (!parser_result.has_error)
                job_result.has_error = false;

            return job_result;
        })
    );

    return 0;
}

int ParserWorker::Stop()
{
    const std::lock_guard<std::mutex> lock(mutex_);
    done_ = true;

    for (auto & result : pool_results_)
    {
        if (result.valid())
        {
            std::cout << "ParserWorker " << GetParserName(parser_type_) << " stop one" << std::endl;
            result.wait();
        }
    }

    std::cout << "ParserWorker " << GetParserName(parser_type_) << " stop all" << std::endl;

    return 0;
}

int ParserWorker::AddJob(const ParserJob &parser_job)
{
    if (parser_job.uuid.empty())
    {
        std::cout << "Error: ParserWorker " << GetParserName(parser_type_) << " was sent parser_job with empty uuid" << std::endl;
        return -1;
    }

    if (parser_job.url.empty())
    {
        std::cout << "Error: ParserWorker " << GetParserName(parser_type_) << " was sent parser_job with empty url" << std::endl;
        return -1;
    }

    std::cout << "ParserWorker " << GetParserName(parser_type_) <<
        " adding parser_job: " << parser_job << std::endl;

    if (job_status_callback_)
        job_status_callback_(parser_job, job_status_t::JOB_WORKER_QUEUED);

    job_queue_.push(parser_job);

    return 0;
}

int ParserWorker::RegisterProgressNumberCallback(const progress_number_callback &callback)
{
    progress_number_callback_ = callback;

    return 0;
}

int ParserWorker::RegisterManagerNotifyCallback(const manager_notify_callback &callback)
{
    notify_callback_ = callback;

    return 0;
}

int ParserWorker::RegisterJobStatusCallback(const job_status_callback &callback)
{
    job_status_callback_ = callback;

    return 0;
}

} // namespace parsers
} // namespace core
} // namespace black_library
