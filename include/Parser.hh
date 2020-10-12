#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <atomic>
#include <fstream>
#include <iostream>
#include <string>

#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <Result.hh>
#include <SourceInformation.hh>

namespace librarycore {
class Parser
{
public:
    Parser(std::string url);
    Parser() : Parser(""){};
    virtual ~Parser();

    virtual Result Parse();
    virtual Parser Copy();
    std::string CurlRequest(std::string url);
    xmlNode* GetElementAttr(xmlNode* root, std::string attr, std::string value);

    void SetUrl(std::string url);
    void SetLocalFilePath(std::string local_des);
    void ParseUrl();

    std::string GetLocalDes();
    std::string GetTitle();
    std::string GetUrl();
    parser_rep GetSource();

protected:
    virtual Result ParseChapter(int chap_num);
    virtual Result ParseChapter(std::string url);

    std::string local_des;
    std::string title;
    std::string url;
    parser_rep source;

private:
};

size_t HandleCurlResponse(void* prt, size_t size, size_t nmemb, void* data);

} // namespace librarycore
#endif
