/**
 * ParserSVF.cc
 */

#include <chrono>
#include <iostream>
#include <locale>
#include <sstream>
#include <thread>

#include <FileOperations.h>

#include <ParserSVF.h>

namespace black_library {

namespace core {

namespace parsers {

namespace SVF {

namespace BlackLibraryCommon = black_library::core::common;

ParserSVF::ParserSVF() :
    Parser(parser_t::SVF_PARSER)
{
    title_ = "SVF_Parser_title";
    source_name_ = BlackLibraryCommon::SVF::source_name;
    source_url_ = BlackLibraryCommon::SVF::source_url;
    author_ = "unknown-author";
}

ParserSVF::~ParserSVF()
{
    done_ = true;
}

std::string ParserSVF::GetSVFChapterName(const ParserIndexEntry &index_entry)
{
    std::locale loc;
    std::string svf_chapter_name = index_entry.chapter_name;

    std::transform(svf_chapter_name.begin(), svf_chapter_name.end(), svf_chapter_name.begin(),
    [&loc](char ch)
    {
        return ch == ' ' ? '-' : std::tolower(ch, loc);
    });

    return svf_chapter_name;
}

std::string ParserSVF::GetTargetId(const std::string &data_url)
{
    auto pos = data_url.find_last_of("-");

    return data_url.substr(pos + 1);
}

std::string ParserSVF::AppendTargetUrl(const std::string &job_url)
{
    return job_url + "threadmarks";
}

// TODO: Extract index entry may fail, use std::optional to check
ParserIndexEntry ParserSVF::ExtractIndexEntry(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;
    ParserIndexEntry index_entry;

    auto likes_result = GetXmlAttributeContentByName(root_node, "data-likes");
    auto author_result = GetXmlAttributeContentByName(root_node, "data-content-author");

    // for SVF this gets the most recent for ALL threadmarks
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
                break;
            }
        }
    }

    if (current_node == NULL)
    {
        return index_entry;
    }

    auto url_seek = SeekToNodeByNameRecursive(current_node->children, "a");

    if (!url_seek.found)
    {
        return index_entry;
    }

    current_node = url_seek.seek_node;

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

void ParserSVF::FindChapterNodes(xmlNodePtr root_node)
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

void ParserSVF::FindMetaData(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;

    for (current_node = root_node; current_node; current_node = current_node->next)
    {
        if (!xmlStrcmp(current_node->name, (const xmlChar *) "meta"))
        {
            auto property_result = GetXmlAttributeContentByName(current_node, "property");

            if (!property_result.found)
                continue;

            if (property_result.result == "og:url")
            {
                auto title_result = GetXmlAttributeContentByName(current_node, "content");

                if (!title_result.found)
                    continue;

                std::string unprocessed_title = title_result.result;
                size_t found_0 = unprocessed_title.find_last_of(".");
                std::string removed_threadmarks = unprocessed_title.substr(0, found_0);
                size_t found_1 = removed_threadmarks.find_last_of("/\\");
                title_ = removed_threadmarks.substr(found_1 + 1, removed_threadmarks.size());
            }
        }
    }

    xmlFree(current_node);
}

ParserChapterInfo ParserSVF::ParseChapter(const ParserIndexEntry &index_entry)
{
    ParserChapterInfo output;
    std::string chapter_url = "https://" + source_url_ + index_entry.data_url;
    std::cout << GetParserName(parser_type_) << " ParseChapter: " << chapter_url << " - " << index_entry.chapter_name << std::endl;

    std::string chapter_result = CurlRequest(chapter_url);
    xmlDocPtr chapter_doc_tree = htmlReadDoc((xmlChar*) chapter_result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (chapter_doc_tree == NULL)
    {
        std::cout << "Error: libxml HTMLparser unable to parse" << std::endl;
        return output;
    }

    std::string target_id = GetTargetId(index_entry.data_url);

    xmlNodePtr root_node = xmlDocGetRootElement(chapter_doc_tree);
    xmlNodePtr current_node = root_node->children;
    xmlNodePtr length_node = NULL;
    size_t length = 0;

    ParserXmlNodeSeek chapter_seek = SeekToChapterContent(current_node, target_id);
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

    std::string chapter_name = GetSVFChapterName(index_entry);

    chapter_name = BlackLibraryCommon::SanitizeFileName(chapter_name);

    if (chapter_name.empty())
    {
        std::cout << "Error: Unable to generate " << GetParserName(parser_type_) << " chapter name" << std::endl;
        xmlFreeDoc(chapter_doc_tree);
        return output;
    }

    std::string chapter_file_name = GetChapterFileName(index_entry, chapter_name);

    FILE* chapter_file;
    std::string file_name = local_des_ + chapter_file_name;
    std::cout << "FILENAME: " << file_name << std::endl;
    chapter_file = fopen(file_name.c_str(), "w+");

    if (chapter_file == NULL)
    {
        std::cout << "Error: could not open file with name: " << file_name << std::endl;
        xmlFreeDoc(chapter_doc_tree);
        return output;
    }

    xmlElemDump(chapter_file, chapter_doc_tree, current_node);
    // TODO: figure out how to handle seg faults/other errors in threadpool/thread
    fclose(chapter_file);

    xmlFreeDoc(chapter_doc_tree);

    output.has_error = false;

    return output;
}

ParserXmlNodeSeek ParserSVF::SeekToChapterContent(xmlNodePtr root_node, const std::string &target_id)
{
    ParserXmlNodeSeek chapter_seek;
    xmlNodePtr current_node = NULL;
    bool found = false;

    for (current_node = root_node; current_node; current_node = current_node->next)
    {
        if (current_node->type != XML_ELEMENT_NODE)
            continue;

        if (NodeHasAttribute(current_node, "data-lb-id"))
        {
            auto post_id = GetXmlAttributeContentByName(current_node, "data-lb-id");
            if (!post_id.found)
                continue;

            // find by post_id
            if (!post_id.result.compare("post-" + target_id))
            {
                auto inner_seek = SeekToNodeByNameRecursive(current_node->children, "div");
                if (!inner_seek.found)
                    continue;

                chapter_seek.seek_node = inner_seek.seek_node;
                found = true;
                break;
            }
        }

        ParserXmlNodeSeek children_seek = SeekToChapterContent(current_node->children, target_id);

        if (children_seek.seek_node != NULL)
            chapter_seek.seek_node = children_seek.seek_node;

        found = found || children_seek.found;
    }

    chapter_seek.found = found;

    return chapter_seek;
}

} // namespace SVF
} // namespace parsers
} // namespace core
} // namespace black_library