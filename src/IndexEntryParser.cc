/**
 * IndexEntryParser.cc
 */

#include <functional>
#include <iostream>
#include <thread>

#include <IndexEntryParser.h>
#include <ShortTimeGenerator.h>

namespace black_library {

namespace core {

namespace parsers {

namespace BlackLibraryCommon = black_library::core::common;

IndexEntryParser::IndexEntryParser(parser_t parser_type) : 
    Parser(parser_type),
    index_entries_()
{
    parser_behavior_ = parser_behavior_t::INDEX_ENTRY;
}

ParserResult IndexEntryParser::Parse(const ParserJob &parser_job)
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

    std::cout << GetParserName(parser_type_) << ": Find index entry nodes" << std::endl;

    FindIndexEntries(current_node);

    std::cout << GetParserName(parser_type_) << ": Found " << index_entries_.size() << " nodes" << std::endl;

    SaveMetaData(parser_result);

    xmlFreeDoc(doc_tree);

    if (CalculateIndexBounds(parser_job))
    {
        return parser_result;
    }

    ParseLoop(parser_result);

    PostProcessLoop();

    // save information to parser_result
    SaveLastUrl(parser_result);
    SaveUpdateDate(parser_result);

    parser_result.has_error = false;

    return parser_result;
}

ParserIndexEntry IndexEntryParser::ExtractIndexEntry(xmlNodePtr root_node)
{
    (void) root_node;
    ParserIndexEntry index_entry;
    return index_entry;
}

void IndexEntryParser::FindIndexEntries(xmlNodePtr root_node)
{
    (void) root_node;
}

void IndexEntryParser::ParseLoop(ParserResult &parser_result)
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
            if (index_ > end_index_)
            {
                done_ = true;
                std::cout << GetParserName(parser_type_) << " - " << uuid_ << " reached end" << std::endl;
                continue;
            }

            if (remaining_attempts <= 0)
            {
                std::cout << "Error: " << GetParserName(parser_type_) << " failed to parse index entry - index: " << index_ << std::endl;
                remaining_attempts = 5;
                ++index_;
                continue;
            }

            ParseSectionInfo parse_section_info = ParseSection();
            --remaining_attempts;

            wait_time = time_generator_->GenerateWaitTime(parse_section_info.length);
            wait_time_total += wait_time;

            if (parse_section_info.has_error)
            {
                std::cout << "Error: " << GetParserName(parser_type_) << " failed to parse index entry - index: " << index_ << " - remaining attempts: " << remaining_attempts
                        << " - waiting " << wait_time << " seconds - wait time total: "  << wait_time_total << " seconds" << std::endl;

                if (remaining_attempts == 0 && progress_number_callback_)
                    progress_number_callback_(uuid_, index_ + 1, true);
            }
            else
            {
                std::cout << GetParserName(parser_type_) << ": " << title_ << " - " << index_ << " index entry length is " << parse_section_info.length
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

void IndexEntryParser::SaveLastUrl(ParserResult &parser_result)
{
    parser_result.metadata.last_url = index_entries_[index_entries_.size() - 1].data_url;
}

void IndexEntryParser::SaveUpdateDate(ParserResult &parser_result)
{
    for (const auto & index_entry : index_entries_)
    {
        if (index_entry.time_published > parser_result.metadata.update_date)
            parser_result.metadata.update_date = index_entry.time_published;
    }
}

int IndexEntryParser::CalculateIndexBounds(const ParserJob &parser_job)
{
    if (parser_job.start_number > index_entries_.size())
    {
        index_ = 0;
    }
    else
    {
        index_ = parser_job.start_number - 1;
    }

    if (parser_job.start_number <= parser_job.end_number && index_entries_.size() >= parser_job.end_number)
    {
        end_index_ = parser_job.end_number - 1;
    }
    else
    {
        end_index_ = index_entries_.size() - 1;
    }

    if (index_ > index_entries_.size() || index_entries_.empty())
    {
        std::cout << "Error: " <<  GetParserName(parser_type_) << " requested start index greater than detected entries" << std::endl;
        return -1;
    }

    return 0;
}

} // namespace parsers
} // namespace core
} // namespace black_library
