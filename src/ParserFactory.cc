/**
 * ParserFactory.cc
 */

#include <iostream>
#include <sstream>

#include <ParserFactory.h>

#include <ParserCommon.h>

#include "ParserAO3.h"
#include "ParserFFN.h"
#include "ParserRR.h"

namespace black_library {

namespace core {

namespace parsers {

ParserFactory::ParserFactory()
{
    std::cout << "Initialize ParserFactory" << std::endl;
    InitParserMap();
}

ParserFactory::~ParserFactory()
{

}

ParserFactoryResult ParserFactory::GetParserByUrl(const std::string &url)
{
    ParserFactoryResult result;
    std::stringstream ss;

    auto parser_type = GetParserTypeByUrl(url);

    if (parser_type == ERROR_PARSER)
    {
        result.has_error = true;
        result.error_string = "Error: ParserFactory could not match url\n";
        result.io_string = ss.str();
        return result;
    }

    return GetParserByType(parser_type);
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

    ss << "Got Parser: " << GetParserName(result.parser_result->GetParserType());

    result.io_string = ss.str();

    return result;
}

int ParserFactory::InitParserMap()
{
    parser_map_.emplace(AO3_PARSER, std::static_pointer_cast<Parser>(std::make_shared<AO3::ParserAO3>()));
    parser_map_.emplace(RR_PARSER, std::static_pointer_cast<Parser>(std::make_shared<RR::ParserRR>()));

    std::cout << "Initialized Parser map with " << parser_map_.size() << " elements" << std::endl;

    return 0;
}

} // namespace parsers
} // namespace core
} // namespace black_library
