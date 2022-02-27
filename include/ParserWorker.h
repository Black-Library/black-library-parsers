/**
 * ParserWorker.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_WORKER_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_WORKER_H__

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "BlockingQueue.h"
#include "ThreadPool.h"

#include "Parser.h"
#include "ParserFactory.h"

namespace black_library {

namespace core {

namespace parsers {

class ParserWorker
{
public:
    explicit ParserWorker(const std::shared_ptr<ParserFactory> parser_factory_, const std::string &storage_dir, parser_t parser_type, size_t num_parsers);
    ParserWorker &operator = (ParserWorker &&) = default;

    int Run();
    int RunOnce();
    int Stop();

    int AddJob(const ParserJob &parser_job);

    int RegisterProgressNumberCallback(const progress_number_callback &callback);
    int RegisterJobStatusCallback(const job_status_callback &callback);
    int RegisterManagerNotifyCallback(const manager_notify_callback &callback);

private:
    void Init();

    ThreadPool pool_;
    BlockingQueue<ParserJob> job_queue_;
    std::vector<std::future<ParserJobResult>> pool_results_;
    std::priority_queue<size_t> pool_erases_;
    progress_number_callback progress_number_callback_;
    job_status_callback job_status_callback_;
    manager_notify_callback notify_callback_;
    std::string storage_dir_;
    std::string worker_name_;
    std::shared_ptr<ParserFactory> parser_factory_;
    std::mutex mutex_;
    parser_t parser_type_;
    std::atomic_bool done_;
};

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
