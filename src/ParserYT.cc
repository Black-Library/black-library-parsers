/**
 * ParserYT.cc
 */

#include <iostream>

#include <FileOperations.h>
#include <TimeOperations.h>

#include <ParserYT.h>

namespace black_library {

namespace core {

namespace parsers {

namespace YT {

namespace BlackLibraryCommon = black_library::core::common;

ParserYT::ParserYT() :
    Parser(parser_t::YT_PARSER)
{
    title_ = "YT_parser_title";
    source_name_ = BlackLibraryCommon::YT::source_name;
    source_url_ = BlackLibraryCommon::YT::source_url;
    author_ = "unknown-author";
}

ParserYT::~ParserYT()
{
    done_ = true;
}

std::string ParserYT::GetYTIndexEntryTitle(const ParserIndexEntry &index_entry)
{
    auto pos = index_entry.data_url.find_last_of("/");

    return index_entry.data_url.substr(pos + 1);
}

ParserIndexEntry ParserYT::ExtractIndexEntry(xmlNodePtr root_node)
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

void ParserYT::FindIndexEntries(xmlNodePtr root_node)
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

        FindIndexEntries(current_node->children);
    }

    xmlFree(current_node);
}

void ParserYT::FindMetaData(xmlNodePtr root_node)
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

ParserIndexEntryInfo ParserYT::ParseIndexEntry(const ParserIndexEntry &index_entry)
{
    (void) index_entry;

    ParserIndexEntryInfo output;
    
    output.has_error = false;

    return output;
}

ParserXmlNodeSeek ParserYT::SeekToIndexEntryContent(xmlNodePtr root_node)
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

} // namespace YT
} // namespace parsers
} // namespace core
} // namespace black_library
