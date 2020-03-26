#include "../include/Parser.h"

Parser::Parser(std::string url) {
    this->url = url;
    this->local_des = "";
    parseUrl();
}

Parser::~Parser() {

}

void Parser::parse() {

}

std::string Parser::curlRequest(std::string url) {
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if(!curl) {
        std::cout << "Problems" << std::endl;
        return "";
    }
    std::string html_raw;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handleCurlResponse);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html_raw);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        std::cout << "Curl Request Failed: " << curl_easy_strerror(res) << std::endl;
        return "";
    }
    curl_easy_cleanup(curl);
    return html_raw;
}

xmlNode* Parser::getElementAttr(xmlNode* root, std::string attr, std::string value) {
    if(memcmp(root->name, "text", 4) == 0 || memcmp(root->name, "style", 5) == 0 ||
        memcmp(root->name, "script", 6) == 0) {
        return NULL;
    }

    xmlAttr* prop = root->properties;
    while(prop) {
        if(memcmp(prop->name, attr.c_str(), attr.length()) == 0 &&
            memcmp(prop->children->content, value.c_str(), value.length()) == 0) {
            return root;
        }
        prop = prop->next;
    }

    xmlNode* current_node = root->children;
    while(current_node) {
        xmlNode* node = getElementAttr(current_node, attr, value);
        if(node) {
            return node;
        }
        current_node = current_node->next;
    }
    return NULL;
}

void Parser::setUrl(std::string url) {
    this->url = url;
    parseUrl();
}

void Parser::setLocalFilePath(std::string local_des) {
    this->local_des = local_des;
}

void Parser::parseUrl() {
    if(url.find(AO3::url) != std::string::npos) {
        source = AO3::name;
    }
}

std::string Parser::getSource() {
    return source;
}

std::string Parser::getUrl() {
    return url;
}

std::string Parser::getLocalDes() {
    return local_des;
}

std::string Parser::getTitle() {
    return title;
}

//Credit: https://stackoverflow.com/questions/5525613/how-do-i-fetch-a-html-page-source-with-libcurl-in-c
size_t handleCurlResponse(void* ptr, size_t size, size_t nmemb, void* data) {
    std::string* str = (std::string*) data;
    char* sptr = (char*) ptr;

    for(size_t x = 0; x < size * nmemb; x++) {
      (*str) += sptr[x];
    }

    return size * nmemb;
}
