/**
 * ParserCommon.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_COMMON_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_COMMON_H__

#include <functional>
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
    std::string uuid;
    std::string url;
    size_t starting_chapter;
};

struct ParserJobResult {
    std::string uuid;
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
    JOB_QUEUED,
    JOB_WORKING,
    JOB_FINISHED,
    JOB_ERROR,
    _NUM_JOB_STATUS_TYPES_
} job_status_t;

typedef uint8_t job_status_rep;

typedef enum {
    AO3_PARSER,
    FFN_PARSER,
    SBF_PARSER,
    RR_PARSER,

    _NUM_PARSERS_TYPE
} parser_t;

typedef uint8_t parser_rep;

typedef std::function<void(ParserJobResult)> database_status_callback;
typedef std::function<void(const std::string &uuid, job_status_rep job_status)> job_status_callback;
typedef std::function<void(ParserJobResult)> manager_notify_callback;

bool ContainsString(const std::string &haystack, const std::string &needle);

std::string GenerateXmlDocTreeString(xmlNodePtr root_node);
std::string GenerateXmlDocTreeStringHelper(xmlNodePtr root_node, size_t depth);

std::string GetChapterFileName(size_t index, const std::string &chapter_name);
std::string GetParserName(parser_rep rep);
parser_rep GetParserTypeByUrl(const std::string &url);
std::string GetSpaceString(size_t num_tabs);
ParserXmlAttributePayload GetXmlAttributeContentByName(xmlAttrPtr &attribute_ptr, const std::string &name);

bool NodeHasAttributeContent(xmlNodePtr root_node, const std::string &target_content);

std::string TrimWhitespace(const std::string &target_string);

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
