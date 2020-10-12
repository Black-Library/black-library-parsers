#ifndef __LIBRARYCORE_FFN_PARSER_FFN_H__
#define __LIBRARYCORE_FFN_PARSER_FFN_H__

#include <Parser.hh>

namespace librarycore {
namespace FFN {

class ParserFFN : public Parser
{
public:
    ParserFFN();
    ~ParserFFN();

    Result Parse();
    Parser Copy();

protected:
    Result ParseChapter(int chap_num);
    Result ParseChapter(std::string url);

private:
};

} // namespace FFN
} // namespace librarycore

#endif
