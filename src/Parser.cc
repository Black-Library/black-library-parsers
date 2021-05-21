/**
 * Parser.cc
 */

#include <chrono>
#include <functional>
#include <iostream>

#include <Parser.h>

namespace black_library {

namespace core {

namespace parsers {

Parser::Parser(parser_rep parser_type)
{
    parser_type_ = parser_type;

    std::random_device rd;
    std::mt19937_64::result_type seed = rd() ^ (
            (std::mt19937_64::result_type)
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
                ).count() +
            (std::mt19937_64::result_type)
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
                ).count() );

    generator_ = std::mt19937_64(seed);
    distribution_ = std::uniform_int_distribution<int>(0, 2);
    done_ = false;
}

Parser::Parser(const Parser &parser) :
    generator_(parser.generator_),
    distribution_(parser.distribution_),
    parser_type_(parser.parser_type_),
    done_(bool(parser.done_))
{
}

ParserResult Parser::Parse(const ParserJob &parser_job)
{
    (void) parser_job;
    const std::lock_guard<std::mutex> lock(mutex_);
    ParserResult parser_result;

    std::cout << parser_job.url << " - parse" << std::endl;

    return parser_result;
}

void Parser::Stop()
{
    done_ = true;
}

std::string Parser::CurlRequest(const std::string &url)
{
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();

    if (!curl)
    {
        std::cout << "Error: curl did not initialize" << std::endl;
        return "";
    }

    std::string html_raw;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HandleCurlResponse);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html_raw);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

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

void Parser::SetLocalFilePath(const std::string &local_des)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    local_des_ = local_des;
}

bool Parser::GetDone()
{
    return done_;
}

std::string Parser::GetLocalDes()
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return local_des_;
}

parser_rep Parser::GetParserType()
{
    return parser_type_;
}

std::string Parser::GetSourceUrl()
{
    return source_url_;
}

std::string Parser::GetTitle()
{
    return title_;
}

size_t Parser::GenerateWaitTime(size_t length)
{
    size_t wait_time = 0;

    size_t loops = length;

    if (length <= 20)
        loops = 40;

    for (size_t i = 0; i < loops; ++i)
    {
        wait_time += 5 + distribution_(generator_);
    }

    return wait_time;
}

ParserIndexEntry Parser::ExtractIndexEntry(xmlNodePtr root_node)
{
    (void) root_node;
    ParserIndexEntry entry;
    return entry;
}

void Parser::FindMetaData(xmlNodePtr root_node)
{
    (void) root_node;
}

ParserChapterInfo Parser::ParseChapter(const ParserIndexEntry &entry)
{
    (void) entry;
    ParserChapterInfo info;
    return info;
}

// Credit: https://stackoverflow.com/questions/5525613/how-do-i-fetch-a-html-page-source-with-libcurl-in-c
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

} // namespace parsers
} // namespace core
} // namespace black_library
