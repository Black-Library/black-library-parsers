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
#include "BlockingUnorderedMap.h"
#include "ThreadPool.h"

#include "Parser.h"
#include "ParserFactory.h"
#include <ParserWorker.h>

namespace black_library {

namespace core {

namespace parsers {

class ParserManager
{
public:
    explicit ParserManager(const std::string &config);
    ParserManager &operator = (ParserManager &&) = default;

    int Run();
    int RunOnce();
    int Stop();

    int AddJob(const std::string &uuid, const std::string &url);
    int AddJob(const std::string &uuid, const std::string &url, const size_t &starting_chapter);

    int RegisterDatabaseStatusCallback(const database_status_callback &callback);

private:
    void Init();
    int AddResult(ParserJobResult result);
    int AddWorker(parser_rep parser_type);
    int RegisterWorkerCallbacks();

    std::unordered_map<parser_rep, std::shared_ptr<ParserWorker>> worker_map_;
    ParserFactory parser_factory_;
    BlockingUnorderedMap<std::string, job_status_rep> current_jobs_;
    BlockingQueue<ParserJob> job_queue_;
    BlockingQueue<ParserJobResult> result_queue_;
    database_status_callback database_status_callback_;
    std::string config_;
    std::atomic_bool done_;
};

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
