/**
 * Parser.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_H__

#include <string.h>

#include <atomic>
#include <fstream>
#include <mutex>
#include <random>
#include <string>

#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <SourceInformation.h>

#include "ParserCommon.h"

namespace black_library {

namespace core {

namespace parsers {

class Parser
{
public:
    Parser(parser_rep parser_type);
    Parser() : Parser(ERROR_PARSER){};
    Parser(const Parser &parser);
    virtual ~Parser() = default;

    virtual ParserResult Parse(const ParserJob &parser_job);
    void Stop();

    std::string CurlRequest(const std::string &url);
    xmlNode* GetElementAttr(xmlNode* root, std::string attr, std::string value);

    void SetLocalFilePath(const std::string &local_des);

    bool GetDone();
    std::string GetLocalDes();
    parser_rep GetParserType();
    std::string GetSourceUrl();
    std::string GetTitle();

    size_t GenerateWaitTime(size_t length);

protected:
    virtual ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    virtual void FindMetaData(xmlNodePtr root_node);
    virtual ParserChapterInfo ParseChapter(const ParserIndexEntry &entry);
    virtual ParserXmlNodeSeek SeekToChapterContent(xmlNodePtr root_node);

    std::mt19937_64 generator_;
    std::uniform_int_distribution<int> distribution_;
    std::vector<ParserIndexEntry> index_entries_;

    std::string title_;
    std::string nickname_;
    std::string source_url_;
    std::string author_;

    std::string uuid_;
    std::string local_des_;
    std::mutex mutex_;
    parser_rep parser_type_;
    std::atomic_bool done_;

private:
};

size_t HandleCurlResponse(void* prt, size_t size, size_t nmemb, void* data);

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
