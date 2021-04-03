/**
 * ParserFactory.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_FACTORY_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_FACTORY_H__

#include <memory>
#include <unordered_map>

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

struct ParserFactoryResult {
    std::shared_ptr<Parser> parser_result;
    std::string io_string;
    std::string error_string;
    bool has_error = false;
};

class ParserFactory
{
public:
    ParserFactory();
    ~ParserFactory();

    ParserFactoryResult GetParserByUrl(const std::string &url);
    ParserFactoryResult GetParserByType(parser_rep parser_type);

protected:
    int InitParserMap();

    std::unordered_map<parser_rep, std::shared_ptr<Parser>> parser_map_;

private:
};

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
