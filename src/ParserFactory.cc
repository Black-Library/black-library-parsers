/**
 * ParserFactory.cc
 */

#include <ParserFactory.h>

#include "ParserAO3.h"
#include "ParserFFN.h"
#include "ParserRR.h"

namespace black_library {

namespace core {

namespace parsers {

namespace {

// C++ wrapper around strstr()
bool ContainsString(const std::string &haystack, const std::string &needle)
{
    // C-style compare is fastest
    if (strstr(haystack.c_str(), needle.c_str()) == NULL)
    {
        return false;
    }
    
    return true;
}

} // namespace

ParserFactory::ParserFactory()
{
    InitParserMap();
    InitParserUrlMap();
}

ParserFactory::~ParserFactory()
{

}

ParserFactoryResult ParserFactory::GetParser(const std::string &url)
{
    ParserFactoryResult result;
    parser_rep parser_type = _NUM_PARSERS_TYPE;

    bool url_found = false;

    for (auto it = parser_url_map_.begin(); it != parser_url_map_.end(); ++it)
    {
        if (ContainsString(url, it->first))
        {
            parser_type = it->second;
            url_found = true;
            break;
        }
    }

    if (!url_found)
    {
        result.has_error = true;
        result.result_error.error_string = "Error: ParserFactory could not match url\n";
        return result;
    }

    auto parser_map_itr = parser_map_.find(parser_type);

    if (parser_map_itr == parser_map_.end())
    {
        result.has_error = true;
        result.result_error.error_string = "Error: ParserFactory could not match parser\n";
        return result;
    }

    // TODO: check if the .Copy method is necessary
    result.parser_result = parser_map_itr->second.Copy();

    result.parser_result.SetUrl(url);
    return result;
}

int ParserFactory::InitParserMap()
{
    AO3::ParserAO3 parserAO3;
    // FFN::ParserFFN parserFFN;
    RR::ParserRR parserRR;
    parser_map_.emplace(AO3_PARSER, parserAO3);
    // parser_map_.emplace(FFN_PARSER, parserFFN);
    parser_map_.emplace(RR_PARSER, parserRR);
    

    return 0;
}

int ParserFactory::InitParserUrlMap()
{
    for (auto it = parser_map_.begin(); it != parser_map_.end(); ++it)
    {
        parser_url_map_.emplace(it->second.GetSourceUrl(), it->first);
    }

    return 0;
}

} // namespace parsers
} // namespace core
} // namespace black_library
