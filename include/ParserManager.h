/**
 * ParserManager.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_MANAGER_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_MANAGER_H__

#include <atomic>
#include <bitset>
#include <unordered_map>
#include <vector>

#include "BlockingQueue.h"
#include "ThreadPool.h"

#include "Parser.h"
#include "ParserFactory.h"
#include <ParserWorker.h>

namespace black_library {

namespace core {

namespace parsers {

struct ParserManagerJob {
    std::string url;
    size_t starting_chapter;
};

struct ParserManagerResult {
    std::string io_result;
    std::string error_string;
    bool has_error = false;
};

class ParserManager
{
public:
    explicit ParserManager(const uint8_t &num_threads, const std::string &config);
    ParserManager &operator = (ParserManager &&) = default;

    int Run();
    int RunOnce();
    int Stop();

    int AddUrl(const std::string &url);
    int AddUrl(const std::string &url, const size_t &starting_chapter);

private:
    void Init();
    int AddWorker(parser_rep parser_type);

    std::unordered_map<parser_rep, std::shared_ptr<ParserWorker>> worker_map_;
    ThreadPool pool_;
    ParserFactory parser_factory_;
    BlockingQueue<ParserManagerJob> job_queue_;
    std::vector<std::future<ParserManagerResult>> pool_results_;
    std::string config_;
    std::atomic_bool done_;
};

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
