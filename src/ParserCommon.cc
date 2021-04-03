/**
 * ParserCommon.cc
 */

#include <string.h>

#include <ParserCommon.h>
#include <SourceInformation.h>

namespace black_library {

namespace core {

namespace parsers {

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

// c level string check is faster for smaller strings then
// needle.find(haystack) != std::string::npos
parser_rep GetParserTypeByUrl(const std::string &url)
{
    parser_rep rep = _NUM_PARSERS_TYPE;

    if (ContainsString(AO3::source_url, url))
    {
        rep = AO3_PARSER;
    }
    else if (ContainsString(FFN::source_url, url))
    {
        rep = FFN_PARSER;
    }
    else if (ContainsString(SBF::source_url, url))
    {
        rep = SBF_PARSER;
    }
    else if (ContainsString(RR::source_url, url))
    {
        rep = RR_PARSER;
    }
    return rep;
}

} // namespace parsers
} // namespace core
} // namespace black_library
