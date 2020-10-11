#ifndef PARSER_FACTORY_H
#define PARSER_FACTORY_H

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
   std::unordered_map<std::string, Parser> parser_map_;

private:
};

} // namespace librarycore

#endif
