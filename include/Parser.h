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

#include "ParserCommon.h"
#include "SourceInformation.h"

namespace black_library {

namespace core {

namespace parsers {

class Parser
{
public:
    Parser(parser_rep parser_type);
    Parser() : Parser(_NUM_PARSERS_TYPE){};
    virtual ~Parser() = default;

    virtual ParserResult Parse();
    virtual ParserResult Parse(size_t start_chapter);
    virtual void Stop();

    std::string CurlRequest(const std::string &url);
    xmlNode* GetElementAttr(xmlNode* root, std::string attr, std::string value);

    void SetLocalFilePath(const std::string &local_des);
    void SetParserIndex(size_t index);
    void SetParserType(parser_rep source);
    void SetUrl(const std::string &url);

    bool GetDone();
    std::string GetLocalDes();
    size_t GetParserIndex();
    parser_rep GetParserType();
    std::string GetSourceUrl();
    std::string GetTitle();
    std::string GetUrl();

    size_t GenerateWaitTime(size_t length);

protected:
    virtual std::string ParseTitle();
    virtual std::string ParseAuthor();

    std::mt19937_64 generator_;
    std::uniform_int_distribution<int> distribution_;

    std::string title_;
    std::string nickname_;
    std::string source_url_;
    std::string url_;
    std::string author_;

    std::string local_des_;
    std::mutex mutex_;
    size_t index_;
    parser_rep parser_type_;
    std::atomic_bool done_;

private:
};

size_t HandleCurlResponse(void* prt, size_t size, size_t nmemb, void* data);

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
