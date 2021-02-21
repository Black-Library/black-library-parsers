/**
 * ParserManager.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_MANAGER_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_MANAGER_H__

#include <atomic>
#include <queue>
#include <unordered_map>

#include <ThreadPool.h>

#include "Parser.h"
#include "ParserFactory.h"

namespace black_library {

namespace core {

namespace parsers {

class ParserManager
{
public:
    explicit ParserManager(const uint8_t &num_threads, const std::string &config);

    ParserManager &operator = (ParserManager &&) = default;

    int Run();
    int RunOnce();
    int Stop();

    int AddUrl(const std::string &url);

private:
    void Init();

    ThreadPool pool_;
    ParserFactory parser_factory_;
    std::queue<std::string> urls_;
    std::string config_;
    std::atomic_bool done_;
};

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
