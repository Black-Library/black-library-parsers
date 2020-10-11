#ifndef PARSER_H
#define PARSER_H

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

class Parser
{
public:
    Parser(std::string url);
    Parser() : Parser(""){};
    virtual ~Parser();

    virtual void Parse();
    std::string CurlRequest(std::string url);
    xmlNode* GetElementAttr(xmlNode* root, std::string attr, std::string value);

    void SetUrl(std::string url);
    void SetLocalFilePath(std::string local_des);
    void ParseUrl();

    std::string GetUrl();
    std::string GetSource();
    std::string GetLocalDes();
    std::string GetTitle();

    virtual Parser Copy();

protected:
    std::string source;
    std::string url;
    std::string local_des;
    std::string title;

private:
};

size_t HandleCurlResponse(void* prt, size_t size, size_t nmemb, void* data);
#endif
