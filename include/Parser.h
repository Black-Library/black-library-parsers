/**
 * Parser.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_H__

#include <string.h>

#include <atomic>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>

#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/tree.h>

#include <SourceInformation.h>

#include "ParserCommon.h"
#include "ParserTimeGenerator.h"

namespace black_library {

namespace core {

namespace parsers {

class Parser
{
public:
    Parser(parser_t parser_type);
    Parser() : Parser(parser_t::ERROR_PARSER){};
    Parser(const Parser &parser);
    virtual ~Parser() = default;

    ParserResult Parse(const ParserJob &parser_job);
    void Stop();

    std::string CurlRequest(const std::string &url);

    void SetLocalFilePath(const std::string &local_des);

    bool GetDone();
    parser_t GetParserType();
    std::string GetSourceName();
    std::string GetSourceUrl();

    int RegisterProgressNumberCallback(const progress_number_callback &callback);

protected:
    virtual std::string AppendTargetUrl(const std::string &job_url);
    virtual ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    virtual void FindChapterNodes(xmlNodePtr root_node);
    virtual void FindMetaData(xmlNodePtr root_node);
    virtual ParserChapterInfo ParseChapter(const ParserIndexEntry &index_entry);

    std::vector<ParserIndexEntry> index_entries_;
    std::shared_ptr<ParserTimeGenerator> time_generator_;
    progress_number_callback progress_number_callback_;

    std::string title_;
    std::string nickname_;
    std::string source_name_;
    std::string source_url_;
    std::string author_;

    std::string target_url_;
    std::string uuid_;
    std::string local_des_;
    std::mutex mutex_;
    parser_t parser_type_;
    std::atomic_bool done_;

private:
};

size_t HandleCurlResponse(void* prt, size_t size, size_t nmemb, void* data);

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
