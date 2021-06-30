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

    const auto working_url = next_url_;
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

    // std::cout << GenerateXmlDocTreeString(current_node) << std::endl;

    const auto target_post = GetTargetPost(current_node);

    std::cout << "target_post: " << target_post << std::endl;

    const auto post_target = "id=" + target_post;

    const auto section_title_seek = SeekToNodeByPattern(current_node, pattern_seek_t::XML_NAME, "span",
        pattern_seek_t::XML_ATTRIBUTE, post_target);
    if (!section_title_seek.found)
    {
        std::cout << "Error: Failed title seek" << std::endl;
        xmlFreeDoc(section_doc_tree);
        return output;
    }

    std::cout << "Found" << std::endl;

    // reset current node
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

    std::string section_title_name = GetXFTitle(working_url);

    section_title_name = BlackLibraryCommon::SanitizeFileName(section_title_name);

    if (section_title_name.empty())
    {
        std::cout << "Error: Unable to generate " << GetParserName(parser_type_) << " index entry name" << std::endl;
        xmlFreeDoc(section_doc_tree);
        return output;
    }

    std::string section_file_name = GetSectionFileName(working_index, section_title_name);

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

std::string ParserXF::PreprocessTargetUrl(const std::string &job_url)
{
    return job_url + "threadmarks";
}

std::string ParserXF::GetTargetPost(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;

    const auto target_post_seek = SeekToNodeByPattern(root_node, pattern_seek_t::XML_NAME, "span",
        pattern_seek_t::XML_ATTRIBUTE, "class=u-anchorTarget",
        pattern_seek_t::XML_ATTRIBUTE, "id=post-18370764"
        );
    if (!target_post_seek.found)
    {
        std::cout << "Error: Failed target post seek" << std::endl;
        return "";
    }

    current_node = target_post_seek.seek_node;

    const auto post_id = GetXmlAttributeContentByName(current_node, "data-lb-id");

    const auto post_content = GetXmlAttributeContentByName(current_node, "data-content");

    if (!post_content.found)
    {
        std::cout << "Error: Failed content" << std::endl;
        return "";
    }

    std::cout << post_content.result << std::endl;

    return "hello";
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

} // namespace XF
} // namespace parsers
} // namespace core
} // namespace black_library
