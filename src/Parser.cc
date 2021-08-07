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

Parser::Parser(parser_t parser_type) :
    progress_number_callback_(),
    time_generator_(std::make_shared<ShortTimeGenerator>()),
    title_("ERROR_parser_title"),
    nickname_(""),
    source_name_(BlackLibraryCommon::ERROR::source_name),
    source_url_(BlackLibraryCommon::ERROR::source_url),
    author_("unknown-author"),
    index_(0),
    end_index_(0),
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

    target_url_ = PreprocessTargetUrl(parser_job);

    std::cout << "Start " << GetParserName(parser_type_) << " Parse: " << parser_job << std::endl;

    const auto curl_result = network_.get()->NetworkRequest(target_url_);
    if (curl_result.has_error)
    {
        std::cout << curl_result.debug_string << std::endl;
        return parser_result;
    }

    xmlDocPtr doc_tree = htmlReadDoc((xmlChar*) curl_result.html.c_str(), NULL, NULL,
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

    if (PreParseLoop(current_node, parser_job))
    {
        return parser_result;
    }

    // reset current node ptr to root node children
    current_node = root_node->children;

    SaveMetaData(parser_result);

    if (CalculateIndexBounds(parser_job))
    {
        return parser_result;
    }

    xmlFreeDoc(doc_tree);

    ParseLoop(parser_result);

    PostParseLoop(parser_result);

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

int Parser::CalculateIndexBounds(const ParserJob &parser_job)
{
    index_ = parser_job.start_number - 1;
    end_index_ = parser_job.end_number -1;

    return 0;
}

void Parser::ExpendedAttempts()
{
    done_ = true;;
}

void Parser::FindMetaData(xmlNodePtr root_node)
{
    (void) root_node;
}

ParseSectionInfo Parser::ParseSection()
{
    ParseSectionInfo parse_section_info;
    return parse_section_info;
}

void Parser::ParseLoop(ParserResult &parser_result)
{
    size_t seconds_counter = 0;
    size_t wait_time = 0;
    size_t wait_time_total = 0;
    size_t remaining_attempts = 5;

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
            if (ReachedEnd())
            {
                done_ = true;
                std::cout << GetParserName(parser_type_) << " - " << uuid_ << " reached end" << std::endl;
                continue;
            }

            if (remaining_attempts <= 0)
            {
                std::cout << "Error: " << GetParserName(parser_type_) << " failed to parse section - index: " << index_ << std::endl;
                remaining_attempts = 5;
                ExpendedAttempts();
                continue;
            }

            ParseSectionInfo parse_section_info = ParseSection();
            --remaining_attempts;

            wait_time = time_generator_->GenerateWaitTime(parse_section_info.length);
            wait_time_total += wait_time;

            if (parse_section_info.has_error)
            {
                std::cout << "Error: " << GetParserName(parser_type_) << " failed to parse section - index: " << index_ << " - remaining attempts: " << remaining_attempts
                        << " - waiting " << wait_time << " seconds - wait time total: "  << wait_time_total << " seconds" << std::endl;

                if (remaining_attempts == 0 && progress_number_callback_)
                    progress_number_callback_(uuid_, index_ + 1, true);
            }
            else
            {
                std::cout << GetParserName(parser_type_) << ": " << title_ << " - " << index_ << " section length is " << parse_section_info.length
                        << " - waiting " << wait_time << " seconds - wait time total: "  << wait_time_total << " seconds" << std::endl;

                if (progress_number_callback_)
                    progress_number_callback_(uuid_, index_ + 1, false);

                parser_result.metadata.series_length = index_ + 1;

                remaining_attempts = 5;
                ++index_;
            }
        }

        ++seconds_counter;

        std::this_thread::sleep_until(deadline);
    }
}

void Parser::PostParseLoop(ParserResult &parser_result)
{
    (void) parser_result;
    return;
}

int Parser::PreParseLoop(xmlNodePtr root_node, const ParserJob &parser_job)
{
    (void) root_node;
    (void) parser_job;
    return 1;
}

std::string Parser::PreprocessTargetUrl(const ParserJob &parser_job)
{
    return parser_job.url;
}

bool Parser::ReachedEnd()
{
    return true;
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

} // namespace parsers
} // namespace core
} // namespace black_library
