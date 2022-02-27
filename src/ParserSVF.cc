/**
 * ParserSVF.cc
 */

#include <ParserSVF.h>

namespace black_library {

namespace core {

namespace parsers {

namespace SVF {

namespace BlackLibraryCommon = black_library::core::common;

ParserSVF::ParserSVF() :
    ParserXF(parser_t::SVF_PARSER)
{
    source_name_ = BlackLibraryCommon::SVF::source_name;
    source_url_ = BlackLibraryCommon::SVF::source_url;
}

} // namespace SVF
} // namespace parsers
} // namespace core
} // namespace black_library
