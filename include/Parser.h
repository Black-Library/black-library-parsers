/**
 * Parser.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <string>

#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SourceInformation.h"

namespace black_library {

namespace core {

namespace parsers {

class Parser
{
public:
    Parser(const std::string &url);
    Parser() : Parser(""){};
    virtual ~Parser();

    virtual void Parse();
    virtual Parser Copy();
    std::string CurlRequest(std::string url);
    xmlNode* GetElementAttr(xmlNode* root, std::string attr, std::string value);

    void SetUrl(std::string url);
    void SetLocalFilePath(std::string local_des_);
    void ParseUrl();

    std::string GetLocalDes();
    std::string GetTitle();
    std::string GetUrl();
    parser_rep GetSource();

protected:
    std::string local_des_;
    std::string title_;
    std::string url_;
    parser_rep source_;

private:
};

size_t HandleCurlResponse(void* prt, size_t size, size_t nmemb, void* data);

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
