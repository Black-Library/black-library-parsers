/**
 * ParserRR.cc
 */

#include <iostream>

#include <FileOperations.h>
#include <TimeOperations.h>

#include <ParserRR.h>

namespace black_library {

namespace core {

namespace parsers {

namespace RR {

namespace BlackLibraryCommon = black_library::core::common;

ParserRR::ParserRR() :
    IndexEntryParser(parser_t::RR_PARSER)
{
    title_ = "RR_parser_title";
    source_name_ = BlackLibraryCommon::RR::source_name;
    source_url_ = BlackLibraryCommon::RR::source_url;
}

std::string ParserRR::GetRRIndexEntryTitle(const ParserIndexEntry &index_entry)
{
    auto pos = index_entry.data_url.find_last_of("/");

    return index_entry.data_url.substr(pos + 1);
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

    auto index_entry_name_result = GetXmlNodeContent(current_node);

    if (!index_entry_name_result.found)
    {
        return index_entry;
    }

    index_entry.name = index_entry_name_result.result;

    auto time_seek = SeekToNodeByNameRecursive(root_node, "time");

    if (!time_seek.found)
    {
        return index_entry;
    }

    auto time_attr_seek = GetXmlAttributeContentByName(time_seek.seek_node, "title");

    if (!time_attr_seek.found)
    {
        return index_entry;
    }

    index_entry.time_published = BlackLibraryCommon::ParseTimet(time_attr_seek.result, "%A, %B %d, %Y %I:%M %p");

    return index_entry;
}

void ParserRR::FindIndexEntries(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;

    ParserXmlNodeSeek body_seek = SeekToNodeByName(root_node, "body");

    if (!body_seek.found)
    {
        std::cout << "Could not find index entries" << std::endl;
        return;
    }

    current_node = body_seek.seek_node->children;

    ParserXmlNodeSeek tbody_seek = SeekToNodeByPattern(current_node, pattern_seek_t::XML_NAME, "tbody");
    if (tbody_seek.found)
    {
        xmlNodePtr index_node = NULL;
        uint16_t index_num = 0;
        for (index_node = tbody_seek.seek_node->children; index_node; index_node = index_node->next)
        {
            if (!xmlStrcmp(index_node->name, (const xmlChar *) "tr"))
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

void ParserRR::FindMetaData(xmlNodePtr root_node)
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

ParseSectionInfo ParserRR::ParseSection()
{
    ParseSectionInfo output;
    const auto index_entry = index_entries_[index_];

    const auto index_entry_url = "https://www." + source_url_ + index_entry.data_url;
    std::cout << GetParserName(parser_type_) << " ParseSection: " << GetParserBehaviorName(parser_behavior_) << " - parse url: " << index_entry_url << " - " << index_entry.name << std::endl;

    const auto curl_request_result = network_.get()->NetworkRequest(index_entry_url);
    if(curl_request_result.has_error)
    {
        std::cout << curl_request_result.debug_string << std::endl;
        return output;
    }

    xmlDocPtr section_doc_tree = htmlReadDoc((xmlChar*) curl_request_result.html.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (section_doc_tree == NULL)
    {
        std::cout << "Error: libxml HTMLparser unable to parse" << std::endl;
        return output;
    }

    xmlNodePtr root_node = xmlDocGetRootElement(section_doc_tree);
    xmlNodePtr current_node = root_node->children;
    xmlNodePtr length_node = NULL;
    size_t length = 0;

    ParserXmlNodeSeek index_entry_content_seek = SeekToIndexEntryContent(current_node);
    if (!index_entry_content_seek.found)
    {
        std::cout << "Error: Failed content seek" << std::endl;
        xmlFreeDoc(section_doc_tree);
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

    std::string index_entry_title = GetRRIndexEntryTitle(index_entry);

    index_entry_title = BlackLibraryCommon::SanitizeFileName(index_entry_title);

    if (index_entry_title.empty())
    {
        std::cout << "Error: Unable to generate " << GetParserName(parser_type_) << " index entry title name" << std::endl;
        xmlFreeDoc(section_doc_tree);
        return output;
    }

    std::string section_file_name = GetSectionFileName(index_entry.index_num, index_entry_title);

    FILE* index_entry_file;
    std::string file_name = local_des_ + section_file_name;
    std::cout << "FILENAME: " << file_name << std::endl;
    index_entry_file = fopen(file_name.c_str(), "w+");

    if (index_entry_file == NULL)
    {
        std::cout << "Error: could not open file with name: " << file_name << std::endl;
        xmlFreeDoc(section_doc_tree);
        return output;
    }

    xmlElemDump(index_entry_file, section_doc_tree, current_node);

    fclose(index_entry_file);

    xmlFreeDoc(section_doc_tree);

    output.has_error = false;

    return output;
}

ParserXmlNodeSeek ParserRR::SeekToIndexEntryContent(xmlNodePtr root_node)
{
    ParserXmlNodeSeek index_entry_content_seek;
    xmlNodePtr current_node = NULL;
    bool found = false;

    for (current_node = root_node; current_node; current_node = current_node->next)
    {
        if (current_node->type != XML_ELEMENT_NODE)
            continue;

        if (NodeHasAttributeContent(current_node, "chapter-inner chapter-content"))
        {
            index_entry_content_seek.seek_node = current_node;
            found = true;
            break;
        }

        ParserXmlNodeSeek children_seek = SeekToIndexEntryContent(current_node->children);

        if (children_seek.seek_node != NULL)
            index_entry_content_seek.seek_node = children_seek.seek_node;

        found = found || children_seek.found;
    }

    index_entry_content_seek.found = found;

    return index_entry_content_seek;
}

} // namespace RR
} // namespace parsers
} // namespace core
} // namespace black_library
