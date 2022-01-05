/**
 * ParserFactory.cc
 */

#include <iostream>
#include <sstream>

#include <LogOperations.h>

#include <ParserCommon.h>

#include <ParserAO3.h>
#include <ParserFFN.h>
#include <ParserRR.h>
#include <ParserSBF.h>
#include <ParserSVF.h>

#include <ParserFactory.h>

namespace black_library {

namespace core {

namespace parsers {

namespace BlackLibraryCommon = black_library::core::common;

ParserFactory::ParserFactory(const njson &config)
{
    njson nconfig = config["config"];

    std::string logger_path = BlackLibraryCommon::DefaultLogPath;
    if (nconfig.contains("logger_path"))
    {
        logger_path = nconfig["logger_path"];
    }

    bool factory_log_level = BlackLibraryCommon::DefaultLogLevel;
    if (nconfig.contains("factory_debug_log"))
    {
        factory_log_level = nconfig["factory_debug_log"];
    }

    BlackLibraryCommon::InitRotatingLogger("parser_factory", logger_path, factory_log_level);
    BlackLibraryCommon::LogInfo("parser_factory", "Initialize ParserFactory");
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
        BlackLibraryCommon::LogError("parser_factory", "Failed to get parser from url: {}", url);
        result.has_error = true;
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
        BlackLibraryCommon::LogError("parser_factory", "Failed to match parser with type: {}", GetParserName(parser_type));
        result.has_error = true;
        return result;
    }

    result = parser_map_itr->second();

    BlackLibraryCommon::LogDebug("parser_factory", "Got parser: {}", GetParserName(result.parser_result->GetParserType()));

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

    BlackLibraryCommon::LogInfo("parser_factory", "Initialized parser map with {} elements", parser_map_.size());

    return 0;
}

} // namespace parsers
} // namespace core
} // namespace black_library
