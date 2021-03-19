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
    std::string curl_result = CurlRequest(url_adult);
    std::string author = "unknown author";
    std::string title = "RR_Parser_base_title";
    std::string nickname;
    bool head_found = false;
    bool body_found = false;

    xmlDocPtr doc_tree = htmlReadDoc((xmlChar*) curl_result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (doc_tree == NULL)
    {
        std::cout << "Error: libxml HTMLparser unable to parse" << std::endl;
        return;
    }

    // const xmlChar* encoding = doc_tree->encoding;

    xmlNodePtr root_node = xmlDocGetRootElement(doc_tree);
    xmlNodePtr current_node = root_node->children;

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
                            if (attr_result.is_null || !attr_result.attribute_found)
                                continue;
                            author = attr_result.result;
                        }
                        else if (!xmlStrcmp(attr_content, (const xmlChar *) "og:url"))
                        {
                            xmlAttrPtr title_attr = attribute->next;
                            xmlAttributePayload attr_result = GetXmlAttributeContentByName(title_attr, "content");
                            if (attr_result.is_null || !attr_result.attribute_found)
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

    std::stringstream ss;
    for (auto const& item : index_entries_)
    {
        ss << "Index: " << item.index_num << " - URL: " << item.data_url << " - Name: " << item.chapter_name << std::endl;
    }
    std::cout << ss.str() << std::endl;

    // for (auto const& entry : index_entries_)
    // {
        std::string rr_url = "https://www.royalroad.com" + index_entries_[0].data_url;
        std::cout << rr_url << std::endl;
        std::string chapter_result = CurlRequest(rr_url);
        xmlDocPtr chapter_doc_tree = htmlReadDoc((xmlChar*) chapter_result.c_str(), NULL, NULL,
            HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
        if (chapter_doc_tree == NULL)
        {
            std::cout << "Error: libxml HTMLparser unable to parse" << std::endl;
            return;
        }
        root_node = xmlDocGetRootElement(chapter_doc_tree);
        current_node = root_node->children;

        // TODO: make chapter_seek exit loop if failure
        // make length of chapter effect sleep duration
        RR_chapter_seek chapter_seek = SeekToChapterContent(current_node);
        if (!chapter_seek.chapter_found)
        {
            std::cout << "Failed seek" << std::endl;
        }
        else
        {
            current_node = chapter_seek.seek_node;
            std::cout << "Succeeded seek" << std::endl;
        }
        // std::string doc_string = GenerateXmlDocTreeString(current_node);
        // std::cout << doc_string << std::endl;

        FILE* chapter_file;
        chapter_file = fopen("CH1.html", "w+");
        xmlElemDump(chapter_file, chapter_doc_tree, current_node);
        fclose(chapter_file);
    // }

    std::string des = local_des_ + title + ".html";

    FILE* file;
    file = fopen(des.c_str(), "w+");


    xmlDocFormatDump(file, doc_tree, 1);

    xmlFreeDoc(doc_tree);
    xmlFreeDoc(chapter_doc_tree);
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

void ParserRR::FindChapterNodes(xmlNodePtr root_node)
{
    xmlNodePtr cur_node = NULL;
    for (cur_node = root_node; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"tbody"))
            {
                xmlNodePtr index_node = NULL;
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
                xmlFree(index_node);
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

RR_index_entry ParserRR::ExtractIndexEntry(xmlNodePtr root_node)
{
    xmlNodePtr cur_node = NULL;
    xmlNodePtr data_url_node = NULL;
    xmlNodePtr chapter_name_node = NULL;
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

RR_chapter_seek ParserRR::SeekToChapterContent(xmlNodePtr root_node)
{
    RR_chapter_seek chapter_seek;
    xmlNodePtr cur_node = NULL;
    bool found = false;

    for (cur_node = root_node; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type != XML_ELEMENT_NODE)
            continue;
        if (NodeHasAttributeContent(cur_node, "chapter-inner chapter-content"))
        {
            chapter_seek.seek_node = cur_node;
            found = true;
            break;
        }

        RR_chapter_seek children_seek = SeekToChapterContent(cur_node->children);

        if (children_seek.seek_node != NULL)
            chapter_seek.seek_node = children_seek.seek_node;
        found = found || children_seek.chapter_found;
    }
    chapter_seek.chapter_found = found;

    return chapter_seek;
}

bool ParserRR::NodeHasAttributeContent(xmlNodePtr root_node, const std::string &target_content)
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

} // namespace RR

} // namespace parsers
} // namespace core
} // namespace black_library
