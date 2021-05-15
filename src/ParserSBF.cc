/**
 * ParserSBF.cc
 */

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include <ParserSBF.h>

namespace black_library {

namespace core {

namespace parsers {

namespace SBF {

ParserSBF::ParserSBF() :
    Parser(SBF_PARSER)
{
    title_ = "SBF_Parser_title";
    nickname_ = "";
    source_url_ = black_library::core::common::SBF::source_url;
    author_ = "unknown author";
}

ParserSBF::~ParserSBF()
{
    done_ = true;
}

ParserResult ParserSBF::Parse(const ParserJob &parser_job)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    ParserResult parser_result;
    done_ = false;

    uuid_ = parser_job.uuid;

    std::cout << "Start " << GetParserName(parser_type_) << " Parse: " << parser_job.url << std::endl;
    std::string curl_result = CurlRequest(parser_job.url + "threadmarks");

    xmlDocPtr doc_tree = htmlReadDoc((xmlChar*) curl_result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (doc_tree == NULL)
    {
        std::cout << "Error: libxml HTMLparser unable to parse" << std::endl;
        parser_result.has_error = true;
        return parser_result;
    }

    // const xmlChar* encoding = doc_tree->encoding;

    xmlNodePtr root_node = xmlDocGetRootElement(doc_tree);
    xmlNodePtr current_node = root_node->children;

    ParserXmlNodeSeek body_seek = SeekToNodeByName(current_node, "body");

    if (!body_seek.found)
    {
        std::cout << "Could not find chapter index, exiting" << std::endl;
        parser_result.has_error = true;
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
    parser_result.metadata.source = black_library::core::common::SBF::name;

    xmlFreeDoc(doc_tree);

    std::cout << GetParserName(parser_type_) << ": Found " << index_entries_.size() << " nodes" << std::endl;

    size_t index = parser_job.start_chapter - 1;

    if (index > index_entries_.size())
    {
        std::cout << "Error: " <<  GetParserName(parser_type_) << " requested index greater than size" << std::endl;
        parser_result.has_error = true;
        xmlFreeDoc(doc_tree);
        return parser_result;
    }

    xmlCleanupParser();

    return parser_result;
}

void ParserSBF::FindChapterNodes(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;

    for (current_node = root_node; current_node; current_node = current_node->next)
    {
        if (current_node->type != XML_ELEMENT_NODE)
            continue;

        if (!xmlStrcmp(current_node->name, (const xmlChar *)"div"))
        {
            if (NodeHasAttributeContent(current_node, "structItemContainer"))
            {
                std::cout << GenerateXmlDocTreeString(current_node) << std::endl;

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

std::string ParserSBF::GetSBFChapterName(const std::string &data_url)
{
    (void) data_url;
    return "";
}

// TODO: Extract index entry may fail, use std::optional to check
ParserIndexEntry ParserSBF::ExtractIndexEntry(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;
    xmlNodePtr data_url_node = NULL;
    xmlNodePtr chapter_name_node = NULL;
    ParserIndexEntry index_entry;

    auto likes_result = GetXmlAttributeContentByName(root_node, "data-likes");
    auto author_result = GetXmlAttributeContentByName(root_node, "data-content-author");
    auto date_result = GetXmlAttributeContentByName(root_node, "data-content-date");

    if (likes_result.found)
        // TODO: track/store the likes/popularity
        // std::cout << likes_result.result << std::endl;

    if (author_result.found)
        author_ = author_result.result;

    if (date_result.found)
        index_entry.time_published = std::stol(date_result.result);

    for (current_node = root_node->children; current_node; current_node = current_node->next)
    {
        if (current_node->type != XML_ELEMENT_NODE)
            continue;

        if (!xmlStrcmp(current_node->name, (const xmlChar *)"div"))
        {
            if (NodeHasAttributeContent(current_node, "structItem-cell structItem-cell--main"))
            {
                data_url_node = current_node;
                break;
            }
        }
    }

    auto url_seek = SeekToNodeByNameRecursive(data_url_node->children, "a");

    if (!url_seek.found)
    {
        return index_entry;
    }

    data_url_node = url_seek.seek_node;

    auto url_result = GetXmlAttributeContentByName(data_url_node, "href");

    if (!url_result.found)
    {
        return index_entry;
    }
    std::cout << url_result.result << std::endl;

    chapter_name_node = data_url_node->children;

    std::cout << GenerateXmlDocTreeString(chapter_name_node) << std::endl;

    auto chapter_name_result = GetXmlAttributeContentByName(chapter_name_node, "Content");

    if (!chapter_name_result.found)
    {
        std::cout << "could not find attribute" << std::endl;
        return index_entry;
    }
    std::cout << chapter_name_result.result << std::endl;

    return index_entry;
}

void ParserSBF::FindMetaData(xmlNodePtr root_node)
{
    (void) root_node;
}

ParserChapterInfo ParserSBF::ParseChapter(const ParserIndexEntry &entry)
{
    ParserChapterInfo output;
    std::string chapter_url = "https://www." + source_url_ + entry.data_url;
    std::cout << GetParserName(parser_type_) << " ParseChapter: " << chapter_url << std::endl;

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
    // xmlNodePtr length_node = NULL;
    size_t length = 0;

    output.length = length;

    std::string chapter_name = GetSBFChapterName(entry.data_url);

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

    return output;}

ParserXmlNodeSeek ParserSBF::SeekToChapterContent(xmlNodePtr root_node)
{
    (void) root_node;
    ParserXmlNodeSeek node_seek;
    return node_seek;
}

} // namespace SBF
} // namespace parsers
} // namespace core
} // namespace black_library