/**
 * ParserManager.hh
 */

#ifndef __LIBRARYCORE_PARSER_MANAGER_HH__
#define __LIBRARYCORE_PARSER_MANAGER_HH__

#include <atomic>
#include <bitset>
#include <queue>
#include <unordered_map>

#include <Parser.h>
#include <ParserWorker.h>

namespace librarycore {

class ParserManager
{
public:
    explicit ParserManager(const std::string &config);

    ParserManager &operator = (ParserManager &&) = default;

    int Run();
    int RunOnce();
    int Stop();

private:
    void Init();

    void SendWorkersUrls();

    std::unordered_map<managed_parsers_rep, Parser> parsers_;
    std::bitset<_MANAGED_PARSER_COUNT> parser_ready_bit_;
    std::queue<std::string> urls_;
    std::string config_;
    std::atomic_bool done_;
};

} // namespace librarycore

#endif