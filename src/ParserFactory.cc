/**
 * ParserFactory.cc
 */

#include <iostream>
#include <sstream>

#include <ParserFactory.h>

#include <ParserCommon.h>

#include <ParserAO3.h>
#include <ParserFFN.h>
#include <ParserRR.h>
#include <ParserSBF.h>
#include <ParserSVF.h>

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

    auto parser_type = GetParserTypeByUrl(url);

    if (parser_type == parser_t::ERROR_PARSER)
    {
        result.has_error = true;
        result.debug_string = "Error: ParserFactory could not match url\n";
        return result;
    }

    return GetParserByType(parser_type);
}

ParserFactoryResult ParserFactory::GetParserByType(parser_t parser_type)
{
    ParserFactoryResult result;
    std::stringstream ss;

    auto parser_map_itr = parser_map_.find(parser_type);

    if (parser_map_itr == parser_map_.end())
    {
        result.has_error = true;
        result.debug_string = "Error: ParserFactory could not match parser\n";
        return result;
    }

    result = parser_map_itr->second();

    ss << "Got Parser: " << GetParserName(result.parser_result->GetParserType());

    result.debug_string = ss.str();

    return result;
}

int ParserFactory::InitParserMap()
{
    parser_map_.emplace(parser_t::AO3_PARSER,
    [](void){
        ParserFactoryResult result;

        result.parser_result = std::static_pointer_cast<Parser>(std::make_shared<AO3::ParserAO3>());

        return result;
    });
    parser_map_.emplace(parser_t::RR_PARSER,
    [](void){
        ParserFactoryResult result;

        result.parser_result = std::static_pointer_cast<Parser>(std::make_shared<RR::ParserRR>());

        return result;
    });
    parser_map_.emplace(parser_t::SBF_PARSER,
    [](void){
        ParserFactoryResult result;

        result.parser_result = std::static_pointer_cast<Parser>(std::make_shared<SBF::ParserSBF>());

        return result;
    });
    parser_map_.emplace(parser_t::SVF_PARSER,
    [](void){
        ParserFactoryResult result;

        result.parser_result = std::static_pointer_cast<Parser>(std::make_shared<SVF::ParserSVF>());

        return result;
    });

    std::cout << "Initialized Parser map with " << parser_map_.size() << " elements" << std::endl;

    return 0;
}

} // namespace parsers
} // namespace core
} // namespace black_library
