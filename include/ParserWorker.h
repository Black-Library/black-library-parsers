/**
 * ParserWorker.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_WORKER_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_WORKER_H__

#include <atomic>
#include <memory>
#include <vector>

#include "BlockingQueue.h"
#include "ThreadPool.h"

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

#define _MANAGED_PARSER_COUNT 2

struct ParserJob {
    std::string url;
    size_t starting_chapter;
};

class ParserWorker
{
public:
    explicit ParserWorker(std::shared_ptr<Parser> parser_ptr, size_t num_parsers, parser_rep parser_type);
    ParserWorker &operator = (ParserWorker &&) = default;

    int Run();
    int RunOnce();
    int Stop();

    int AddJob(const std::string &url);
    int AddJob(const std::string &url, const size_t &starting_chapter);

private:
    void Init();

    std::vector<std::shared_ptr<Parser>> parsers_;
    ThreadPool pool_;
    BlockingQueue<ParserJob> job_queue_;
    parser_rep parser_type_;
    std::atomic_bool done_;
};

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
