/**
 * ParserCommon.cc
 */

#include <string.h>

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

// limited to 999 chapters right now (no CH000)
std::string GetChapterFileName(size_t index, const std::string &chapter_name)
{
    char buffer [CHAPTER_FILENAME_BUFFER_SIZE];
    int res = snprintf(buffer, CHAPTER_FILENAME_BUFFER_SIZE, "CH%03lu_%s.html", index, chapter_name.c_str());
    if (res < 0)
        return "";

    return std::string(buffer);
}

std::string GetParserName(parser_rep rep)
{
    switch (rep)
    {
    case ERROR_PARSER:
        return "ERROR_PARSER";
        break;
    case AO3_PARSER:
        return "AO3_PARSER";
        break;
    case FFN_PARSER:
        return "FFN_PARSER";
        break;
    case SBF_PARSER:
        return "SBF_PARSER";
        break;
    case RR_PARSER:
        return "RR_PARSER";
        break;
    case _NUM_PARSERS_TYPE:
        return "_NUM_PARSERS_TYPE";
        break;
    default:
        return "NO_PARSER";
        break;
    }
}

// c level string check is faster for smaller strings then
// needle.find(haystack) != std::string::npos
parser_rep GetParserTypeByUrl(const std::string &url)
{
    parser_rep rep = ERROR_PARSER;

    if (ContainsString(url, black_library::core::common::AO3::source_url))
    {
        rep = AO3_PARSER;
    }
    else if (ContainsString(url, black_library::core::common::FFN::source_url))
    {
        rep = FFN_PARSER;
    }
    else if (ContainsString(url, black_library::core::common::SBF::source_url))
    {
        rep = SBF_PARSER;
    }
    else if (ContainsString(url, black_library::core::common::RR::source_url))
    {
        rep = RR_PARSER;
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
                xmlChar *attr_content = xmlNodeListGetString(root_node->doc, attribute->children, 1);                attr_result.found = true;
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
    xmlFree(attribute);

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

} // namespace parsers
} // namespace core
} // namespace black_library
