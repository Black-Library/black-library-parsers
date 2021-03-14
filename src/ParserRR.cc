/**
 * ParserRR.cc
 */

#include <sstream>

#include <ParserRR.h>

namespace black_library {

namespace core {

namespace parsers {

namespace RR {

ParserRR::ParserRR()
{
    parser_type_ = RR_PARSER;
    source_url_ = RR::source_url;
}

void ParserRR::Parse()
{
    std::cout << "Start ParserRR Parse" << std::endl;
    std::string url_adult = url_;
    std::string result = CurlRequest(url_adult);
    std::string author = "unknown author";
    std::string title = "RR_Parser_base_title";
    std::string nickname;
    bool head_found = false;
    bool body_found = false;

    xmlDocPtr doc_tree = htmlReadDoc((xmlChar*) result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if(doc_tree == NULL)
    {
        std::cout << "Error: libxml HTMLparser unable to parse" << std::endl;
        return;
    }

    xmlNodePtr root_node = xmlDocGetRootElement(doc_tree);
    xmlNodePtr current_node = root_node->children;

    std::string doc_string = GenerateXmlDocTreeString(current_node);
    std::cout << doc_string << std::endl;

    // TODO make this a separate function that finds top-level sections
    while (current_node != NULL)
    {
        if (!xmlStrcmp(current_node->name, (const xmlChar *)"head"))
        {
            head_found = true;
            current_node = current_node->children;
            break;
        }

        current_node = current_node->next;
    }

    if (!head_found)
    {
        std::cout << "Could not find head, exiting" << std::endl;
        return;
    }

    int element_counter = 0;

    while (current_node != NULL)
    {
        if (!xmlStrcmp(current_node->name, (const xmlChar *) "meta"))
        {
            xmlAttrPtr attribute = current_node->properties;
            while (attribute)
            {
                if (!xmlStrcmp(attribute->name, (const xmlChar *) "property"))
                {
                    xmlChar *attr_content = xmlNodeListGetString(current_node->doc, attribute->children, 1);
                    if (attr_content != NULL)
                    {
                        if (!xmlStrcmp(attr_content, (const xmlChar *) "books:author"))
                        {
                            xmlAttrPtr author_attr = attribute->next;
                            xmlAttributePayload attr_result = GetXmlAttributeContentByName(author_attr, "content");
                            if (attr_result.is_null)
                                continue;
                            author = attr_result.result;
                        }
                        else if (!xmlStrcmp(attr_content, (const xmlChar *) "og:url"))
                        {
                            xmlAttrPtr title_attr = attribute->next;
                            xmlAttributePayload attr_result = GetXmlAttributeContentByName(title_attr, "content");
                            if (attr_result.is_null)
                                continue;
                            std::string unprocessed_title = attr_result.result;
                            size_t found = unprocessed_title.find_last_of("/\\");
                            title = unprocessed_title.substr(found + 1, unprocessed_title.size());
                        }
                    }
                    xmlFree(attr_content);
                }
                attribute = attribute->next;
            }
            xmlFree(attribute);
        }

        current_node = current_node->next;
        ++element_counter;
    }
    xmlFree(current_node);

    std::cout << "\tTitle: " << title << std::endl;
    std::cout << "\tAuthor: " << author << std::endl;
    std::cout << "\tNickname: " << nickname << std::endl;
    std::cout << "\tElement_counter: " << element_counter << std::endl;

    // reset current node ptr to root node children
    current_node = root_node->children;

    while (current_node != NULL)
    {
        if (!xmlStrcmp(current_node->name, (const xmlChar *)"body"))
        {
            body_found = true;
            current_node = current_node->children;
            break;
        }
        current_node = current_node->next;
    }

    if (!body_found)
    {
        std::cout << "Could not find index, exiting" << std::endl;
        return;
    }

    std::cout << "find chapter nodes" << std::endl;

    FindChapterNodes(current_node);

    std::cout << "found chapter nodes" << std::endl;

    std::stringstream ss;
    for (auto const& item : index_entries_)
    {
        ss << "Index: " << item.index_num << " - URL: " << item.data_url << " - Name: " << item.chapter_name << std::endl;
    }
    std::cout << ss.str() << std::endl;

    std::string des = local_des_ + title + ".html";

    FILE* file;
    file = fopen(des.c_str(), "w+");
    xmlDocFormatDump(file, doc_tree, 1);

    xmlFreeDoc(doc_tree);
    xmlCleanupParser();

    fclose(file);
}

Parser ParserRR::Copy()
{
    std::cout << "ParserRR Copy" << std::endl;
    ParserRR parser;
    parser.SetSourceUrl(this->GetSourceUrl());
    return parser;
}

std::string ParserRR::ParseTitle()
{
    return "";
}

std::string ParserRR::ParseAuthor()
{
    return "";
}

void ParserRR::ParseChapter()
{

}

void ParserRR::FindChapterNodes(xmlNode *root_node)
{
    xmlNode *cur_node = NULL;
    for (cur_node = root_node; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"tbody"))
            {
                xmlNode *index_node = NULL;
                uint16_t index_num = 0;
                for (index_node = cur_node->children; index_node; index_node = index_node->next)
                {
                    if (index_node->type == XML_ELEMENT_NODE)
                    {
                        RR_index_entry index_entry = ExtractIndexEntry(index_node);
                        index_entry.index_num = index_num;
                        index_entries_.emplace_back(index_entry);
                        ++index_num;
                    }
                }
            }
        }
        else if (cur_node->type == XML_ATTRIBUTE_NODE)
        {
            
        }
        else if (cur_node->type == XML_TEXT_NODE)
        {
        }
        FindChapterNodes(cur_node->children);
    }
    xmlFree(cur_node);
}

RR_index_entry ParserRR::ExtractIndexEntry(xmlNode *root_node)
{
    xmlNode *cur_node = NULL;
    xmlNode *data_url_node = NULL;
    xmlNode *chapter_name_node = NULL;
    RR_index_entry index_entry;

    for (cur_node = root_node->children; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type != XML_ELEMENT_NODE)
            continue;

        if (!xmlStrcmp(cur_node->name, (const xmlChar *)"td"))
        {
            for (data_url_node = cur_node->children; data_url_node; data_url_node = data_url_node->next)
            {
                if (data_url_node->type != XML_ELEMENT_NODE)
                    continue;

                if (!xmlStrcmp(data_url_node->name, (const xmlChar *)"a"))
                {
                    // get data url
                    xmlAttrPtr attribute = data_url_node->properties;
                    while (attribute)
                    {
                        if (!xmlStrcmp(attribute->name, (const xmlChar *) "href"))
                        {
                            xmlChar *attr_content = xmlNodeListGetString(data_url_node->doc, attribute->children, 1);
                            if (attr_content != NULL)
                            {
                                index_entry.data_url = std::string((char *)attr_content);
                            }
                            xmlFree(attr_content);

                            // get chapter name
                            chapter_name_node = data_url_node->children;
                            if (chapter_name_node->type != XML_TEXT_NODE)
                                continue;

                            xmlChar *content = xmlNodeGetContent(chapter_name_node);
                            if (content != NULL)
                            {
                                std::string unclean = std::string((char *)content);
                                index_entry.chapter_name = TrimWhitespace(unclean);
                            }
                            xmlFree(content);
                        }
                        // TODO: get time published
                        // else if (!xmlStrcmp(attribute->name, (const xmlChar *) "class"))
                        // {
                        //     xmlChar *attr_content = xmlNodeListGetString(data_url_node->doc, attribute->children, 1);
                        //     if (attr_content != NULL)
                        //     {
                        //         index_entry.date_published = std::string((char *)attr_content);
                        //     }
                        //     xmlFree(attr_content);
                        // }
                        attribute = attribute->next;
                    }
                    xmlFree(attribute);
                }
            }
        }
    }
    xmlFree(cur_node);
    xmlFree(data_url_node);

    return index_entry;
}

} // namespace RR

} // namespace parsers
} // namespace core
} // namespace black_library
