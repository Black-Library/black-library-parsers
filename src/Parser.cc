/**
 * Parser.cc
 */

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

#include <Parser.h>
#include <ShortTimeGenerator.h>

namespace black_library {

namespace core {

namespace parsers {

namespace BlackLibraryCommon = black_library::core::common;

Parser::Parser(parser_t parser_type)
{
    time_generator_ = std::make_shared<ShortTimeGenerator>();
    nickname_ = "";
    source_name_ = BlackLibraryCommon::ERROR::source_name;
    parser_type_ = parser_type;
    done_ = false;
}

Parser::Parser(const Parser &parser) :
    parser_type_(parser.parser_type_),
    done_(bool(parser.done_))
{
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

    target_url_ = AppendTargetUrl(parser_job.url);

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

    ParserXmlNodeSeek body_seek = SeekToNodeByName(current_node, "body");

    if (!body_seek.found)
    {
        std::cout << "Could not find chapter index, exiting" << std::endl;
        xmlFreeDoc(doc_tree);
        return parser_result;
    }

    current_node = body_seek.seek_node;

    std::cout << GetParserName(parser_type_) << ": Find chapter nodes" << std::endl;

    FindChapterNodes(current_node->children);

    std::cout << "\tTitle: " << title_ << std::endl;
    std::cout << "\tAuthor: " << author_ << std::endl;
    std::cout << "\tNickname: " << nickname_ << std::endl;

    parser_result.metadata.title = title_;
    parser_result.metadata.author = author_;
    parser_result.metadata.nickname = nickname_;
    parser_result.metadata.source = GetSourceName();

    xmlFreeDoc(doc_tree);

    std::cout << GetParserName(parser_type_) << ": Found " << index_entries_.size() << " nodes" << std::endl;

    size_t index;
    size_t end_index;

    if (parser_job.start_number > index_entries_.size())
    {
        index = 0;
    }
    else
    {
        index = parser_job.start_number - 1;
    }

    if (parser_job.start_number <= parser_job.end_number && index_entries_.size() >= parser_job.end_number)
    {
        end_index = parser_job.end_number - 1;
    }
    else
    {
        end_index = index_entries_.size() - 1;
    }

    if (index > index_entries_.size() || index_entries_.empty())
    {
        std::cout << "Error: " <<  GetParserName(parser_type_) << " requested start index greater than detected entries" << std::endl;
        return parser_result;
    }

    size_t seconds_counter = 0;
    size_t wait_time = 0;
    size_t wait_time_total = 0;
    size_t remaining_attempts = 5;

    // TODO: make parser take 8ish hour break
    while (!done_)
    {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

        if (done_)
            break;

        if (seconds_counter >= wait_time)
            seconds_counter = 0;

        if (seconds_counter == 0)
        {
            // let the fake reader finish waiting before exiting
            if (index > end_index)
            {
                done_ = true;
                std::cout << GetParserName(parser_type_) << " - " << uuid_ << " reached end" << std::endl;
                continue;
            }

            if (remaining_attempts <= 0)
            {
                std::cout << "Error: " << GetParserName(parser_type_) << " failed to parse chapter entry index: " << index << std::endl;
                remaining_attempts = 5;
                ++index;
                continue;
            }

            ParserChapterInfo chapter_parse_info = ParseChapter(index_entries_[index]);
            --remaining_attempts;

            wait_time = time_generator_->GenerateWaitTime(chapter_parse_info.length);
            wait_time_total += wait_time;

            if (chapter_parse_info.has_error)
            {
                std::cout << "Error: " << GetParserName(parser_type_) << " failed to parse chapter entry index: " << index << " - remaining attempts: " << remaining_attempts
                        << " - waiting " << wait_time << " seconds - wait time total: "  << wait_time_total << " seconds" << std::endl;

                if (remaining_attempts == 0 && progress_number_callback_)
                    progress_number_callback_(uuid_, index + 1, true);
            }
            else
            {
                std::cout << GetParserName(parser_type_) << ": " << title_ << " - " << index << " chapter length is " << chapter_parse_info.length
                        << " - waiting " << wait_time << " seconds - wait time total: "  << wait_time_total << " seconds" << std::endl;

                if (progress_number_callback_)
                    progress_number_callback_(uuid_, index + 1, false);

                parser_result.metadata.series_length = index + 1;

                remaining_attempts = 5;
                ++index;
            }
        }

        ++seconds_counter;

        std::this_thread::sleep_until(deadline);
    }

    parser_result.metadata.last_url = index_entries_[index_entries_.size() - 1].data_url;

    // find newest update date
    for (const auto & index_entry : index_entries_)
    {
        if (index_entry.time_published > parser_result.metadata.update_date)
            parser_result.metadata.update_date = index_entry.time_published;
    }

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


std::string Parser::AppendTargetUrl(const std::string &job_url)
{
    return job_url;
}

ParserIndexEntry Parser::ExtractIndexEntry(xmlNodePtr root_node)
{
    (void) root_node;
    ParserIndexEntry index_entry;
    return index_entry;
}

void Parser::FindChapterNodes(xmlNodePtr root_node)
{
    (void) root_node;
}

void Parser::FindMetaData(xmlNodePtr root_node)
{
    (void) root_node;
}

ParserChapterInfo Parser::ParseChapter(const ParserIndexEntry &index_entry)
{
    (void) index_entry;
    ParserChapterInfo info;
    return info;
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
