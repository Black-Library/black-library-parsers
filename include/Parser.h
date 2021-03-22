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

struct xmlAttributePayload {
    std::string result = "";
    bool is_null = false;
    bool attribute_found = false;
};

class Parser
{
public:
    Parser(parser_rep parser_type);
    Parser() : Parser(_NUM_PARSERS_TYPE){};
    virtual ~Parser() = default;

    virtual void Parse();
    virtual Parser Copy();

    std::string CurlRequest(const std::string &url);
    xmlNode* GetElementAttr(xmlNode* root, std::string attr, std::string value);

    void SetParserType(parser_rep source);
    void SetSourceUrl(const std::string &url);
    void SetUrl(const std::string &url);
    void SetLocalFilePath(const std::string &local_des);

    std::string GetLocalDes();
    parser_rep GetParserType();
    std::string GetSourceUrl();
    std::string GetTitle();
    std::string GetUrl();

    size_t GenerateWaitTime(size_t length);
    bool NodeHasAttributeContent(xmlNodePtr root_node, const std::string &target_content);
    std::string TrimWhitespace(const std::string &target_string);

protected:
    virtual std::string ParseTitle();
    virtual std::string ParseAuthor();

    std::string GenerateXmlDocTreeString(xmlNode *root_node);
    std::string GenerateXmlDocTreeStringHelper(xmlNode *root_node, size_t depth);
    std::string GetChapterFileName(size_t index, const std::string &chapter_name);
    std::string GetSpaceString(size_t num_tabs);
    xmlAttributePayload GetXmlAttributeContentByName(xmlAttrPtr &attribute_ptr, const std::string &name);

    std::default_random_engine generator_;
    std::uniform_int_distribution<int> distribution_;

    std::string title_;
    std::string nickname_;
    std::string source_url_;
    std::string url_;
    std::string author_;

    std::string local_des_;

    parser_rep parser_type_;

private:
};

size_t HandleCurlResponse(void* prt, size_t size, size_t nmemb, void* data);

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
