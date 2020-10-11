/**
 * ParserManager.hh
 */

#ifndef __LIBRARYCORE_PARSER_MANAGER_HH__
#define __LIBRARYCORE_PARSER_MANAGER_HH__

#include <atomic>
#include <queue>
#include <unordered_map>

#include <ThreadPool.h>

#include <Parser.h>

namespace librarycore {

class ParserManager
{
public:
    explicit ParserManager(const uint8_t &num_threads, const std::string &config);

    ParserManager &operator = (ParserManager &&) = default;

    int Run();
    int RunOnce();
    int Stop();

private:
    void Init();

    ThreadPool pool_;
    std::queue<std::string> urls_;
    std::string config_;
    std::atomic_bool done_;
};

} // namespace librarycore

#endif