/**
 * ParserManager.cc
 */

#include <chrono>
#include <sstream>
#include <thread>

#include <ParserAO3.h>
#include <ParserManager.h>

namespace black_library {

namespace core {

namespace parsers {

ParserManager::ParserManager(const uint8_t &num_threads, const std::string &config) :
    pool_(num_threads),
    urls_(),
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
    urls_.emplace(url);

    return 0;
}

void ParserManager::Init()
{
    // TODO make this map results to result objects
    std::vector<std::future<ParserManagerResult>> results;

    for(size_t i = 0; i < 8; ++i)
    {
        results.emplace_back(
            pool_.enqueue([this, i]
            {
                ParserManagerResult result;
                std::stringstream ss;
                ss << "Starting parser manager slot " << i << std::endl;
                std::string url = "fanfiction.net/s/123456";
                ParserFactoryResult factory_result = parser_factory_.GetParser(url);
                if (factory_result.has_error)
                {
                  ss << factory_result.result_error.error_string;
                  result.has_error = true;
                  ss << "Error: parser manager slot " << i << " factory failed" << std::endl;
                  result.io_result = ss.str();
                  return result;
                }
                factory_result.parser_result.Parse();

                std::this_thread::sleep_for(std::chrono::seconds(1));
                ss << "stopping parser manager slot " << i << std::endl;
                result.io_result = ss.str();
                return result;
            })
        );
    }

    for (auto & res : results)
    {
        res.wait();
        ParserManagerResult result = res.get(); 
        std::cout << result.io_result << std::endl;
    }
}

} // namespace parsers
} // namespace core
} // namespace black_library
