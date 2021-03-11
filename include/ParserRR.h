/**
 * ParserRR.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_RR_PARSER_RR_H__
#define __BLACK_LIBRARY_CORE_PARSERS_RR_PARSER_RR_H__

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace RR {

class ParserRR : public Parser
{
public:
    ParserRR();

    void Parse();
    Parser Copy();

protected:
    std::string ParseTitle();
    std::string ParseAuthor();
    void ParseChapter();
    void FindChapterNodes(xmlNode *root_node);

private:
};

} // namespace RR

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
