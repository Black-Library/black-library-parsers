/**
 * ParserWorker.cc
 */

#include <iostream>
#include <sstream>

#include <ParserWorker.h>

#include <ParserRR.h>

namespace black_library {

namespace core {

namespace parsers {

ParserWorker::ParserWorker(std::shared_ptr<Parser> parser_ptr, parser_rep parser_type, size_t num_parsers) :
    parsers_(),
    pool_(num_parsers),
    job_queue_(),
    pool_results_(),
    job_status_callback_(),
    notify_callback_(),
    num_parsers_(num_parsers),
    parser_type_(parser_type),
    done_(false)
{
    for (size_t i = 0; i < num_parsers_; ++i)
    {
        parsers_.emplace_back(std::static_pointer_cast<Parser>(parser_ptr));
        parsers_[i]->SetParserIndex(i);
    }

    for (auto parser : parsers_)
    {
        std::cout << "ParserWorker: " << GetParserName(parser_type_) << " with parser index: " parser->GetParserIndex() << std::endl;
    }
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
            ParserJobResult result = res.get();
            std::cout << result.io_result << std::endl;

            if (result.has_error)
                // TODO: do something about error? notify?
                std::cout << result.error_string << std::endl;

            if (notify_callback_)
                notify_callback_(result);
        }
    }

    if (job_queue_.empty())
        return 0;

    // TODO: use more than just the first parser

    auto parser = parsers_[0];

    pool_results_.emplace_back(
        pool_.enqueue([this, parser]()
        {
            std::stringstream ss;
            ParserJobResult result;
            std::atomic_bool parser_error;

            parser_error = false;

            ss << "Starting parser: " << GetParserName(parser->GetParserType()) << ": " << parser->GetParserIndex() <<  std::endl;

            if (job_queue_.empty())
            {
                ss << "no jobs in queue" << std::endl;
                result.io_result = ss.str();
                return result;
            }

            auto job = job_queue_.pop();

            std::cout << "starting parser done tracking thread" << std::endl;
            
            std::thread t([this, parser, &parser_error](){

                while (!done_ && !parser->GetDone() && !parser_error)
                {
                    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

                    if (done_)
                        break;

                    std::cout << parser->GetUrl() << " not done yet" << std::endl;

                    std::this_thread::sleep_until(deadline);
                }

                std::cout << parser->GetUrl() << " done" << std::endl;

                parser->Stop();
            });

            parser->SetUrl(job.url);

            if (job_status_callback_)
                job_status_callback_(job.uuid, JOB_WORKING);

            auto parser_result = parser->Parse(job.starting_chapter);

            if (parser_result.has_error)
                parser_error = true;

            t.join();

            ss << "Stopping parser: " << GetParserName(parser->GetParserType()) << ": " << parser->GetParserIndex() <<  std::endl;
            result.io_result = ss.str();
            result.metadata = parser_result.metadata;

            return result;
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
        std::cout << "Error: ParserWorker was sent job with empty uuid" << std::endl;
        return -1;
    }

    if (parser_job.url.empty())
    {
        std::cout << "Error: ParserWorker was sent job with empty url" << std::endl;
        return -1;
    }

    std::cout << "ParserWorker " << GetParserName(parser_type_) <<
     " adding job with uuid: " << parser_job.uuid << " with url: " << parser_job.url << " starting chapter: " << parser_job.starting_chapter << std::endl;

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
