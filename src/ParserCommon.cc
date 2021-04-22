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
            ss << GetSpaceString(depth) << "node type: Text, name: " << cur_node->name << 
            ", Attributes: " << attribute_content_string << ", Content: " << content_string << ", Size: " << string_size << std::endl;
        }

        ss << GenerateXmlDocTreeStringHelper(cur_node->children, depth + 1);
    }

    return ss.str();
}

// limited to 999 chapters right now
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
    parser_rep rep = _NUM_PARSERS_TYPE;

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
        tab_string += "    ";
    }

    return tab_string;
}

ParserXmlAttributePayload GetXmlAttributeContentByName(xmlAttrPtr &attribute_ptr, const std::string &name)
{
    ParserXmlAttributePayload attr_result;

    if (attribute_ptr == NULL)
    {
        attr_result.is_null = true;
        return attr_result;
    }

    if (!xmlStrcmp(attribute_ptr->name, (const xmlChar *) name.c_str()))
    {
        attr_result.result = std::string((char *)attribute_ptr->children->content);
        attr_result.attribute_found = true;
    }

    return attr_result;
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
