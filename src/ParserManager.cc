/**
 * ParserManager.cc
 */

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include <ParserManager.h>

namespace black_library {

namespace core {

namespace parsers {

ParserManager::ParserManager(const std::string &config) :
    job_queue_(),
    result_queue_(),
    config_(config),
    done_(true)
{
    Init();
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

        if (parser_type == _NUM_PARSERS_TYPE)
        {
            std::cout << "Error: could not match url to parser" << std::endl;
            continue;
        }

        auto worker = worker_map_.find(parser_type);

        if (worker == worker_map_.end())
        {
            std::cout << "Error: could not find parser with rep: " << parser_type << std::endl;
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
        auto job = result_queue_.pop();
        std::cout << "ParserManager: finished job with uuid: " << job.uuid << std::endl;
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

    job_queue_.push(job);

    return 0;
}

void ParserManager::Init()
{
    AddWorker(AO3_PARSER);
    AddWorker(RR_PARSER);
    RegisterWorkerCallbacks();
}

int ParserManager::AddWorker(parser_rep parser_type)
{
    ParserFactoryResult factory_result = parser_factory_.GetParserByType(parser_type);
    
    std::cout << "ParserManager AddWorker " << GetParserName(parser_type) << " - " << factory_result.io_string << std::endl;

    if (factory_result.has_error)
    {
        std::cout << "ParserManager AddWorker " << factory_result.error_string << std::endl;
        return -1;
    }

    worker_map_.emplace(parser_type, std::make_shared<ParserWorker>(factory_result.parser_result, 2, parser_type));

    return 0;
}

int ParserManager::RegisterWorkerCallbacks()
{
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
            }
        );
    }

    return 0;
}

} // namespace parsers
} // namespace core
} // namespace black_library
