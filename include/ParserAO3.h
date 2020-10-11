#ifndef __LIBRARYCORE_AO3_PARSER_AO3_H__
#define __LIBRARYCORE_AO3_PARSER_AO3_H__

#include "Parser.h"

namespace librarycore {

namespace AO3 {

class ParserAO3 : public Parser
{
public:
    ParserAO3();
    ~ParserAO3();

    void Parse();
    Parser Copy();

protected:
    void ParseChapter();

private:
};

} // namespace AO3

} // namespace librarycore

#endif
