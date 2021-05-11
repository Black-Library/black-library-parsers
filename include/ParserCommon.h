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

typedef enum {
    JOB_ERROR,
    JOB_QUEUED,
    JOB_WORKING,
    JOB_FINISHED,

    _NUM_JOB_STATUS_TYPES_
} job_status_t;

typedef uint8_t job_status_rep;

typedef enum {
    ERROR_PARSER,
    AO3_PARSER,
    FFN_PARSER,
    SBF_PARSER,
    RR_PARSER,

    _NUM_PARSERS_TYPE
} parser_t;

typedef uint8_t parser_rep;

struct ParserChapterInfo {
    size_t length = 0;
    bool has_error = true;
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
    size_t start_chapter = 1;
};

struct ParserResultMetadata {
    std::string uuid;
    std::string title;
    std::string author;
    std::string nickname = "";
    std::string source;
    std::string url;
    std::string last_url;
    std::string series;
    uint16_t series_length = 1;
    std::string media_path;
    std::string update_date;
};

struct ParserJobResult {
    ParserResultMetadata metadata;

    std::string debug_string;
    bool has_error = true;
};

struct ParserResult {
    ParserResultMetadata metadata;

    std::string io_result;
    std::string error_string;
    bool has_error = true;
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


// TODO check using vs typedef
using database_status_callback = std::function<void(ParserJobResult)>;
using job_status_callback = std::function<void(const std::string &uuid, job_status_rep job_status)>;
using manager_notify_callback = std::function<void(ParserJobResult)>;

bool ContainsString(const std::string &haystack, const std::string &needle);

std::string GenerateXmlDocTreeString(xmlNodePtr root_node);
std::string GenerateXmlDocTreeStringHelper(xmlNodePtr root_node, size_t depth);

std::string GetChapterFileName(size_t index, const std::string &chapter_name);
std::string GetParserName(parser_rep rep);
parser_rep GetParserTypeByUrl(const std::string &url);
std::string GetSpaceString(size_t num_tabs);
ParserXmlAttributePayload GetXmlAttributeContentByName(xmlAttrPtr &attribute_ptr, const std::string &name);

bool NodeHasAttributeContent(xmlNodePtr root_node, const std::string &target_content);
ParserXmlNodeSeek SeekToNodeByName(xmlNodePtr root_node, const std::string &name);

std::string TrimWhitespace(const std::string &target_string);

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
