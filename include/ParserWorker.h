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

class ParserWorker
{
public:
    explicit ParserWorker(std::shared_ptr<Parser> parser_ptr, parser_rep parser_type, size_t num_parsers);
    ParserWorker &operator = (ParserWorker &&) = default;

    int Run();
    int RunOnce();
    int Stop();

    int AddJob(ParserJob parser_job);

    int RegisterJobStatusCallback(const job_status_callback &callback);
    int RegisterManagerNotifyCallback(const manager_notify_callback &callback);

private:
    void Init();

    std::vector<std::shared_ptr<Parser>> parsers_;
    ThreadPool pool_;
    BlockingQueue<ParserJob> job_queue_;
    std::vector<std::future<ParserJobResult>> pool_results_;
    job_status_callback job_status_callback_;
    manager_notify_callback notify_callback_;
    size_t num_parsers_;
    parser_rep parser_type_;
    std::atomic_bool done_;
};

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
