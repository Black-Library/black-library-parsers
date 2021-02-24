/**
 * ParserCommon.cc
 */

#include <ParserCommon.h>

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

} // namespace parsers
} // namespace core
} // namespace black_library
