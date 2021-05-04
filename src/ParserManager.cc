/**
 * ParserManager.cc
 */

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include <FileOperations.h>

#include <ParserManager.h>

namespace black_library {

namespace core {

namespace parsers {

ParserManager::ParserManager(const std::string &storage_dir, const std::string &config) :
    worker_map_(),
    parser_factory_(),
    current_jobs_(),
    job_queue_(),
    result_queue_(),
    database_status_callback_(),
    config_(config),
    storage_dir_(storage_dir),
    done_(true),
    initialized_(false)
{
    if (!black_library::core::common::CheckFilePermission(storage_dir_))
    {
        std::cout << "Error: parser manager could not access storage directory" << std::endl;
        return;
    }

    AddWorker(AO3_PARSER, 2);
    AddWorker(RR_PARSER, 2);

    for (auto & worker : worker_map_)
    {
        worker.second->RegisterJobStatusCallback(
            [this](const std::string &uuid, job_status_rep job_status)
            {
                current_jobs_.find_and_replace(uuid, job_status);
            }
        );
        worker.second->RegisterManagerNotifyCallback(
            [this](ParserJobResult result)
            {
                result_queue_.push(result);
                current_jobs_.find_and_replace(result.metadata.uuid, JOB_FINISHED);
            }
        );
    }

    initialized_ = true;
}

int ParserManager::Run()
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

int ParserManager::RunOnce()
{
    while (!job_queue_.empty())
    {
        auto job = job_queue_.pop();
        auto parser_type = GetParserTypeByUrl(job.url);

        if (parser_type == ERROR_PARSER)
        {
            std::cout << "Error: could not match url to parser" << std::endl;
            continue;
        }

        auto worker = worker_map_.find(parser_type);

        if (worker == worker_map_.end())
        {
            std::cout << "Error: could not find parser with rep: " << GetParserName(parser_type) << std::endl;
            continue;
        }

        worker->second->AddJob(job);
        current_jobs_.emplace(job.uuid, JOB_QUEUED);
    }

    for (auto & worker : worker_map_)
    {
        worker.second->RunOnce();
    }

    while (!result_queue_.empty())
    {
        auto job_result = result_queue_.pop();
        std::cout << "ParserManager: finished job with uuid: " << job_result.metadata.uuid << std::endl;
        current_jobs_.erase(job_result.metadata.uuid);

        if (database_status_callback_)
            database_status_callback_(job_result);
    }

    return 0;
}

int ParserManager::Stop()
{
    done_ = true;

    for (auto & worker : worker_map_)
    {
        worker.second->Stop();
    }

    return 0;
}

bool ParserManager::IsReady()
{
    return initialized_;
}

int ParserManager::AddJob(const std::string &uuid, const std::string &url)
{
    AddJob(uuid, url, 1);

    return 0;
}

int ParserManager::AddJob(const std::string &uuid, const std::string &url, const size_t &starting_chapter)
{
    ParserJob job;
    job.starting_chapter = starting_chapter;
    job.uuid = uuid;
    job.url = url;

    std::cout << "ParserManager adding job: " << job.uuid <<  " with url: " << job.url << " - starting chapter: " << job.starting_chapter << std::endl;

    if (current_jobs_.key_exists(job.uuid))
    {
        std::cout << "ParserManager already working on " << job.uuid << std::endl;
        return 0;
    }

    job_queue_.push(job);

    return 0;
}

bool ParserManager::GetDone()
{
    return done_;
}

int ParserManager::AddWorker(parser_rep parser_type, size_t num_parsers)
{
    ParserFactoryResult factory_result = parser_factory_.GetParserByType(parser_type);
    
    std::cout << "ParserManager AddWorker " << GetParserName(parser_type) << " - " << factory_result.io_string << std::endl;

    if (factory_result.has_error)
    {
        std::cout << "ParserManager AddWorker " << factory_result.error_string << std::endl;
        return -1;
    }

    worker_map_.emplace(parser_type, std::make_shared<ParserWorker>(factory_result.parser_result, num_parsers));

    return 0;
}

int ParserManager::RegisterDatabaseStatusCallback(const database_status_callback &callback)
{
    database_status_callback_ = callback;

    return 0;
}

} // namespace parsers
} // namespace core
} // namespace black_library
