/**
 * ParserFactory.cc
 */

#include <ParserFactory.h>

namespace black_library {

namespace core {

namespace parsers {

ParserFactory::ParserFactory()
{
    AO3::ParserAO3 parserAO3;
    FFN::ParserFFN parserFFN;
    this->parser_map_.emplace(AO3_PARSER, parserAO3);
    this->parser_map_.emplace(FFN_PARSER, parserFFN);
}

ParserFactory::~ParserFactory()
{

}

Parser ParserFactory::GetParser(const std::string &url)
{
    Parser parser(url);
    std::unordered_map<parser_rep, Parser>::iterator itr = parser_map_.find(parser.GetSource());

    if (itr == parser_map_.end())
    {
        std::cout << "Error, ParserFactory could not match parser" << std::endl;
        return Parser("");
    }

    parser = itr->second.Copy();
    parser.SetUrl(url);
    return parser;
}

} // namespace parsers
} // namespace core
} // namespace black_library
