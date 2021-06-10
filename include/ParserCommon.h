/**
 * ParserCommon.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_COMMON_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_COMMON_H__

#include <time.h>

#include <functional>
#include <optional>
#include <string>

#include <libxml/tree.h>

namespace black_library {

namespace core {

namespace parsers {

enum class job_status_t {
    JOB_ERROR,
    JOB_MANAGER_QUEUED,
    JOB_WORKER_QUEUED,
    JOB_WORKING,
    JOB_FINISHED,

    _NUM_JOB_STATUS_TYPES_
};

enum class parser_t {
    ERROR_PARSER,
    AO3_PARSER,
    FFN_PARSER,
    SBF_PARSER,
    RR_PARSER,

    _NUM_PARSERS_TYPE
};

enum class pattern_seek_t {
    XML_NAME,
    XML_ATTRIBUTE,
    XML_CONTENT
};

struct ParserChapterInfo {
    size_t length = 0;
    bool has_error = true;
};

struct ParserIndexEntry {
    std::string data_url;
    std::string chapter_name;
    time_t time_published = 0;
    size_t index_num;
};

struct ParserJob {
    std::string uuid;
    std::string url;
    size_t start_number = 1;
    size_t end_number = 0;
};

struct ParserJobHash
{
    std::size_t operator() (const ParserJob &parser_job) const
    {
        std::size_t h1 = std::hash<std::string>()(parser_job.uuid);
        std::size_t h2 = std::hash<std::string>()(parser_job.url);
        std::size_t h3 = std::hash<size_t>()(parser_job.start_number);
        std::size_t h4 = std::hash<size_t>()(parser_job.end_number);

        return h1 ^ h2 ^ h3 ^ h4;
    }
};

inline bool operator == (const ParserJob &left, const ParserJob &right)
{
    return left.uuid == right.uuid && left.url == right.url && 
        left.start_number == right.start_number && left.end_number == right.end_number;
}

inline std::ostream& operator << (std::ostream &o, const ParserJob &parser_job)
{
    o << "uuid: " << parser_job.uuid << " ";
    o << "url: " << parser_job.url << " ";
    o << "start_number: " << parser_job.start_number << " ";
    o << "end_number: " << parser_job.end_number;

    return o;
}

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
    time_t update_date = 0;
};

struct ParserJobResult {
    ParserResultMetadata metadata;

    size_t start_number = 0;
    size_t end_number = 0;

    std::string debug_string;
    bool has_error = true;
};

struct ParserResult {
    ParserResultMetadata metadata;

    std::string debug_string;
    bool has_error = true;
};

struct ParserXmlAttributeResult {
    std::string result = "";
    bool found = false;
};

struct ParserXmlContentResult {
    std::string result = "";
    bool found = false;
};

struct ParserXmlNodeSeek {
    xmlNodePtr seek_node = NULL;
    bool found = false;
};

struct ParserTimeResult {
    time_t time = {0};
    bool found = false;
};

// TODO check using vs typedef
using database_status_callback = std::function<void(ParserJobResult result)>;
using job_status_callback = std::function<void(const ParserJob &parser_job, job_status_t job_status)>;
using manager_notify_callback = std::function<void(ParserJobResult result)>;
using progress_number_callback = std::function<void(const std::string &uuid, size_t progress_num, bool error)>;

bool ContainsString(const std::string &haystack, const std::string &needle);

std::string GenerateXmlDocTreeString(xmlNodePtr root_node);
std::string GenerateXmlDocTreeStringHelper(xmlNodePtr root_node, size_t depth);

std::string GetChapterFileName(const ParserIndexEntry &entry, const std::string &chapter_name);
std::string GetParserName(parser_t rep);
parser_t GetParserTypeByUrl(const std::string &url);
std::string GetSpaceString(size_t num_tabs);
std::string GetStatusName(job_status_t job_status);
ParserXmlNodeSeek SeekToNodeByElementAttr(xmlNodePtr root, std::string attr, std::string value);
ParserXmlContentResult GetXmlNodeContent(xmlNodePtr root_node);
ParserXmlAttributeResult GetXmlAttributeContentByName(xmlNodePtr root_node, const std::string &target_name);

bool NodeHasAttribute(xmlNodePtr root_node, const std::string &target_name);
bool NodeHasAttributeContent(xmlNodePtr root_node, const std::string &target_content);

std::string SanitizeFileName(const std::string &file_name);

ParserXmlNodeSeek SeekToNodeByName(xmlNodePtr root_node, const std::string &name);
ParserXmlNodeSeek SeekToNodeByNameRecursive(xmlNodePtr root_node, const std::string &name);
ParserXmlNodeSeek SeekToNodeByPattern(xmlNodePtr root_node, int num, ...);

std::string TrimWhitespace(const std::string &target_string);

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
