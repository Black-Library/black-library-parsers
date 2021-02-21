/**
 * ParserFactory.cc
 */

#include <ParserFactory.h>

#include "ParserAO3.h"
#include "ParserFFN.h"

namespace black_library {

namespace core {

namespace parsers {

ParserFactory::ParserFactory()
{
    InitParserMap();
    InitParserUrlMap();
}

ParserFactory::~ParserFactory()
{

}

Parser ParserFactory::GetParser(const std::string &url)
{
    auto parser_type = _NUM_PARSERS_TYPE;
    if (url.find(AO3::url) != std::string::npos)
    {
        parser_type = AO3_PARSER;
    }
    else if (url.find(FFN::url) != std::string::npos)
    {
        parser_type = FFN_PARSER;
    }
    else if (url.find(SBF::url) != std::string::npos)
    {
        parser_type = SBF_PARSER;
    }
    else if (url.find(RR::url) != std::string::npos)
    {
        parser_type = RR_PARSER;
    }
    else 
    {
        std::cout << "Error, ParserFactory could not match parser url" << std::endl;
        return Parser(_NUM_PARSERS_TYPE);
    }

    std::unordered_map<parser_rep, Parser>::iterator itr = parser_map_.find(parser_type);

    if (itr == parser_map_.end())
    {
        std::cout << "Error, ParserFactory could not match parser" << std::endl;
        return Parser(_NUM_PARSERS_TYPE);
    }

    Parser parser = itr->second.Copy();
    parser.SetUrl(url);
    return parser;
}

int ParserFactory::InitParserMap()
{
    AO3::ParserAO3 parserAO3;
    // FFN::ParserFFN parserFFN;
    this->parser_map_.emplace(AO3_PARSER, parserAO3);
    // this->parser_map_.emplace(FFN_PARSER, parserFFN);

    return 0;
}

int ParserFactory::InitParserUrlMap()
{
    for (auto it = parser_map_.begin(); it != parser_map_.end(); ++it)
    {
        parser_url_map_.emplace(it->first, );
    }

    return 0;
}

} // namespace parsers
} // namespace core
} // namespace black_library
