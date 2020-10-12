#ifndef __LIBRARYCORE_PARSER_FACTORY_HH__
#define __LIBRARYCORE_PARSER_FACTORY_HH__

#include <unordered_map>

#include <Parser.h>
#include <ParserAO3.h>
#include <ParserFFN.h>

namespace librarycore {

class ParserFactory
{
public:
    ParserFactory();
    ~ParserFactory();

    Parser GetParser(std::string url);
protected:
    std::unordered_map<parser_rep, Parser> parser_map_;

private:
};

} // namespace librarycore

#endif
