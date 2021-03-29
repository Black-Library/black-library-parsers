/**
 * ParserManager.cc
 */

#include <chrono>
#include <sstream>
#include <thread>

#include <ParserManager.h>

namespace black_library {

namespace core {

namespace parsers {

ParserManager::ParserManager(const uint8_t &num_threads, const std::string &config) :
    pool_(num_threads),
    job_queue_(),
    pool_results_(),
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

        std::cout << "tick" << std::endl;

        std::this_thread::sleep_until(deadline);
    }

    return 0;
}

int ParserManager::RunOnce()
{
    // add new urls to pool

    // check if futures list is ready
    for (auto & res : pool_results_)
    {
        // check if future is ready
        if (res.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            if (!res.valid())
                continue;

            ParserManagerResult result = res.get(); 
        }
    }

    // add new urls to ready

    return 0;
}

int ParserManager::Stop()
{
    // TODO: check to make sure pool_ does not leak memory
    done_ = true;

    return 0;
}

int ParserManager::AddUrl(const std::string &url)
{
    ParserManagerJob job;
    job.starting_chapter = 1;
    job.url = url;

    std::cout << "ParserManager adding job with url: " << job.url << " starting chapter: " << job.starting_chapter << std::endl;

    job_queue_.push(job);

    return 0;
}

int ParserManager::AddUrl(const std::string &url, const size_t &starting_chapter)
{
    ParserManagerJob job;
    job.starting_chapter = starting_chapter;
    job.url = url;

    std::cout << "ParserManager adding job with url: " << job.url << " starting chapter: " << job.starting_chapter << std::endl;

    job_queue_.push(job);

    return 0;
}

void ParserManager::Init()
{
    std::vector<std::future<ParserManagerResult>> results;

    for(size_t i = 0; i < 8; ++i)
    {
        results.emplace_back(
            pool_.enqueue([this, i]
            {
                ParserManagerResult result;
                std::stringstream ss;
                ss << "Starting parser manager slot " << i << std::endl;
                if (job_queue_.empty())
                {
                    ss << "no jobs in queue" << std::endl;
                    result.io_result = ss.str();
                    return result;
                }
                ParserManagerJob job = job_queue_.pop();
                ParserFactoryResult factory_result = parser_factory_.GetParser(job.url);
                ss << factory_result.io_string;
                ss << "Parser type: " << GetParserName(factory_result.parser_result->GetParserType()) << std::endl;
                if (factory_result.has_error)
                {
                  ss << factory_result.error_string;
                  result.has_error = true;
                  ss << "Error: parser manager slot " << i << " factory failed" << std::endl;
                  result.io_result = ss.str();
                  return result;
                }
                factory_result.parser_result->Parse(job.starting_chapter);

                ss << "Stopping parser manager slot " << i << std::endl;
                result.io_result = ss.str();
                return result;
            })
        );
    }

    for (auto & res : results)
    {
        if (!res.valid())
            continue;

        res.wait();
        ParserManagerResult result = res.get(); 
        std::cout << result.io_result << std::endl;
    }
}

} // namespace parsers
} // namespace core
} // namespace black_library
