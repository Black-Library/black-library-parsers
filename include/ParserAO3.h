 #ifndef PARSER_AO3_H
#define PARSER_AO3_H

#include "Parser.h"

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

#endif
