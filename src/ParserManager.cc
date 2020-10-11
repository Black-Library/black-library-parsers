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
    urls_(),
    config_(config),
    done_(true)
{
    Init();
}

int ParserManager::Run()
{
    done_ = true;

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
    
    done_ = true;

    return 0;
}

void ParserManager::Init()
{
    // TODO make this map results to result objects
    std::vector<std::future<int>> results;

    for(int i = 0; i < 8; ++i) {
        results.emplace_back(
            pool_.enqueue([i] {
                std::cout << "hello " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "world " << i << std::endl;
                return i*i;
            })
        );
    }
}

} // namespace librarycore