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
    explicit ParserManager(const std::string &storage_dir, const std::string &config);
    ParserManager &operator = (ParserManager &&) = default;

    int Run();
    int RunOnce();
    int Stop();

    bool IsReady();

    int AddJob(const std::string &uuid, const std::string &url);
    int AddJob(const std::string &uuid, const std::string &url, const size_t &start_number);
    bool GetDone();

    int RegisterProgressNumberCallback(const progress_number_callback &callback);
    int RegisterDatabaseStatusCallback(const database_status_callback &callback);

private:
    int AddResult(ParserJobResult result);
    int AddWorker(parser_t parser_type, size_t num_parsers);

    std::unordered_map<parser_t, std::shared_ptr<ParserWorker>> worker_map_;
    std::shared_ptr<ParserFactory> parser_factory_;
    BlockingUnorderedMap<std::string, job_status_t> current_jobs_;
    BlockingVector<ParserJob> current_errors_;
    BlockingQueue<ParserJob> job_queue_;
    BlockingQueue<ParserJobResult> result_queue_;
    progress_number_callback progress_number_callback_;
    database_status_callback database_status_callback_;
    std::string config_;
    std::string storage_dir_;
    std::atomic_bool done_;
    std::atomic_bool initialized_;
};

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
