/**
 * ParserWorker.cc
 */

#include <ParserWorker.h>

namespace black_library {

namespace core {

namespace parsers {

ParserWorker::ParserWorker(parser_rep parser_type, std::shared_ptr<Parser> parser_ptr) :
    parsers_(),
    pool_(_MANAGED_PARSER_COUNT),
    job_queue_(),
    parser_type_(parser_type),
    done_(true)
{
    for (size_t i = 0; i < _MANAGED_PARSER_COUNT; ++i)
    {
        parsers_.emplace_back(std::static_pointer_cast<Parser>(parser_ptr));
    }
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
