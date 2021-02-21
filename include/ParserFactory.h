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

class ParserFactory
{
public:
    ParserFactory();
    ~ParserFactory();

    Parser GetParser(const std::string &url);
protected:
    std::unordered_map<parser_rep, Parser> parser_map_;

private:
};

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
