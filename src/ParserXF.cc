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
    xmlNodePtr current_node = NULL;

    ParserXmlNodeSeek head_seek = SeekToNodeByName(root_node, "head");
    if (!head_seek.found)
    {
        std::cout << "Warning: Could not find head from: " << uuid_ << std::endl;
        return;
    }

    current_node = head_seek.seek_node;

    const auto has_threadmark_seek = SeekToNodeByPattern(root_node, pattern_seek_t::XML_NAME, "article",
        pattern_seek_t::XML_ATTRIBUTE, "class=message message--post hasThreadmark  js-post js-inlineModContainer   ");
    if (!has_threadmark_seek.found)
    {
        std::cout << "Error: could not find hasThreadmark post" << std::endl;
        return;
    }

    current_node = has_threadmark_seek.seek_node;

    const auto message_inner_seek = SeekToNodeByPattern(current_node, pattern_seek_t::XML_NAME, "div",
        pattern_seek_t::XML_ATTRIBUTE, "class=message-inner");
    if (!message_inner_seek.found)
    {
        std::cout << "Error: could not find message-inner" << std::endl;
        return;
    }

    current_node = message_inner_seek.seek_node;

    const auto message_userdetails_seek = SeekToNodeByPattern(current_node, pattern_seek_t::XML_NAME, "div",
        pattern_seek_t::XML_ATTRIBUTE, "class=message-userDetails");
    if (!message_userdetails_seek.found)
    {
        std::cout << "Error: could not find message-userDetails" << std::endl;
        return;
    }

    current_node = message_userdetails_seek.seek_node->children;

    const auto name_seek = SeekToNodeByName(current_node, "h4");
    if (!name_seek.found)
    {
        std::cout << "Error: could not find author content" << std::endl;
        return;
    }

    current_node = name_seek.seek_node;

    const auto author_result = GetXmlNodeContent(current_node->children);
    if (!author_result.found)
    {
        std::cout << "Error: could not get author content" << std::endl;
        return;
    }

    author_ = author_result.result;
}

ParseSectionInfo ParserXF::ParseSection()
{
    ParseSectionInfo output;

    const auto working_url = next_url_;
    const auto working_index = index_;

    std::cout << GetParserName(parser_type_) << " ParseSection: " << GetParserBehaviorName(parser_behavior_) << " - parse url: " << working_url << std::endl;

    if (working_index > target_end_index_)
    {
        reached_end_ = true;
        output.has_error = false;
        return output;
    }

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

    // std::cout << GenerateXmlDocTreeString(current_node) << std::endl;

    const auto p_body_main_seek = SeekToNodeByPattern(current_node, pattern_seek_t::XML_NAME, "div",
        pattern_seek_t::XML_ATTRIBUTE, "class=p-body-main  ");
    if (!p_body_main_seek.found)
    {
        std::cout << "Error: failed content wrapper seek" << std::endl;
        xmlFreeDoc(section_doc_tree);
        return output;
    }

    current_node = p_body_main_seek.seek_node;

    const auto section_post_seek = SeekToSectionPost(current_node, target_post);
    if (!section_post_seek.found)
    {
        std::cout << "Error: failed post seek" << std::endl;
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

    // skip saving content if before target start index
    if (working_index < target_start_index_)
    {
        std::cout << "Target start index: " << target_start_index_ << " - current index: " << working_index << " Skipping filesave" << std::endl;
        output.has_error = false;
        return output;
    }

    // reset current node to threadmark header
    current_node = root_node->children;

    const auto section_content_seek = SeekToSectionContent(current_node, target_post);
    if (!section_content_seek.found)
    {
        std::cout << "Error: failed section content seek" << std::endl;
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
        std::cout << "Error: unable to generate " << GetParserName(parser_type_) << " section name" << std::endl;
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

std::string ParserXF::PreprocessTargetUrl(const ParserJob &parser_job)
{
    title_ = GetWorkTitleFromUrl(parser_job.url);

    if (parser_job.url == parser_job.last_url)
    {
        return parser_job.url;
    }

    return parser_job.url;
}

std::string ParserXF::GetFirstUrl(xmlNodePtr root_node, const std::string &data_url)
{
    xmlNodePtr current_node = NULL;

    const auto has_threadmark_seek = SeekToNodeByPattern(root_node, pattern_seek_t::XML_NAME, "article",
        pattern_seek_t::XML_ATTRIBUTE, "class=message message--post hasThreadmark  js-post js-inlineModContainer   ");
    if (!has_threadmark_seek.found)
    {
        std::cout << "Error: could not find hasThreadmark post" << std::endl;
        return "";
    }

    current_node = has_threadmark_seek.seek_node->children->next;

    const auto post_id_result = GetXmlAttributeContentByName(current_node, "id");
    if (!post_id_result.found)
    {
        std::cout << "Error: could not get post id" << std::endl;
        return "";
    }

    return data_url + '#' + post_id_result.result;
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
            next_url = href_result.result;

            break;
        }

        if (!found_next)
        {
            std::cout <<  GetParserName(parser_type_) << " GetNextUrl - reached end" << std::endl;
            reached_end_ = true;
            return next_url_;
        }
    }

    return "https://" + source_url_ + next_url;
}

std::string ParserXF::GetSectionTitle(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;

    const auto section_threadmark_header_seek = SeekToNodeByPattern(root_node, pattern_seek_t::XML_NAME, "div",
        pattern_seek_t::XML_ATTRIBUTE, "class=message-cell message-cell--threadmark-header");
    if (!section_threadmark_header_seek.found)
    {
        std::cout << "Error: failed threadmark header seek" << std::endl;
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
    xmlNodePtr current_node = NULL;

    // std::cout << GenerateXmlDocTreeString(root_node) << std::endl;

    const auto message_attribution_main_seek = SeekToNodeByPattern(root_node, pattern_seek_t::XML_NAME, "ul",
        pattern_seek_t::XML_ATTRIBUTE, "class=message-attribution-main listInline ");
    if (!message_attribution_main_seek.found)
    {
        std::cout << "Error: failed message attribution main seek" << std::endl;
        return 0;
    }

    current_node = message_attribution_main_seek.seek_node->children;

    const auto li_seek = SeekToNodeByName(current_node, "li");
    if (!li_seek.found)
    {
        std::cout << "Error: failed 'li' seek" << std::endl;
        return 0;
    }

    current_node = li_seek.seek_node->children;

    const auto a_seek = SeekToNodeByName(current_node, "a");
    if (!a_seek.found)
    {
        std::cout << "Error: failed 'a' seek" << std::endl;
        return 0;
    }

    current_node = a_seek.seek_node->children;

    const auto time_seek = SeekToNodeByName(current_node, "time");
    if (!time_seek.found)
    {
        std::cout << "Error: failed time seek" << std::endl;
        return 0;
    }

    current_node = time_seek.seek_node;

    const auto time_result = GetXmlAttributeContentByName(current_node, "data-time");

    if (!time_result.found)
    {
        std::cout << "Error: failed time result" << std::endl;
        return 0;
    }

    return std::stol(time_result.result);
}

std::string ParserXF::GetWorkTitleFromUrl(const std::string &data_url)
{
    std::string unprocessed_title = data_url;
    size_t found_0 = unprocessed_title.find_last_of(".");
    std::string removed_threadmarks = unprocessed_title.substr(0, found_0);
    size_t found_1 = removed_threadmarks.find_last_of("/\\");

    return removed_threadmarks.substr(found_1 + 1, removed_threadmarks.size());
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
