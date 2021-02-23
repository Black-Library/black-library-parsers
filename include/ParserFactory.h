/**
 * ParserFactory.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_FACTORY_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_FACTORY_H__

#include <unordered_map>

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

struct ParserFactoryResultError {
    std::string error_string;
};

struct ParserFactoryResult {
    Parser parser_result;
    ParserFactoryResultError result_error;
    bool has_error = false;
};

class ParserFactory
{
public:
    ParserFactory();
    ~ParserFactory();

    ParserFactoryResult GetParser(const std::string &url);

protected:
    int InitParserMap();
    int InitParserUrlMap();

    std::unordered_map<parser_rep, Parser> parser_map_;
    std::unordered_map<std::string, parser_rep> parser_url_map_;

private:
};

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
