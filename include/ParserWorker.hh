/**
 * ParserWorker.hh
 */

#ifndef __LIBRARYCORE_PARSER_Worker_HH__
#define __LIBRARYCORE_PARSER_Worker_HH__

#include <atomic>
#include <bitset>
#include <queue>
#include <unordered_map>

#include <ThreadPool.h>

#include <Parser.h>

namespace librarycore {

class ParserWorker
{
public:
    explicit ParserWorker(const std::string &source_name);

private:
    void Init();

    std::unordered_map<managed_parsers_rep, Parser> parsers_;
    std::bitset<_MANAGED_PARSER_COUNT> parser_ready_bit_;
    std::queue<std::string> urls_;
    std::string source_name_;
    std::atomic_bool done_;
};

} // namespace librarycore

#endif