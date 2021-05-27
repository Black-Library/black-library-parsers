/**
 * ParserRR.cc
 */

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include <ParserRR.h>

namespace black_library {

namespace core {

namespace parsers {

namespace RR {

ParserRR::ParserRR() :
    Parser(RR_PARSER)
{
    title_ = "RR_Parser_title";
    nickname_ = "";
    source_url_ = black_library::core::common::RR::source_url;
    author_ = "unknown author";
}

ParserRR::~ParserRR()
{
    done_ = true;
}

ParserResult ParserRR::Parse(const ParserJob &parser_job)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    ParserResult parser_result;
    done_ = false;

    uuid_ = parser_job.uuid;

    parser_result.metadata.url = parser_job.url;
    parser_result.metadata.uuid = parser_job.uuid;
    parser_result.metadata.media_path = local_des_;

    std::cout << "Start " << GetParserName(parser_type_) << " Parse: " << parser_job.url << std::endl;
    std::string curl_result = CurlRequest(parser_job.url);

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

    ParserXmlNodeSeek head_seek = SeekToNodeByName(current_node, "head");

    if (!head_seek.found)
    {
        std::cout << "Could not find head, exiting" << std::endl;
        xmlFreeDoc(doc_tree);
        return parser_result;
    }

    current_node = head_seek.seek_node;

    std::cout << GetParserName(parser_type_) << ": Find metadata" << std::endl;

    FindMetaData(current_node->children);

    std::cout << "\tTitle: " << title_ << std::endl;
    std::cout << "\tAuthor: " << author_ << std::endl;
    std::cout << "\tNickname: " << nickname_ << std::endl;

    parser_result.metadata.title = title_;
    parser_result.metadata.author = author_;
    parser_result.metadata.nickname = nickname_;
    parser_result.metadata.source = black_library::core::common::RR::name;

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

    xmlFreeDoc(doc_tree);

    std::cout << GetParserName(parser_type_) << ": Found " << index_entries_.size() << " nodes" << std::endl;

    size_t index = parser_job.start_chapter - 1;

    if (index > index_entries_.size())
    {
        std::cout << "Error: " <<  GetParserName(parser_type_) << " requested starting index greater than detected entries" << std::endl;
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
            // TODO: let the fake reader finish waiting before exiting
            if (index + 1 > index_entries_.size())
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

            wait_time = GenerateWaitTime(chapter_parse_info.length);
            wait_time_total += wait_time;

            if (chapter_parse_info.has_error)
            {
                std::cout << "Error: " << GetParserName(parser_type_) << " failed to parse chapter entry index: " << index << " - remaining attempts: " << remaining_attempts
                        << " - waiting " << wait_time << " seconds - wait time total: "  << wait_time_total << " seconds" << std::endl;
            }
            else
            {
                std::cout << GetParserName(parser_type_) << ": " << title_ << " - " << index << " chapter length is " << chapter_parse_info.length
                        << " - waiting " << wait_time << " seconds - wait time total: "  << wait_time_total << " seconds" << std::endl;
                remaining_attempts = 5;
                ++index;
            }
        }

        ++seconds_counter;

        std::this_thread::sleep_until(deadline);
    }

    parser_result.has_error = false;

    return parser_result;
}

void ParserRR::FindChapterNodes(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;

    for (current_node = root_node; current_node; current_node = current_node->next)
    {
        if (current_node->type == XML_ELEMENT_NODE)
        {
            if (!xmlStrcmp(current_node->name, (const xmlChar *)"tbody"))
            {
                xmlNodePtr index_node = NULL;
                uint16_t index_num = 0;
                for (index_node = current_node->children; index_node; index_node = index_node->next)
                {
                    if (index_node->type == XML_ELEMENT_NODE)
                    {
                        ParserIndexEntry index_entry = ExtractIndexEntry(index_node);
                        if (index_entry.data_url.empty())
                            continue;

                        index_entry.index_num = index_num;
                        index_entries_.emplace_back(index_entry);
                        ++index_num;
                    }
                }

                xmlFree(index_node);
            }
        }

        FindChapterNodes(current_node->children);
    }

    xmlFree(current_node);
}

std::string ParserRR::GetRRChapterName(const std::string &data_url)
{
    auto pos = data_url.find_last_of("/");

    return data_url.substr(pos + 1);
}

ParserIndexEntry ParserRR::ExtractIndexEntry(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;
    ParserIndexEntry index_entry;

    auto td_seek = SeekToNodeByName(root_node->children, "td");

    if (!td_seek.found)
    {
        return index_entry;
    }

    current_node = td_seek.seek_node;

    auto a_seek = SeekToNodeByName(current_node->children, "a");

    if (!a_seek.found)
    {
        return index_entry;
    }

    current_node = a_seek.seek_node;

    if (!NodeHasAttribute(current_node, "href"))
    {
        return index_entry;
    }

    auto url_result = GetXmlAttributeContentByName(current_node, "href");

    if (!url_result.found)
    {
        return index_entry;
    }

    index_entry.data_url = url_result.result;

    current_node = current_node->children;

    auto chapter_name_result = GetXmlNodeContent(current_node);

    if (!chapter_name_result.found)
    {
        return index_entry;
    }

    index_entry.chapter_name = chapter_name_result.result;

    return index_entry;
}

void ParserRR::FindMetaData(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;

    for (current_node = root_node; current_node; current_node = current_node->next)
    {
        if (!xmlStrcmp(current_node->name, (const xmlChar *) "meta"))
        {
            auto property_result = GetXmlAttributeContentByName(current_node, "property");
            
            if (!property_result.found)
                continue;

            if (property_result.result == "books:author")
            {
                auto author_result = GetXmlAttributeContentByName(current_node, "content");

                if (!author_result.found)
                    continue;

                author_ = author_result.result;
            }
            else if (property_result.result == "og:url")
            {
                auto title_result = GetXmlAttributeContentByName(current_node, "content");

                if (!title_result.found)
                    continue;

                std::string unprocessed_title = title_result.result;
                size_t found = unprocessed_title.find_last_of("/\\");
                title_ = unprocessed_title.substr(found + 1, unprocessed_title.size());
            }
        }
    }

    xmlFree(current_node);
}

ParserChapterInfo ParserRR::ParseChapter(const ParserIndexEntry &entry)
{
    ParserChapterInfo output;
    std::string chapter_url = "https://www." + source_url_ + entry.data_url;
    std::cout << GetParserName(parser_type_) << " ParseChapter: " << chapter_url << " - " << entry.chapter_name << std::endl;

    std::string chapter_result = CurlRequest(chapter_url);
    xmlDocPtr chapter_doc_tree = htmlReadDoc((xmlChar*) chapter_result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (chapter_doc_tree == NULL)
    {
        std::cout << "Error: libxml HTMLparser unable to parse" << std::endl;
        return output;
    }

    xmlNodePtr root_node = xmlDocGetRootElement(chapter_doc_tree);
    xmlNodePtr current_node = root_node->children;
    xmlNodePtr length_node = NULL;
    size_t length = 0;

    ParserXmlNodeSeek chapter_seek = SeekToChapterContent(current_node);
    if (!chapter_seek.found)
    {
        std::cout << "Error: Failed seek" << std::endl;
        xmlFreeDoc(chapter_doc_tree);
        return output;
    }

    current_node = chapter_seek.seek_node;

    for (length_node = current_node->children; length_node; length_node = length_node->next)
    {
        if (length_node->type != XML_ELEMENT_NODE)
            continue;
        ++length;
    }

    output.length = length;

    std::string chapter_name = GetRRChapterName(entry.data_url);

    if (chapter_name.empty())
    {
        std::cout << "Error: Unable to generate " << GetParserName(parser_type_) << " chapter name" << std::endl;
        xmlFreeDoc(chapter_doc_tree);
        return output;
    }

    std::string chapter_file_name = GetChapterFileName(entry.index_num + 1, chapter_name);

    FILE* chapter_file;
    std::string file_name = local_des_ + chapter_file_name;
    std::cout << "FILENAME: " << file_name << std::endl;
    chapter_file = fopen(file_name.c_str(), "w+");
    xmlElemDump(chapter_file, chapter_doc_tree, current_node);
    fclose(chapter_file);

    xmlFreeDoc(chapter_doc_tree);

    output.has_error = false;

    return output;
}

ParserXmlNodeSeek ParserRR::SeekToChapterContent(xmlNodePtr root_node)
{
    ParserXmlNodeSeek chapter_seek;
    xmlNodePtr current_node = NULL;
    bool found = false;

    for (current_node = root_node; current_node; current_node = current_node->next)
    {
        if (current_node->type != XML_ELEMENT_NODE)
            continue;

        if (NodeHasAttributeContent(current_node, "chapter-inner chapter-content"))
        {
            chapter_seek.seek_node = current_node;
            found = true;
            break;
        }

        ParserXmlNodeSeek children_seek = SeekToChapterContent(current_node->children);

        if (children_seek.seek_node != NULL)
            chapter_seek.seek_node = children_seek.seek_node;

        found = found || children_seek.found;
    }

    chapter_seek.found = found;

    return chapter_seek;
}

} // namespace RR
} // namespace parsers
} // namespace core
} // namespace black_library
