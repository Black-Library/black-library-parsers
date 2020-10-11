/**
 * PaserFactory.cc
 */

#include <ParserFactory.hh>

namespace librarycore {

 ParserFactory::ParserFactory()
 {
    AO3::ParserAO3 parserAO3;
    this->parser_map_.emplace(AO3::name, parserAO3);
 }

 ParserFactory::~ParserFactory()
 {

 }

 Parser ParserFactory::GetParser(std::string url)
 {
    Parser parser(url);
    std::unordered_map<std::string, Parser>::iterator itr = parser_map_.find(parser.GetSource());
    if(itr == parser_map_.end()) {
        return Parser("");
    }
    parser = itr->second.Copy();
    parser.SetUrl(url);
    return parser;
 }

} // namespace librarycore
