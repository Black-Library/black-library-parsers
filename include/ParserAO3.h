/**
 * ParserAO3.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_AO3_PARSER_AO3_H__
#define __BLACK_LIBRARY_CORE_PARSERS_AO3_PARSER_AO3_H__

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace AO3 {

class ParserAO3 : public Parser
{
public:
    ParserAO3();
    ~ParserAO3();

    void Parse();

protected:
    std::string ParseTitle();
    std::string ParseAuthor();
    void ParseChapter();

private:
};

} // namespace AO3

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
