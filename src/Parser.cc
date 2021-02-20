#include <Parser.h>

namespace librarycore {

Parser::Parser(const std::string &url)
{
    this->url_ = url;
    this->local_des_ = "";
    ParseUrl();
}

Parser::~Parser()
{

}

void Parser::Parse()
{

}

std::string Parser::CurlRequest(std::string url)
{
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();

    if(!curl)
    {
        std::cout << "Problems" << std::endl;
        return "";
    }

    std::string html_raw;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HandleCurlResponse);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html_raw);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cout << "Curl Request Failed: " << curl_easy_strerror(res) << std::endl;
        return "";
    }

    curl_easy_cleanup(curl);

    return html_raw;
}

xmlNode* Parser::GetElementAttr(xmlNode* root, std::string attr, std::string value) {
    if (memcmp(root->name, "text", 4) == 0 || memcmp(root->name, "style", 5) == 0 ||
        memcmp(root->name, "script", 6) == 0)
    {
        return NULL;
    }

    xmlAttr* prop = root->properties;
    while (prop)
    {
        if (memcmp(prop->name, attr.c_str(), attr.length()) == 0 &&
            memcmp(prop->children->content, value.c_str(), value.length()) == 0)
        {
            return root;
        }

        prop = prop->next;
    }

    xmlNode* current_node = root->children;

    while (current_node)
    {
        xmlNode* node = GetElementAttr(current_node, attr, value);

        if (node)
        {
            return node;
        }

        current_node = current_node->next;
    }

    return NULL;
}

void Parser::SetUrl(std::string url)
{
    this->url_ = url;
    ParseUrl();
}

void Parser::SetLocalFilePath(std::string local_des_)
{
    this->local_des_ = local_des_;
}

void Parser::ParseUrl()
{
    if (url_.find(AO3::url) != std::string::npos)
    {
        source_ = AO3_PARSER;
    }
    else if (url_.find(FFN::url) != std::string::npos)
    {
        source_ = FFN_PARSER;
    }
    else if (url_.find(SBF::url) != std::string::npos)
    {
        source_ = SBF_PARSER;
    }
}

Parser Parser::Copy()
{
    Parser parser;
    parser.SetUrl(this->GetUrl());
    parser.SetLocalFilePath(this->GetLocalDes());
    return parser;
}

parser_rep Parser::GetSource()
{
    return source_;
}

std::string Parser::GetUrl()
{
    return url_;
}

std::string Parser::GetLocalDes()
{
    return local_des_;
}

std::string Parser::GetTitle()
{
    return title_;
}

//Credit: https://stackoverflow.com/questions/5525613/how-do-i-fetch-a-html-page-source-with-libcurl-in-c
size_t HandleCurlResponse(void* ptr, size_t size, size_t nmemb, void* data)
{
    std::string* str = (std::string*) data;
    char* sptr = (char*) ptr;

    for(size_t x = 0; x < size * nmemb; x++)
    {
      (*str) += sptr[x];
    }

    return size * nmemb;
}

} // namespace librarycore
