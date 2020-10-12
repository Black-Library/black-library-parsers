#ifndef __LIBRARYCORE_AO3_PARSER_AO3_H__
#define __LIBRARYCORE_AO3_PARSER_AO3_H__

#include <Parser.hh>

namespace librarycore {

namespace AO3 {

class ParserAO3 : public Parser
{
public:
    ParserAO3();
    ~ParserAO3();

    Result Parse();
    Parser Copy();

protected:
    Result ParseChapter(int chap_num);
    Result ParseChapter(std::string url);

private:
};

} // namespace AO3

} // namespace librarycore

#endif
