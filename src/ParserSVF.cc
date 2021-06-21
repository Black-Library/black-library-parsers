/**
 * ParserSVF.cc
 */

#include <chrono>
#include <iostream>
#include <locale>
#include <sstream>
#include <thread>

#include <FileOperations.h>

#include <ParserSVF.h>

namespace black_library {

namespace core {

namespace parsers {

namespace SVF {

namespace BlackLibraryCommon = black_library::core::common;

ParserSVF::ParserSVF() :
    ParserXF(parser_t::SVF_PARSER)
{
    title_ = "SVF_parser_title";
    source_name_ = BlackLibraryCommon::SVF::source_name;
    source_url_ = BlackLibraryCommon::SVF::source_url;
    author_ = "unknown-author";
}

} // namespace SVF
} // namespace parsers
} // namespace core
} // namespace black_library
