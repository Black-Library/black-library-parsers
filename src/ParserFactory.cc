/**
 * ParserFactory.cc
 */

#include <sstream>

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
    std::cout << "Initialize ParserFactory" << std::endl;
    InitParserMap();
    InitParserUrlMap();
}

ParserFactory::~ParserFactory()
{

}

ParserFactoryResult ParserFactory::GetParser(const std::string &url)
{
    ParserFactoryResult result;
    std::stringstream ss;
    parser_rep parser_type = _NUM_PARSERS_TYPE;

    bool url_found = false;

    for (auto it = parser_url_map_.begin(); it != parser_url_map_.end(); ++it)
    {
        if (ContainsString(url, it->first))
        {
            parser_type = it->second;
            url_found = true;
            ss << "Matched url: " << url << " with: " << it->first << std::endl;
            break;
        }
    }

    if (!url_found)
    {
        result.has_error = true;
        result.error_string = "Error: ParserFactory could not match url\n";
        result.io_string = ss.str();
        return result;
    }

    auto parser_map_itr = parser_map_.find(parser_type);

    if (parser_map_itr == parser_map_.end())
    {
        result.has_error = true;
        result.error_string = "Error: ParserFactory could not match parser\n";
        result.io_string = ss.str();
        return result;
    }

    result.parser_result = parser_map_itr->second;

    ss << "Got Parser: " << GetParserName(result.parser_result->GetParserType()) << std::endl;

    result.parser_result->SetUrl(url);

    result.io_string = ss.str();

    return result;
}

ParserFactoryResult ParserFactory::GetParserByType(parser_rep parser_type)
{
    ParserFactoryResult result;
    std::stringstream ss;

    auto parser_map_itr = parser_map_.find(parser_type);

    if (parser_map_itr == parser_map_.end())
    {
        result.has_error = true;
        result.error_string = "Error: ParserFactory could not match parser\n";
        result.io_string = ss.str();
        return result;
    }

    result.parser_result = parser_map_itr->second;

    ss << "Got Parser: " << GetParserName(result.parser_result->GetParserType()) << std::endl;

    result.io_string = ss.str();

    return result;
}

int ParserFactory::InitParserMap()
{
    parser_map_.emplace(AO3_PARSER, std::static_pointer_cast<Parser>(std::make_shared<AO3::ParserAO3>()));
    parser_map_.emplace(RR_PARSER, std::static_pointer_cast<Parser>(std::make_shared<RR::ParserRR>()));

    std::cout << "first parser\n" << std::endl;

    auto iter = parser_map_.find(RR_PARSER);

    auto parser_got = iter->second;

    std::shared_ptr<RR::ParserRR> rr_parser = std::make_shared<RR::ParserRR>();
    std::shared_ptr<Parser> normal_parser;

    std::cout << "static cast\n" << std::endl;

    normal_parser = std::static_pointer_cast<Parser>(rr_parser);
    normal_parser->Parse();

    std::cout << "dynamic cast\n" << std::endl;

    auto downcasted = std::dynamic_pointer_cast<RR::ParserRR>(normal_parser);
    downcasted->Parse();

    std::cout << "static cast from map\n" << std::endl;

    auto static_cast_from_map = std::static_pointer_cast<Parser>(parser_got);
    static_cast_from_map->Parse();

    // std::cout << "Parser iter: " << GetParserName(iter->first) << std::endl;

    // std::cout << "Parser source url: " << parser_got.GetSourceUrl() << std::endl;
    // std::cout << "Parser type: " << GetParserName(parser_got.GetParserType()) << std::endl;
    // std::cout << "second parser\n" << std::endl;
    // parser_got.Parse();
    std::cout << "end parser test\n" << std::endl;

    std::cout << "Initialized Parser map with " << parser_map_.size() << " elements" << std::endl;

    return 0;
}

int ParserFactory::InitParserUrlMap()
{
    std::cout << "Initialize Parser url map with " << parser_map_.size() << " elements" << std::endl;

    for (auto it = parser_map_.begin(); it != parser_map_.end(); ++it)
    {
        parser_url_map_.emplace((*it->second).GetSourceUrl(), it->first);
    }

    return 0;
}

} // namespace parsers
} // namespace core
} // namespace black_library
