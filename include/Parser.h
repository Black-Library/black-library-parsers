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

class Parser {
public:
    Parser(std::string url);
    Parser() : Parser(""){};
    virtual ~Parser();

    virtual void parse();
    std::string curlRequest(std::string url);
    xmlNode* getElementAttr(xmlNode* root, std::string attr, std::string value);

    void setUrl(std::string url);
    void setLocalFilePath(std::string local_des);
    void parseUrl();

    std::string getUrl();
    std::string getSource();
    std::string getLocalDes();
    std::string getTitle();

protected:
    std::string source;
    std::string url;
    std::string local_des;
    std::string title;

private:
};

size_t handleCurlResponse(void* prt, size_t size, size_t nmemb, void* data);
#endif
