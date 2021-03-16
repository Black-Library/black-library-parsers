/**
 * Parser.cc
 */

#include <sstream>

#include <Parser.h>

namespace black_library {

namespace core {

namespace parsers {

Parser::Parser(parser_rep parser_type)
{
    parser_type_ = parser_type;
}

void Parser::Parse()
{

}

std::string Parser::CurlRequest(const std::string &url)
{
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();

    if (!curl)
    {
        std::cout << "Error: curl did not initialize" << std::endl;
        return "";
    }

    std::string html_raw;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HandleCurlResponse);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html_raw);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cout << "Curl Request Failed: " << curl_easy_strerror(res) << std::endl;
        return "";
    }

    curl_easy_cleanup(curl);

    return html_raw;
}

xmlNode* Parser::GetElementAttr(xmlNode* root, std::string attr, std::string value) {
    if (memcmp(root->name, "text", 4) == 0 || memcmp(root->name, "style", 5) == 0 ||
        memcmp(root->name, "script", 6) == 0)
    {
        return NULL;
    }

    xmlAttr* prop = root->properties;
    while (prop)
    {
        if (memcmp(prop->name, attr.c_str(), attr.length()) == 0 &&
            memcmp(prop->children->content, value.c_str(), value.length()) == 0)
        {
            return root;
        }

        prop = prop->next;
    }

    xmlNode* current_node = root->children;

    while (current_node)
    {
        xmlNode* node = GetElementAttr(current_node, attr, value);

        if (node)
        {
            return node;
        }

        current_node = current_node->next;
    }

    return NULL;
}

void Parser::SetParserType(parser_rep parser_type)
{
    this->parser_type_ = parser_type;
}

void Parser::SetSourceUrl(const std::string &url)
{
    this->source_url_ = url;
}

void Parser::SetUrl(const std::string &url)
{
    this->url_ = url;
}

void Parser::SetLocalFilePath(const std::string &local_des)
{
    this->local_des_ = local_des;
}

Parser Parser::Copy()
{
    Parser parser;
    parser.SetSourceUrl(this->GetSourceUrl());
    return parser;
}

std::string Parser::GetLocalDes()
{
    return local_des_;
}

parser_rep Parser::GetParserType()
{
    return parser_type_;
}

std::string Parser::GetSourceUrl()
{
    return source_url_;
}

std::string Parser::GetTitle()
{
    return title_;
}

std::string Parser::GetUrl()
{
    return url_;
}

std::string Parser::ParseTitle()
{
    return "no title";
}

std::string Parser::ParseAuthor()
{
    return "no author";
}

// based on http://www.xmlsoft.org/examples/tree1.c
std::string Parser::GenerateXmlDocTreeString(xmlNode *root_node)
{
    return GenerateXmlDocTreeStringHelper(root_node, 0);
}

std::string Parser::GenerateXmlDocTreeStringHelper(xmlNode *root_node, size_t depth)
{
    std::stringstream ss;
    xmlNode *cur_node = NULL;

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

std::string Parser::GetSpaceString(size_t num_tabs)
{
    std::string tab_string = "";

    for (size_t i = 0; i < num_tabs; ++i)
    {
        tab_string += "    ";
    }

    return tab_string;
}

xmlAttributePayload Parser::GetXmlAttributeContentByName(xmlAttrPtr &attribute_ptr, const std::string &name)
{
    xmlAttributePayload attr_result;

    if (attribute_ptr == NULL)
    {
        attr_result.is_null = true;
        return attr_result;
    }

    if (!xmlStrcmp(attribute_ptr->name, (const xmlChar *) name.c_str()))
    {
        attr_result.result = std::string((char *)attribute_ptr->children->content);
    }

    return attr_result;
}

std::string Parser::TrimWhitespace(const std::string& target_string)
{
    auto leading_pos = target_string.find_first_not_of(" \t\r\n\0");
    auto trailing_pos = target_string.find_last_not_of(" \t\r\n\0");
    if (leading_pos == std::string::npos && trailing_pos == std::string::npos)
    {
        return "";
    }
    std::cout << "size: " << target_string.size() << " - leading_pos: " << leading_pos << " - trailing_pos: " << trailing_pos << std::endl;
    return target_string.substr(leading_pos, trailing_pos - leading_pos + 1);
}

// Credit: https://stackoverflow.com/questions/5525613/how-do-i-fetch-a-html-page-source-with-libcurl-in-c
size_t HandleCurlResponse(void* ptr, size_t size, size_t nmemb, void* data)
{
    std::string* str = (std::string*) data;
    char* sptr = (char*) ptr;

    for(size_t x = 0; x < size * nmemb; x++)
    {
      (*str) += sptr[x];
    }

    return size * nmemb;
}

} // namespace parsers
} // namespace core
} // namespace black_library
