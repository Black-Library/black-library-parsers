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
    LinkedListParser(parser_type)
{
    title_ = "XF_parser_title";
    source_name_ = BlackLibraryCommon::ERROR::source_name;
    source_url_ = BlackLibraryCommon::ERROR::source_url;
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

ParseSectionInfo ParserXF::ParseSection()
{
    ParseSectionInfo output;

    std::cout << GetParserName(parser_type_) << " ParseSection: " << GetParserBehaviorName(parser_behavior_) << " - parse url: " << next_url_ << std::endl;

    const auto working_url = "https://" + source_url_ + next_url_;
    const auto working_index = index_;

    const auto section_curl_result = CurlRequest(working_url);
    xmlDocPtr section_doc_tree = htmlReadDoc((xmlChar*) section_curl_result.c_str(), NULL, NULL,
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

    const auto target_post = GetTargetPost(working_url);

    const auto content_wrapper_seek = SeekToNodeByPattern(current_node, pattern_seek_t::XML_NAME, "div",
        pattern_seek_t::XML_ATTRIBUTE, "class=xb-content-wrapper");
    if (!content_wrapper_seek.found)
    {
        std::cout << "Error: Failed content wrapper seek" << std::endl;
        xmlFreeDoc(section_doc_tree);
        return output;
    }

    current_node = content_wrapper_seek.seek_node;

    const auto section_post_seek = SeekToSectionPost(current_node, target_post);
    if (!section_post_seek.found)
    {
        std::cout << "Error: Failed post seek" << std::endl;
        xmlFreeDoc(section_doc_tree);
        return output;
    }

    current_node = section_post_seek.seek_node;

    // get title
    const auto section_title = GetSectionTitle(current_node->children);

    // get next url
    next_url_ = GetNextUrl(current_node->children);

    // get last update date
    last_update_date_ = GetUpdateDate(current_node->children);

    // reset current node to threadmark header
    current_node = root_node->children;

    const auto section_content_seek = SeekToSectionContent(current_node, target_post);
    if (!section_content_seek.found)
    {
        std::cout << "Error: Failed content seek" << std::endl;
        xmlFreeDoc(section_doc_tree);
        return output;
    }

    current_node = section_content_seek.seek_node;

    for (length_node = current_node->children; length_node; length_node = length_node->next)
    {
        if (length_node->type != XML_ELEMENT_NODE)
            continue;
        ++length;
    }

    output.length = length;

    auto section_title_name = GetXFTitle(section_title);

    section_title_name = BlackLibraryCommon::SanitizeFileName(section_title_name);

    if (section_title_name.empty())
    {
        std::cout << "Error: Unable to generate " << GetParserName(parser_type_) << " section name" << std::endl;
        xmlFreeDoc(section_doc_tree);
        return output;
    }

    auto section_file_name = GetSectionFileName(working_index, section_title_name);

    FILE* index_entry_file;
    const auto file_name = local_des_ + section_file_name;
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

std::string ParserXF::PreprocessTargetUrl(const std::string &job_url)
{
    return job_url + "threadmarks";
}

std::string ParserXF::GetFirstUrl(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;

    const auto body_seek = SeekToNodeByName(root_node, "body");

    if (!body_seek.found)
    {
        std::cout << "Could not find first url" << std::endl;
        return "";
    }

    current_node = body_seek.seek_node->children;

    ParserXmlNodeSeek item_container_seek = SeekToNodeByPattern(current_node, pattern_seek_t::XML_NAME, "div",
        pattern_seek_t::XML_ATTRIBUTE, "class=structItemContainer");
    if (item_container_seek.found)
    {
        xmlNodePtr index_node = NULL;
        for (index_node = item_container_seek.seek_node->children; index_node; index_node = index_node->next)
        {
            if (index_node->type == XML_ELEMENT_NODE)
            {
                auto author_result = GetXmlAttributeContentByName(index_node, "data-content-author");

                if (author_result.found)
                    author_ = author_result.result;

                current_node = index_node;

                break;
            }
        }
    }

    for (current_node = current_node->children; current_node; current_node = current_node->next)
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
        return "";
    }

    auto url_seek = SeekToNodeByNameRecursive(current_node->children, "a");

    if (!url_seek.found)
    {
        return "";
    }

    current_node = url_seek.seek_node;

    auto url_result = GetXmlAttributeContentByName(current_node, "href");

    if (!url_result.found)
    {
        return "";
    }

    return url_result.result;
}

std::string ParserXF::GetNextUrl(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;
    std::string next_url = "";

    const auto threadmark_nav_seek = SeekToNodeByPattern(root_node, pattern_seek_t::XML_NAME, "li",
        pattern_seek_t::XML_ATTRIBUTE, "class=threadmark-nav");
    if (threadmark_nav_seek.found)
    {
        bool found_next = false;

        for (current_node = threadmark_nav_seek.seek_node->children; current_node; current_node = current_node->next)
        {
            if (!NodeHasAttributeContent(current_node, "threadmark-control threadmark-control--next"))
                continue;

            const auto href_result = GetXmlAttributeContentByName(current_node, "href");

            if (!href_result.found)
                continue;

            found_next = true;
            std::cout << "has next:" << href_result.result << std::endl;
            next_url = href_result.result;

            break;
        }

        if (!found_next)
        {
            std::cout << "does not have next" << std::endl;
            reached_end_ = true;
        }
    }

    return next_url;
}

std::string ParserXF::GetSectionTitle(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;

    const auto section_threadmark_header_seek = SeekToNodeByPattern(root_node, pattern_seek_t::XML_NAME, "div",
        pattern_seek_t::XML_ATTRIBUTE, "class=message-cell message-cell--threadmark-header");
    if (!section_threadmark_header_seek.found)
    {
        std::cout << "Error: Failed threadmark header seek" << std::endl;
        return "";
    }

    current_node = section_threadmark_header_seek.seek_node->children;

    const auto span_seek = SeekToNodeByName(current_node, "span");

    if (!span_seek.found)
    {
        std::cout << "Error: could not find span node" << std::endl;
        return "";
    }

    current_node = span_seek.seek_node;

    const auto title_result = GetXmlNodeContent(current_node->children);

    if (!title_result.found)
    {
        std::cout << "Error: could not find title content" << std::endl;
        return "";
    }

    return title_result.result;
}

std::string ParserXF::GetTargetPost(const std::string &data_url)
{
    auto pos = data_url.find_last_of("-");
    const auto target_id = data_url.substr(pos + 1);

    return "post-" + target_id;
}

time_t ParserXF::GetUpdateDate(xmlNodePtr root_node)
{
    (void) root_node;
    // std::stol(date_result.result)
    return 0;
}

std::string ParserXF::GetXFTitle(const std::string &title)
{
    std::locale loc;
    std::string xf_title_name = title;

    std::transform(xf_title_name.begin(), xf_title_name.end(), xf_title_name.begin(),
    [&loc](char ch)
    {
        return ch == ' ' ? '-' : std::tolower(ch, loc);
    });

    return xf_title_name;
}

ParserXmlNodeSeek ParserXF::SeekToSectionContent(xmlNodePtr root_node, const std::string &target_post)
{
    ParserXmlNodeSeek section_content_seek;
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
            if (!post_id.result.compare(target_post))
            {
                auto inner_seek = SeekToNodeByNameRecursive(current_node->children, "div");
                if (!inner_seek.found)
                    continue;

                section_content_seek.seek_node = inner_seek.seek_node;
                found = true;
                break;
            }
        }

        ParserXmlNodeSeek children_seek = SeekToSectionContent(current_node->children, target_post);

        if (children_seek.seek_node != NULL)
            section_content_seek.seek_node = children_seek.seek_node;

        found = found || children_seek.found;
    }

    section_content_seek.found = found;

    return section_content_seek;
}

ParserXmlNodeSeek ParserXF::SeekToSectionPost(xmlNodePtr root_node, const std::string &target_post)
{
    ParserXmlNodeSeek section_post_seek;
    xmlNodePtr current_node = NULL;
    bool found = false;

    for (current_node = root_node; current_node; current_node = current_node->next)
    {
        if (current_node->type != XML_ELEMENT_NODE)
            continue;

        if (NodeHasAttribute(current_node, "data-content"))
        {
            if (NodeHasAttributeContent(current_node, target_post))
            {
                section_post_seek.seek_node = current_node;
                found = true;
                break;
            }
        }

        ParserXmlNodeSeek children_seek = SeekToSectionPost(current_node->children, target_post);

        if (children_seek.seek_node != NULL)
            section_post_seek.seek_node = children_seek.seek_node;

        found = found || children_seek.found;
    }

    section_post_seek.found = found;

    return section_post_seek;
}

} // namespace XF
} // namespace parsers
} // namespace core
} // namespace black_library
