/**
 * ParserManager.cc
 */

#include <chrono>
#include <thread>

#include <ParserAO3.h>

#include <ParserManager.hh>

namespace librarycore {

ParserManager::ParserManager(const std::string &config) :
    parsers_(),
    parser_ready_bit_(0),
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
    CheckParsersReady();
    SendParsersUrls();

    return 0;
}

int ParserManager::Stop()
{
    done_ = true;

    return 0;
}

void ParserManager::Init()
{
    // construct managed parsers
    parsers_.emplace(AO3_0_PARSER, AO3::ParserAO3());

    // set all bits ready
    parser_ready_bit_.set();
}

void ParserManager::CheckParsersReady()
{
    for (size_t i = 0; i < _MANAGED_PARSER_COUNT; ++i)
    {

    }
}

void ParserManager::SendParsersUrls()
{
    
}



} // namespace librarycore