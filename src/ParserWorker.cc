/**
 * ParserWorker.cc
 */

#include <ParserWorker.h>

namespace black_library {

namespace core {

namespace parsers {

ParserWorker::ParserWorker(std::shared_ptr<Parser> parser_ptr, size_t num_parsers, parser_rep parser_type) :
    parsers_(),
    pool_(_MANAGED_PARSER_COUNT),
    job_queue_(),
    parser_type_(parser_type),
    done_(true)
{
    (void) parser_ptr;
    for (size_t i = 0; i < num_parsers; ++i)
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
    ParserJob job;
    job.starting_chapter = 1;
    job.url = url;

    std::cout << "ParserWorker " << GetParserName(parser_type_) << " adding job with url: " << job.url << " starting chapter: " << job.starting_chapter << std::endl;

    job_queue_.push(job);

    return 0;
}

int ParserWorker::AddJob(const std::string &url, const size_t &starting_chapter)
{
    ParserJob job;
    job.starting_chapter = starting_chapter;
    job.url = url;

    std::cout << "ParserWorker " << GetParserName(parser_type_) << " adding job with url: " << job.url << " starting chapter: " << job.starting_chapter << std::endl;

    job_queue_.push(job);

    return 0;
}

} // namespace parsers
} // namespace core
} // namespace black_library
