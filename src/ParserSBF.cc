/**
 * ParserSBF.cc
 */

#include <ParserSBF.h>

namespace black_library {

namespace core {

namespace parsers {

namespace SBF {

namespace BlackLibraryCommon = black_library::core::common;

ParserSBF::ParserSBF() :
    ParserXF(parser_t::SBF_PARSER)
{
    title_ = "SBF_parser_title";
    source_name_ = BlackLibraryCommon::SBF::source_name;
    source_url_ = BlackLibraryCommon::SBF::source_url;
}

} // namespace SBF
} // namespace parsers
} // namespace core
} // namespace black_library
