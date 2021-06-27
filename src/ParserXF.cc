/**
 * ParserXF.cc
 */

#include <chrono>
#include <iostream>
#include <locale>
#include <sstream>
#include <thread>

#include <FileOperations.h>

#include <ParserXF.h>

namespace black_library {

namespace core {

namespace parsers {

namespace XF {

namespace BlackLibraryCommon = black_library::core::common;

ParserXF::ParserXF(parser_t parser_type) :
    Parser(parser_type)
{
    title_ = "XF_parser_title";
    source_name_ = BlackLibraryCommon::ERROR::source_name;
    source_url_ = BlackLibraryCommon::ERROR::source_url;
    author_ = "unknown-author";
}

ParserXF::~ParserXF()
{
    done_ = true;
}

std::string ParserXF::GetXFIndexEntryTitle(const ParserIndexEntry &index_entry)
{
    std::locale loc;
    std::string xf_index_entry_name = index_entry.name;

    std::transform(xf_index_entry_name.begin(), xf_index_entry_name.end(), xf_index_entry_name.begin(),
    [&loc](char ch)
    {
        return ch == ' ' ? '-' : std::tolower(ch, loc);
    });

    return xf_index_entry_name;
}

std::string ParserXF::GetTargetId(const std::string &data_url)
{
    auto pos = data_url.find_last_of("-");

    return data_url.substr(pos + 1);
}

std::string ParserXF::AppendTargetUrl(const std::string &job_url)
{
    return job_url + "threadmarks";
}

// TODO: Extract index entry may fail, use std::optional to check
ParserIndexEntry ParserXF::ExtractIndexEntry(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;
    ParserIndexEntry index_entry;

    auto likes_result = GetXmlAttributeContentByName(root_node, "data-likes");
    auto author_result = GetXmlAttributeContentByName(root_node, "data-content-author");

    // for XF this gets the most recent for ALL threadmarks
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

    auto index_entry_name_result = GetXmlNodeContent(current_node);

    if (!index_entry_name_result.found)
    {
        return index_entry;
    }

    index_entry.name = index_entry_name_result.result;

    return index_entry;
}

void ParserXF::FindSectionNodes(xmlNodePtr root_node)
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

        FindSectionNodes(current_node->children);
    }

    xmlFree(current_node);
}

void ParserXF::FindMetaData(xmlNodePtr root_node)
{
    ParserXmlNodeSeek head_seek = SeekToNodeByName(root_node, "head");

    if (!head_seek.found)
    {
        std::cout << "Warning: Could not get metadata from: " << uuid_ << std::endl;
        return;
    }

    xmlNodePtr current_node = NULL;

    for (current_node = head_seek.seek_node->children; current_node; current_node = current_node->next)
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

ParserIndexEntryInfo ParserXF::ParseIndexEntry(const ParserIndexEntry &index_entry)
{
    ParserIndexEntryInfo output;
    std::string index_entry_url = "https://" + source_url_ + index_entry.data_url;
    std::cout << GetParserName(parser_type_) << " ParseIndexEntry: " << index_entry_url << " - " << index_entry.name << std::endl;

    std::string index_entry_curl_result = CurlRequest(index_entry_url);
    xmlDocPtr index_entry_doc_tree = htmlReadDoc((xmlChar*) index_entry_curl_result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (index_entry_doc_tree == NULL)
    {
        std::cout << "Error: libxml HTMLparser unable to parse" << std::endl;
        return output;
    }

    std::string target_id = GetTargetId(index_entry.data_url);

    xmlNodePtr root_node = xmlDocGetRootElement(index_entry_doc_tree);
    xmlNodePtr current_node = root_node->children;
    xmlNodePtr length_node = NULL;
    size_t length = 0;

    ParserXmlNodeSeek index_entry_content_seek = SeekToIndexEntryContent(current_node, target_id);
    if (!index_entry_content_seek.found)
    {
        std::cout << "Error: Failed seek" << std::endl;
        xmlFreeDoc(index_entry_doc_tree);
        return output;
    }

    current_node = index_entry_content_seek.seek_node;

    for (length_node = current_node->children; length_node; length_node = length_node->next)
    {
        if (length_node->type != XML_ELEMENT_NODE)
            continue;
        ++length;
    }

    output.length = length;

    std::string index_entry_name = GetXFIndexEntryTitle(index_entry);

    index_entry_name = BlackLibraryCommon::SanitizeFileName(index_entry_name);

    if (index_entry_name.empty())
    {
        std::cout << "Error: Unable to generate " << GetParserName(parser_type_) << " index entry name" << std::endl;
        xmlFreeDoc(index_entry_doc_tree);
        return output;
    }

    std::string index_entry_file_name = GetIndexEntryFileName(index_entry, index_entry_name);

    FILE* index_entry_file;
    std::string file_name = local_des_ + index_entry_file_name;
    std::cout << "FILENAME: " << file_name << std::endl;
    index_entry_file = fopen(file_name.c_str(), "w+");

    if (index_entry_file == NULL)
    {
        std::cout << "Error: could not open file with name: " << file_name << std::endl;
        xmlFreeDoc(index_entry_doc_tree);
        return output;
    }

    xmlElemDump(index_entry_file, index_entry_doc_tree, current_node);
    // TODO: figure out how to handle seg faults/other errors in threadpool/thread
    fclose(index_entry_file);

    xmlFreeDoc(index_entry_doc_tree);

    output.has_error = false;

    return output;
}

ParserXmlNodeSeek ParserXF::SeekToIndexEntryContent(xmlNodePtr root_node, const std::string &target_id)
{
    ParserXmlNodeSeek index_entry_content_seek;
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

                index_entry_content_seek.seek_node = inner_seek.seek_node;
                found = true;
                break;
            }
        }

        ParserXmlNodeSeek children_seek = SeekToIndexEntryContent(current_node->children, target_id);

        if (children_seek.seek_node != NULL)
            index_entry_content_seek.seek_node = children_seek.seek_node;

        found = found || children_seek.found;
    }

    index_entry_content_seek.found = found;

    return index_entry_content_seek;
}

} // namespace XF
} // namespace parsers
} // namespace core
} // namespace black_library
