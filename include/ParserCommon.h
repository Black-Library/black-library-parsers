/**
 * ParserCommon.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_COMMON_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_COMMON_H__

#include <string>

#include <libxml/tree.h>

namespace black_library {

namespace core {

namespace parsers {

struct parser_xml_node_seek {
    xmlNodePtr seek_node = NULL;
    bool found = false;
};

typedef enum {
    AO3_PARSER,
    FFN_PARSER,
    SBF_PARSER,
    RR_PARSER,

    _NUM_PARSERS_TYPE
} parser_t;

typedef uint8_t parser_rep;

std::string GetParserName(parser_rep rep);
parser_rep GetParserTypeByUrl(const std::string &url);

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
