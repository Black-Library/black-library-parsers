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

ParserWorker::ParserWorker(std::shared_ptr<Parser> parser_ptr, size_t num_parsers, parser_rep parser_type) :
    parsers_(),
    pool_(num_parsers),
    job_queue_(),
    pool_results_(),
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
            
            std::thread t([this, parser, &parser_error](){

                while (!done_ && !parser->GetDone() && !parser_error)
                {
                    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

                    if (done_)
                        break;

                    std::this_thread::sleep_until(deadline);
                }

                parser->Stop();
            });

            parser->SetUrl(job.url);

            auto parser_result = parser->Parse(job.starting_chapter);

            if (parser_result.has_error)
                parser_error = true;

            t.join();

            ss << "Stopping parser: " << GetParserName(parser->GetParserType()) << ": " << parser->GetParserIndex() <<  std::endl;
            result.io_result = ss.str();
            result.uuid = job.uuid;
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
        std::cout << "Error: ParserWorker was sent empty uuid" << std::endl;
        return -1;
    }

    if (parser_job.url.empty())
    {
        std::cout << "Error: ParserWorker was sent empty url" << std::endl;
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

} // namespace parsers
} // namespace core
} // namespace black_library
