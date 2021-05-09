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

ParserWorker::ParserWorker(const std::shared_ptr<ParserFactory> parser_factory, const std::string &storage_dir, parser_rep parser_type, size_t num_parsers) :
    pool_(num_parsers),
    job_queue_(),
    pool_results_(),
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
    // check if futures list is ready
    for (auto & res : pool_results_)
    {
        // check if future is ready
        if (!res.valid())
            continue;

        if (res.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            ParserJobResult job_result = res.get();
            std::cout << job_result.debug_string << std::endl;

            if (job_result.has_error)
                // TODO: do something about error? notify?
                std::cout << job_result.debug_string << std::endl;

            if (notify_callback_)
                notify_callback_(job_result);
        }
    }

    if (job_queue_.empty())
        return 0;

    // TODO: use more than just the first parser

    pool_results_.emplace_back(
        pool_.enqueue([this]()
        {
            std::stringstream ss;
            ParserJobResult job_result;
            std::atomic_bool parser_error;

            parser_error = false;

            if (job_queue_.empty())
            {
                ss << "ParserWorker: no jobs in queue" << std::endl;
                job_result.debug_string = ss.str();
                return job_result;
            }

            auto parser_job = job_queue_.pop();

            auto factory_result = parser_factory_->GetParserByUrl(parser_job.url);

            if (factory_result.has_error)
            {
                job_result.metadata.url = parser_job.url;
                job_result.metadata.uuid = parser_job.uuid;
                ss << "Factory Error: " << factory_result.debug_string << std::endl;
                job_result.debug_string = ss.str();
                return job_result;
            }

            auto parser = factory_result.parser_result;

            std::string local_file_path = storage_dir_ + "/" + parser_job.uuid + "/";

            parser->SetLocalFilePath(local_file_path);

            if (!black_library::core::common::MakeDirectories(local_file_path))
            {
                ss << "Error: parser worker could make local file path directory: " << local_file_path << std::endl;
                job_result.debug_string = ss.str();
                return job_result;              
            }

            if (!black_library::core::common::CheckFilePermission(local_file_path))
            {
                ss << "Error: parser worker could not access uuid directory: " << local_file_path << std::endl;
                job_result.debug_string = ss.str();
                return job_result;              
            }

            ss << "Starting parser: " << GetParserName(parser->GetParserType()) << ": " << parser_job.uuid <<  std::endl;
            
            std::thread t([this, parser, &parser_job, &parser_error](){

                while (!done_ && !parser->GetDone() && !parser_error)
                {
                    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

                    if (done_)
                        break;

                    std::this_thread::sleep_until(deadline);
                }

                std::cout << GetParserName(parser->GetParserType()) << ": " << parser_job.uuid << " done" << std::endl;

                parser->Stop();
            });

            if (job_status_callback_)
                job_status_callback_(parser_job.uuid, JOB_WORKING);

            auto parser_result = parser->Parse(parser_job);

            if (parser_result.has_error)
                parser_error = true;

            t.join();

            ss << "Stopping parser: " << GetParserName(parser->GetParserType()) << ": " << parser_job.uuid <<  std::endl;
            job_result.debug_string = ss.str();
            job_result.metadata = parser_result.metadata;
            job_result.has_error = false;

            return job_result;
        })
    );
    
    return 0;
}

int ParserWorker::Stop()
{
    // TODO: check to make sure pool_ does not leak memory   
    done_ = true;

    std::cout << "ParserWorker " << GetParserName(parser_type_) << " stop" << std::endl;

    return 0;
}

int ParserWorker::AddJob(ParserJob parser_job)
{
    if (parser_job.uuid.empty())
    {
        std::cout << "Error: ParserWorker was sent parser_job with empty uuid" << std::endl;
        return -1;
    }

    if (parser_job.url.empty())
    {
        std::cout << "Error: ParserWorker was sent parser_job with empty url" << std::endl;
        return -1;
    }

    std::cout << "ParserWorker " << GetParserName(parser_type_) <<
     " adding parser_job with uuid: " << parser_job.uuid << " with url: " << parser_job.url << " starting chapter: " << parser_job.start_chapter << std::endl;

    job_queue_.push(parser_job);

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
