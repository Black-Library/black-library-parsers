/**
 * ParserCommon.cc
 */

#include <ParserCommon.h>
#include <SourceInformation.h>

namespace black_library {

namespace core {

namespace parsers {

std::string GetParserName(parser_rep rep)
{
    switch (rep)
    {
    case AO3_PARSER:
        return "AO3_PARSER";
        break;
    case FFN_PARSER:
        return "FFN_PARSER";
        break;
    case SBF_PARSER:
        return "SBF_PARSER";
        break;
    case RR_PARSER:
        return "RR_PARSER";
        break;
    case _NUM_PARSERS_TYPE:
        return "_NUM_PARSERS_TYPE";
        break;
    default:
        return "NO_PARSER";
        break;
    }
}

parser_rep GetParserTypeByUrl(const std::string &url)
{
    parser_rep rep = _NUM_PARSERS_TYPE;

    if (url.find(AO3::source_url) != std::string::npos)
    {
        rep = AO3_PARSER;
    }
    else if (url.find(FFN::source_url) != std::string::npos)
    {
        rep = FFN_PARSER;
    }
    else if (url.find(SBF::source_url) != std::string::npos)
    {
        rep = SBF_PARSER;
    }
    else if (url.find(RR::source_url) != std::string::npos)
    {
        rep = RR_PARSER;
    }
    return rep;
}

} // namespace parsers
} // namespace core
} // namespace black_library
