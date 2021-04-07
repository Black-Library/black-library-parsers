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

struct ParserChapterInfo {
    size_t length = 0;
    bool has_error = false;
};

struct ParserIndexEntry {
    std::string data_url;
    std::string chapter_name;
    uint16_t index_num;
    // TODO add date added
};

struct ParserJob {
    std::string url;
    size_t starting_chapter;
};

struct ParserJobResult {
    std::string io_result;
    std::string error_string;
    bool has_error = false;
};

struct ParserResult {
    std::string io_result;
    std::string error_string;
    bool has_error = false;
};

struct ParserXmlAttributePayload {
    std::string result = "";
    bool is_null = false;
    bool attribute_found = false;
};

struct ParserXmlNodeSeek {
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
