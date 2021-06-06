/**
 * ParserCommon.cc
 */

#include <string.h>

#include <iomanip>
#include <iostream>
#include <cstdarg>
#include <sstream>

#include <ParserCommon.h>
#include <SourceInformation.h>

namespace black_library {

namespace core {

namespace parsers {

#define CHAPTER_FILENAME_BUFFER_SIZE 128

// C++ wrapper around strstr()
bool ContainsString(const std::string &haystack, const std::string &needle)
{
    // C-style compare is fastest
    if (strstr(haystack.c_str(), needle.c_str()) == NULL)
    {
        return false;
    }

    return true;
}

// based on http://www.xmlsoft.org/examples/tree1.c
std::string GenerateXmlDocTreeString(xmlNodePtr root_node)
{
    return GenerateXmlDocTreeStringHelper(root_node, 0);
}

std::string GenerateXmlDocTreeStringHelper(xmlNodePtr root_node, size_t depth)
{
    std::stringstream ss;
    xmlNodePtr cur_node = NULL;

    for (cur_node = root_node; cur_node; cur_node = cur_node->next)
    {
        std::string attribute_content_string = "";
        xmlAttrPtr attribute = cur_node->properties;
        while (attribute)
        {
            const xmlChar *attr_name = attribute->name;
            xmlChar *attr_content = xmlNodeListGetString(cur_node->doc, attribute->children, 1);
            if (attr_name !=NULL && attr_content != NULL)
            {
                attribute_content_string += std::string((char *) attr_name) + ": " + std::string((char *) attr_content) + " ";
            }
            xmlFree(attr_content);
            attribute = attribute->next;
        }
        xmlFree(attribute);

        if (cur_node->type == XML_ELEMENT_NODE)
        {
            ss << GetSpaceString(depth) << "node type: Element, name: " << cur_node->name <<
            ", Attributes: " << attribute_content_string << std::endl;
        }
        else if (cur_node->type == XML_ATTRIBUTE_NODE)
        {
            ss << GetSpaceString(depth) << "node type: Attribute, name: " << cur_node->name <<
            ", Attributes: " << attribute_content_string << std::endl;
        }
        else if (cur_node->type == XML_TEXT_NODE)
        {
            std::string content_string = "unknown content";
            size_t string_size = 0;
            xmlChar *content = xmlNodeGetContent(cur_node);
            if (content != NULL)
            {
                std::string unclean = std::string((char *)content);
                content_string = TrimWhitespace(unclean);
            }
            xmlFree(content);
            string_size = content_string.size();
            if (string_size > 0)
            {
                ss << GetSpaceString(depth) << "node type: Text, name: " << cur_node->name <<
                ", Attributes: " << attribute_content_string << ", Content: " << content_string << ", Size: " << string_size << std::endl;
            }
        }

        ss << GenerateXmlDocTreeStringHelper(cur_node->children, depth + 1);
    }

    return ss.str();
}

// limited to 9999 chapters right now (no CH10000), index starts at 1 for chapters
std::string GetChapterFileName(size_t index, const std::string &chapter_name)
{
    char buffer [CHAPTER_FILENAME_BUFFER_SIZE];
    int res = snprintf(buffer, CHAPTER_FILENAME_BUFFER_SIZE, "CH%04lu_%s.html", index, chapter_name.c_str());
    if (res < 0)
        return "";

    return std::string(buffer);
}

std::string GetParserName(parser_t rep)
{
    switch (rep)
    {
    case parser_t::ERROR_PARSER:
        return "ERROR_PARSER";
        break;
    case parser_t::AO3_PARSER:
        return "AO3_PARSER";
        break;
    case parser_t::FFN_PARSER:
        return "FFN_PARSER";
        break;
    case parser_t::SBF_PARSER:
        return "SBF_PARSER";
        break;
    case parser_t::RR_PARSER:
        return "RR_PARSER";
        break;
    case parser_t::_NUM_PARSERS_TYPE:
        return "_NUM_PARSERS_TYPE";
        break;
    default:
        return "NO_PARSER";
        break;
    }
}

// c level string check is faster for smaller strings then
// needle.find(haystack) != std::string::npos
parser_t GetParserTypeByUrl(const std::string &url)
{
    parser_t rep = parser_t::ERROR_PARSER;

    if (ContainsString(url, black_library::core::common::AO3::source_url))
    {
        rep = parser_t::AO3_PARSER;
    }
    else if (ContainsString(url, black_library::core::common::FFN::source_url))
    {
        rep = parser_t::FFN_PARSER;
    }
    else if (ContainsString(url, black_library::core::common::SBF::source_url))
    {
        rep = parser_t::SBF_PARSER;
    }
    else if (ContainsString(url, black_library::core::common::RR::source_url))
    {
        rep = parser_t::RR_PARSER;
    }

    return rep;
}

std::string GetSpaceString(size_t num_tabs)
{
    std::string tab_string = "";

    for (size_t i = 0; i < num_tabs; ++i)
    {
        tab_string += "  ";
    }

    return tab_string;
}

std::string GetStatusName(job_status_t job_status)
{
    switch (job_status)
    {
    case job_status_t::JOB_ERROR:
        return "job_error";
        break;
    case job_status_t::JOB_MANAGER_QUEUED:
        return "job_manager_queued";
        break;
    case job_status_t::JOB_WORKER_QUEUED:
        return "job_worker_queued";
        break;
    case job_status_t::JOB_WORKING:
        return "job_working";
        break;
    case job_status_t::JOB_FINISHED:
        return "job_finished";
        break;
    default:
        return "job_error";
        break;
    }
}

ParserXmlNodeSeek SeekToNodeByElementAttr(xmlNodePtr root, std::string attr, std::string value)
{
    ParserXmlNodeSeek result;
    if (!xmlStrcmp(root->name, (const xmlChar *) "text") || !xmlStrcmp(root->name, (const xmlChar *) "style") ||
        !xmlStrcmp(root->name, (const xmlChar *) "script"))
    {
        return result;
    }

    xmlAttr* prop = root->properties;
    while (prop)
    {
        if (memcmp(prop->name, attr.c_str(), attr.length()) == 0 &&
            memcmp(prop->children->content, value.c_str(), value.length()) == 0)
        {
            result.seek_node = root;
            result.found = true;
            return result;
        }

        prop = prop->next;
    }

    xmlNode* current_node = root->children;

    while (current_node)
    {
        ParserXmlNodeSeek node_result = SeekToNodeByElementAttr(current_node, attr, value);

        if (node_result.found)
        {
            return node_result;
        }

        current_node = current_node->next;
    }

    return result;
}

ParserXmlContentResult GetXmlNodeContent(xmlNodePtr root_node)
{
    ParserXmlContentResult content_result;

    xmlChar *content = xmlNodeGetContent(root_node);
    if (content == NULL)
    {
        xmlFree(content);
        return content_result;
    }

    std::string unclean = std::string((char *)content);
    content_result.result = TrimWhitespace(unclean);

    xmlFree(content);

    content_result.found = true;

    return content_result;
}

ParserXmlAttributeResult GetXmlAttributeContentByName(xmlNodePtr root_node, const std::string &target_name)
{
    ParserXmlAttributeResult attr_result;

    xmlAttrPtr attribute = root_node->properties;

    while (attribute)
    {
        const xmlChar *attr_name = attribute->name;
        if (attr_name != NULL)
        {
            if (!target_name.compare(std::string((char *)attr_name)))
            {
                xmlChar *attr_content = xmlNodeListGetString(root_node->doc, attribute->children, 1);
                if (attr_content != NULL)
                {
                    attr_result.result = std::string((char *) attr_content);
                    attr_result.found = true;
                }
                xmlFree(attr_content);
                break;
            }
        }

        attribute = attribute->next;
    }

    return attr_result;
}

bool NodeHasAttribute(xmlNodePtr root_node, const std::string &target_name)
{
    xmlAttrPtr attribute = root_node->properties;
    bool found = false;

    while (attribute)
    {
        const xmlChar *attr_name = attribute->name;
        if (attr_name != NULL)
        {
            if (!target_name.compare(std::string((char *)attr_name)))
            {
                found = true;
                break;
            }
        }

        attribute = attribute->next;
    }

    return found;
}

bool NodeHasAttributeContent(xmlNodePtr root_node, const std::string &target_content)
{
    xmlAttrPtr attribute = root_node->properties;
    bool found = false;

    while (attribute)
    {
        xmlChar *attr_content = xmlNodeListGetString(root_node->doc, attribute->children, 1);
        if (attr_content != NULL)
        {
            if (!target_content.compare(std::string((char *)attr_content)))
                found = true;
        }
        xmlFree(attr_content);

        attribute = attribute->next;
    }
    xmlFree(attribute);

    return found;
}

std::string SanitizeFileName(const std::string &file_name)
{
    std::string sanatized_file_name = file_name;
    std::string unallowed = " /\\*?<>:;=[]!@|";

    if (sanatized_file_name.front() == '-')
    {
        sanatized_file_name.erase(sanatized_file_name.begin());
    }

    std::transform(sanatized_file_name.begin(), sanatized_file_name.end(), sanatized_file_name.begin(),
    [&unallowed](char ch)
    {
        return (std::find(unallowed.begin(), unallowed.end(), ch) != unallowed.end()) ? '-' : ch;
    });


    return sanatized_file_name;
}

ParserXmlNodeSeek SeekToNodeByName(xmlNodePtr root_node, const std::string &name)
{
    ParserXmlNodeSeek seek;
    xmlNodePtr current_node = NULL;

    for (current_node = root_node; current_node; current_node = current_node->next)
    {
        if (current_node->type != XML_ELEMENT_NODE)
            continue;

        if (!xmlStrcmp(current_node->name, (const xmlChar *) name.c_str()))
        {
            seek.seek_node = current_node;
            seek.found = true;
            break;
        }
    }

    return seek;
}

ParserXmlNodeSeek SeekToNodeByNameRecursive(xmlNodePtr root_node, const std::string &name)
{
    ParserXmlNodeSeek seek;
    xmlNodePtr current_node = NULL;

    for (current_node = root_node; current_node; current_node = current_node->next)
    {
        if (current_node->type != XML_ELEMENT_NODE)
            continue;

        if (!xmlStrcmp(current_node->name, (const xmlChar *) name.c_str()))
        {
            seek.seek_node = current_node;
            seek.found = true;
            break;
        }

        ParserXmlNodeSeek children_seek = SeekToNodeByNameRecursive(current_node->children, name);

        if (children_seek.seek_node != NULL)
            seek.seek_node = children_seek.seek_node;

        seek.found = seek.found || children_seek.found;
    }

    return seek;
}

ParserXmlNodeSeek SeekToNodeByPattern(xmlNodePtr root_node, int num, ...)
{
    ParserXmlNodeSeek result;

    // SeekToNodeByPattern(node, 7, XML_NAME, "div", XML_ATTRIBUTE, "id", "main", XML_CONTENT, "stuff");
    va_list valist;
    va_start(valist, num);

    auto checkNode = [valist, root_node, num]()
    {
        int i;
        for (i = 0; i < num - 1; i += 2)
        {
            switch (va_arg(valist, pattern_seek_t))
            {
                case pattern_seek_t::XML_NAME:
                {

                    if (xmlStrcmp(root_node->name, va_arg(valist, const xmlChar *)))
                    {
                        return false;
                    }
                    break;
                }
                case pattern_seek_t::XML_ATTRIBUTE:
                {
                    if (i + 2 >= num)
                    {
                        std::cout << "INVALID ARGUMENTS" << std::endl;
                        break;
                    }
                    xmlAttr* prop = root_node->properties;
                    while (prop)
                    {
                        if (xmlStrcmp(prop->name, va_arg(valist, const xmlChar *)) ||
                            (xmlStrcmp(prop->children->content, va_arg(valist, const xmlChar *))))
                        {
                            if (!prop->next)
                            {
                                return false;
                            }
                        }

                        prop = prop->next;
                    }
                    ++i;
                    break;
                }
                case pattern_seek_t::XML_CONTENT:
                {
                    ParserXmlContentResult content_result = GetXmlNodeContent(root_node);
                    if(xmlStrcmp((const xmlChar *) content_result.result.c_str(), va_arg(valist, const xmlChar *)))
                    {
                        return false;
                    }
                    break;
                }
            }
        }

        return true;
    };

    if (checkNode())
    {
        result.seek_node = root_node;
        result.found = true;
        return result;
    }

    xmlNode* current_node = root_node->children;

    while (current_node)
    {
        ParserXmlNodeSeek node_result = SeekToNodeByPattern(current_node, num, valist);

        if (node_result.found)
        {
            return node_result;
        }

        current_node = current_node->next;
    }
    va_end(valist);
    return result;
}

std::string TrimWhitespace(const std::string& target_string)
{
    auto leading_pos = target_string.find_first_not_of(" \t\r\n\0");
    auto trailing_pos = target_string.find_last_not_of(" \t\r\n\0");
    if (leading_pos == std::string::npos && trailing_pos == std::string::npos)
    {
        return "";
    }

    return target_string.substr(leading_pos, trailing_pos - leading_pos + 1);
}

time_t parseTimet(std::string input, std::string format)
{
    struct tm tm_l {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    strptime(input.c_str(), format.c_str(), &tm_l);
    return mktime(&tm_l);
}

std::string getISOString(time_t time)
{
    std::stringstream str;
    str << std::put_time(std::gmtime(&time), "%FT%TZ") << std::ends;
    return str.str();
}

} // namespace parsers
} // namespace core
} // namespace black_library
