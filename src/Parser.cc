/**
 * Parser.cc
 */

#include <chrono>
#include <functional>
#include <iostream>

#include <Parser.h>
#include <ShortTimeGenerator.h>

namespace black_library {

namespace core {

namespace parsers {

namespace BlackLibraryCommon = black_library::core::common;

Parser::Parser(parser_t parser_type) : 
    progress_number_callback_(),
    time_generator_(std::make_shared<ShortTimeGenerator>()),
    title_("ERROR_parser_title"),
    nickname_(""),
    source_name_(BlackLibraryCommon::ERROR::source_name),
    source_url_(BlackLibraryCommon::ERROR::source_url),
    author_("unknown-author"),
    parser_type_(parser_type),
    parser_behavior_(parser_behavior_t::ERROR),
    done_(false)
{
}

Parser::Parser(const Parser &parser) :
    parser_type_(parser.parser_type_),
    parser_behavior_(parser.parser_behavior_),
    done_(bool(parser.done_))
{
}

Parser::~Parser()
{
    done_ = true;
}

ParserResult Parser::Parse(const ParserJob &parser_job)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    ParserResult parser_result;
    done_ = false;

    uuid_ = parser_job.uuid;

    parser_result.metadata.url = parser_job.url;
    parser_result.metadata.uuid = parser_job.uuid;
    parser_result.metadata.media_path = local_des_;
    parser_result.is_error_job = parser_job.is_error_job;

    target_url_ = PreprocessTargetUrl(parser_job.url);

    std::cout << "Start " << GetParserName(parser_type_) << " Parse: " << parser_job << std::endl;

    const auto curl_result = CurlRequest(target_url_);

    xmlDocPtr doc_tree = htmlReadDoc((xmlChar*) curl_result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (doc_tree == NULL)
    {
        std::cout << "Error: libxml HTMLparser unable to parse: " << parser_job.url << std::endl;
        return parser_result;
    }

    // const xmlChar* encoding = doc_tree->encoding;

    xmlNodePtr root_node = xmlDocGetRootElement(doc_tree);
    xmlNodePtr current_node = root_node->children;

    std::cout << GetParserName(parser_type_) << ": Find metadata" << std::endl;

    FindMetaData(current_node);

    // reset current node ptr to root node children
    current_node = root_node->children;

    SaveMetaData(parser_result);

    xmlFreeDoc(doc_tree);

    ParseLoop(parser_result);

    PostProcessMetaData();

    // save information to parser_result
    SaveLastUrl(parser_result);
    SaveUpdateDate(parser_result);

    parser_result.has_error = false;

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

void Parser::SetLocalFilePath(const std::string &local_des)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    local_des_ = local_des;
}

bool Parser::GetDone()
{
    return done_;
}

parser_behavior_t Parser::GetParserBehaviorType()
{
    return parser_behavior_;
}

parser_t Parser::GetParserType()
{
    return parser_type_;
}

std::string Parser::GetSourceName()
{
    return source_name_;
}

std::string Parser::GetSourceUrl()
{
    return source_url_;
}

int Parser::RegisterProgressNumberCallback(const progress_number_callback &callback)
{
    progress_number_callback_ = callback;

    return 0;
}

void Parser::FindMetaData(xmlNodePtr root_node)
{
    (void) root_node;
}

ParserBehaviorInfo Parser::ParseBehavior()
{
    ParserBehaviorInfo info;
    return info;
}

void Parser::ParseLoop(ParserResult &parser_result)
{
    (void) parser_result;
    return;
}

void Parser::PostProcessMetaData()
{
    return;
}

std::string Parser::PreprocessTargetUrl(const std::string &job_url)
{
    return job_url;
}

void Parser::SaveLastUrl(ParserResult &parser_result)
{
    (void) parser_result;
    return;
}

void Parser::SaveMetaData(ParserResult &parser_result)
{
    std::cout << "\tTitle: " << title_ << std::endl;
    std::cout << "\tAuthor: " << author_ << std::endl;
    std::cout << "\tNickname: " << nickname_ << std::endl;

    parser_result.metadata.title = title_;
    parser_result.metadata.author = author_;
    parser_result.metadata.nickname = nickname_;
    parser_result.metadata.source = GetSourceName();
}

void Parser::SaveUpdateDate(ParserResult &parser_result)
{
    (void) parser_result;
    return;
}

// Credit: https://stackoverflow.com/questions/5525613/how-do-i-fetch-a-html-page-source-with-libcurl-in-c
size_t HandleCurlResponse(void* ptr, size_t size, size_t nmemb, void* data)
{
    std::string* str = (std::string*) data;
    char* sptr = (char*) ptr;

    for (size_t x = 0; x < size * nmemb; ++x)
    {
        (*str) += sptr[x];
    }

    return size * nmemb;
}

} // namespace parsers
} // namespace core
} // namespace black_library
