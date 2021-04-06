/**
 * ParserWorker.cc
 */

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
    num_parsers_(num_parsers),
    parser_type_(parser_type),
    done_(true)
{
    (void) parser_ptr;
    for (size_t i = 0; i < num_parsers_; ++i)
    {
        parsers_.emplace_back(std::make_shared<Parser>());
        // parsers_.emplace_back(std::static_pointer_cast<Parser>(parser_ptr));

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
        }
    }

    if (job_queue_.empty())
        return 0;

    // TODO: use more than just the first parser
    auto parser = parsers_[0];

    pool_results_.emplace_back(
        pool_.enqueue([this, parser]
        {
            ParserJobResult result;
            std::stringstream ss;

            ss << "Starting parser: " << GetParserName(parser->GetParserType()) << ": " << parser->GetParserIndex() <<  std::endl;

            if (job_queue_.empty())
            {
                ss << "no jobs in queue" << std::endl;
                result.io_result = ss.str();
                return result;
            }

            auto job = job_queue_.pop();
            
            std::thread t([this, parser](){

                while (!done_ && !parser->GetDone())
                {
                    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

                    if (done_)
                        break;

                    std::cout << "beat" << std::endl;

                    std::this_thread::sleep_until(deadline);
                }
                std::cout << "Pass done" << std::endl;
                parser->Stop();
            });

            parser->SetUrl(job.url);

            parser->Parse(job.starting_chapter);

            // std::cout << "joining" << std::endl;

            t.join();

            ss << "Stopping parser: " << GetParserName(parser->GetParserType()) << ": " << parser->GetParserIndex() <<  std::endl;
            result.io_result = ss.str();
            return result;
        })
    );
    
    return 0;
}

int ParserWorker::Stop()
{
    // TODO: check to make sure pool_ does not leak memory
    done_ = true;

    return 0;
}

int ParserWorker::AddJob(const std::string &url)
{
    AddJob(url, 1);

    return 0;
}

int ParserWorker::AddJob(const std::string &url, const size_t &starting_chapter)
{
    ParserJob job;
    job.starting_chapter = starting_chapter;
    job.url = url;

    if (url.empty())
    {
        std::cout << "Error: ParserWorker sent empty url" << std::endl;
        return -1;
    }

    std::cout << "ParserWorker " << GetParserName(parser_type_) << " adding job with url: " << job.url << " starting chapter: " << job.starting_chapter << std::endl;

    job_queue_.push(job);

    return 0;
}

} // namespace parsers
} // namespace core
} // namespace black_library
