#ifndef __LIBRARYCORE_PARSER_FACTORY_HH__
#define __LIBRARYCORE_PARSER_FACTORY_HH__

#include <unordered_map>

#include <Parser.hh>
#include <ParserAO3.hh>
#include <ParserFFN.hh>

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
