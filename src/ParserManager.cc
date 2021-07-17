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

namespace BlackLibraryCommon = black_library::core::common;

ParserManager::ParserManager(const std::string &storage_dir, const std::string &config) :
    worker_map_(),
    parser_factory_(std::make_shared<ParserFactory>()),
    current_jobs_(),
    job_queue_(),
    result_queue_(),
    progress_number_callback_(),
    database_status_callback_(),
    config_(config),
    storage_dir_(storage_dir),
    done_(true),
    initialized_(false)
{
    if (storage_dir_.empty())
    {
        storage_dir_ = "/mnt/black-library/store";
        std::cout << "Empty storage dir given, using default: " << storage_dir_ << std::endl;
    }

    // okay to pop_back(), string isn't empty
    if (storage_dir_.back() == '/')
        storage_dir_.pop_back();

    if (!BlackLibraryCommon::CheckFilePermission(storage_dir_))
    {
        std::cout << "Error: parser manager could not access storage directory: " << storage_dir_ << std::endl;
        return;
    }

    std::cout << "Using storage dir: " << storage_dir_ << std::endl;

    AddWorker(parser_t::AO3_PARSER, 1);
    AddWorker(parser_t::RR_PARSER, 1);
    AddWorker(parser_t::SBF_PARSER, 1);
    AddWorker(parser_t::SVF_PARSER, 1);

    for (auto & worker : worker_map_)
    {
        worker.second->RegisterProgressNumberCallback(
            [this](const std::string &uuid, size_t progress_num, bool error)
            {
                if (progress_number_callback_)
                    progress_number_callback_(uuid, progress_num, error);
            }
        );
        worker.second->RegisterJobStatusCallback(
            [this](const ParserJob &parser_job, job_status_t job_status)
            {
                if (!current_jobs_.find_and_replace(std::make_pair(parser_job.uuid, parser_job.is_error_job), job_status))
                    std::cout << "Error: could not replace job status" << std::endl;
            }
        );
        worker.second->RegisterManagerNotifyCallback(
            [this](ParserJobResult result)
            {
                std::cout << "ParserManager recieved: " << result << std::endl;
                result_queue_.push(result);
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

        if (parser_type == parser_t::ERROR_PARSER)
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
    }

    for (auto & worker : worker_map_)
    {
        worker.second->RunOnce();
    }

    while (!result_queue_.empty())
    {
        auto job_result = result_queue_.pop();
        if (job_result.metadata.uuid.empty() || job_result.metadata.url.empty())
        {
            std::cout << "ParserManager: got job result with empty uuid or url" << std::endl;
            continue;
        }

        std::cout << "ParserManager: finished job with uuid: " << job_result.metadata.uuid << " - " << job_result.metadata.url << std::endl;
        current_jobs_.erase(std::make_pair(job_result.metadata.uuid, job_result.is_error_job));

        if (job_result.has_error)
            std::cout << "\tParserManager: Error in job with uuid: " << job_result.metadata.uuid << " - " << job_result.metadata.url << std::endl;

        if (!job_result.has_error && database_status_callback_)
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

int ParserManager::AddJob(const std::string &uuid, const std::string &url, const std::string &last_url)
{
    AddJob(uuid, url, last_url, 1, 0, false);

    return 0;
}

int ParserManager::AddJob(const std::string &uuid, const std::string &url, const std::string &last_url, const size_t &start_number)
{
    AddJob(uuid, url, last_url, start_number, 0, false);

    return 0;
}

int ParserManager::AddJob(const std::string &uuid, const std::string &url, const std::string &last_url, const size_t &start_number, const size_t &end_number)
{
    AddJob(uuid, url, last_url, start_number, end_number, false);

    return 0;
}

int ParserManager::AddJob(const std::string &uuid, const std::string &url, const std::string &last_url, const size_t &start_number, const size_t &end_number, const error_job_rep &is_error_job)
{
    ParserJob parser_job;
    
    parser_job.uuid = uuid;
    parser_job.url = url;
    parser_job.last_url = last_url;
    parser_job.start_number = start_number;
    parser_job.end_number = end_number;
    parser_job.is_error_job = is_error_job;

    std::cout << "ParserManager adding job: " << parser_job << std::endl;

    if (current_jobs_.count(std::make_pair(parser_job.uuid, parser_job.is_error_job)))
    {
        std::cout << "ParserManager already working on job: " << parser_job << std::endl;
        return 0;
    }
    else
    {
        current_jobs_.emplace(std::make_pair(parser_job.uuid, parser_job.is_error_job), job_status_t::JOB_MANAGER_QUEUED);
    }

    job_queue_.push(parser_job);

    return 0;
}

bool ParserManager::GetDone()
{
    return done_;
}

int ParserManager::AddWorker(parser_t parser_type, size_t num_parsers)
{
    std::cout << "ParserManager AddWorker: " << GetParserName(parser_type) << " num: " << num_parsers << std::endl;

    worker_map_.emplace(parser_type, std::make_shared<ParserWorker>(parser_factory_, storage_dir_, parser_type, num_parsers));

    return 0;
}

int ParserManager::RegisterProgressNumberCallback(const progress_number_callback &callback)
{
    progress_number_callback_ = callback;

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
