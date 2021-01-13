/**
 * ParserManager.cc
 */

#include <chrono>
#include <thread>

#include <ParserAO3.h>

#include <ParserManager.hh>

namespace librarycore {

ParserManager::ParserManager(const uint8_t &num_threads, const std::string &config) :
    pool_(num_threads),
    parser_factory_(),
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
    // check to make sure pool_ does not leak memory
    done_ = true;

    return 0;
}

void ParserManager::Init()
{
    // TODO make this map results to result objects
    std::vector<std::future<int>> results;

    for(int i = 0; i < 8; ++i)
    {
        results.emplace_back(
            pool_.enqueue([this, i]
            {
                std::cout << "starting parser " << i << std::endl;
                std::string url = "";
                Parser parser = parser_factory_.GetParser(url);
                if(parser.GetUrl() == "")
                {
                  // TODO Return result object
                  std::cout << "ERROR parser " << i << std::endl;
                  return -1;
                }
                parser.Parse();

                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "stopping parser " << i << std::endl;
                return 0;
            })
        );
    }
}

} // namespace librarycore
