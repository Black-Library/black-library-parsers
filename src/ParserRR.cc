/**
 * ParserRR.cc
 */

#include <chrono>
#include <sstream>
#include <thread>

#include <ParserRR.h>

namespace black_library {

namespace core {

namespace parsers {

namespace RR {

ParserRR::ParserRR() :
    index_entries_()
{
    title_ = "RR_Parser_title";
    nickname_ = "";
    source_url_ = RR::source_url;
    author_ = "unknown author";
    parser_type_ = RR_PARSER;
}

ParserResult ParserRR::Parse()
{
    return Parse(1);
}

ParserResult ParserRR::Parse(size_t start_chapter)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    ParserResult parser_result;
    done_ = false;

    std::cout << "Start ParserRR Parse" << std::endl;
    std::string curl_result = CurlRequest(url_);

    xmlDocPtr doc_tree = htmlReadDoc((xmlChar*) curl_result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (doc_tree == NULL)
    {
        std::cout << "Error: libxml HTMLparser unable to parse" << std::endl;
        parser_result.has_error = true;
        return parser_result;
    }

    // const xmlChar* encoding = doc_tree->encoding;

    xmlNodePtr root_node = xmlDocGetRootElement(doc_tree);
    xmlNodePtr current_node = root_node->children;

    ParserXmlNodeSeek head_seek = SeekToNodeByName(current_node, "head");

    if (!head_seek.found)
    {
        std::cout << "Could not find head, exiting" << std::endl;
        parser_result.has_error = true;
        xmlFreeDoc(doc_tree);
        return parser_result;
    }

    current_node = head_seek.seek_node;

    FindMetaData(current_node);

    std::cout << "\tTitle: " << title_ << std::endl;
    std::cout << "\tAuthor: " << author_ << std::endl;
    std::cout << "\tNickname: " << nickname_ << std::endl;

    // reset current node ptr to root node children
    current_node = root_node->children;

    ParserXmlNodeSeek body_seek = SeekToNodeByName(current_node, "body");

    if (!body_seek.found)
    {
        std::cout << "Could not find chapter index, exiting" << std::endl;
        parser_result.has_error = true;
        xmlFreeDoc(doc_tree);
        return parser_result;
    }

    current_node = body_seek.seek_node;

    std::cout << "ParserRR: Find chapter nodes" << std::endl;

    FindChapterNodes(current_node);

    xmlFreeDoc(doc_tree);

    std::cout << "ParserRR: Found " << index_entries_.size() << " nodes" << std::endl;

    size_t index = start_chapter - 1;

    if (index > index_entries_.size())
    {
        std::cout << "Error: ParserRR requested index greater than size" << std::endl;
        parser_result.has_error = true;
        xmlFreeDoc(doc_tree);
        return parser_result;
    }

    size_t seconds_counter = 0;
    size_t wait_time = 0;

    // TODO: make parser take 8ish hour break
    while (!done_)
    {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
    
        if (done_)
            break;

        if (seconds_counter >= wait_time)
            seconds_counter = 0;

        if (seconds_counter == 0)
        {
            RR_chapter_parse chapter_parse_info = ParseChapter(index_entries_[index]);
            wait_time = GenerateWaitTime(chapter_parse_info.RR_length);
            if (chapter_parse_info.has_error)
            {
                std::cout << "Error: ParserRR failed to parse chapter index: " << index << std::endl;
            }
            std::cout << "ParserRR: " << title_ << " chapter length is " << chapter_parse_info.RR_length 
                      << " - waiting " << wait_time << " seconds" << std::endl;
            ++index;
        }

        ++seconds_counter;

        std::this_thread::sleep_until(deadline);
    }

    xmlCleanupParser();

    return parser_result;
}

void ParserRR::Stop()
{
    done_ = true;
    std::cout << "ParserRR: stop" << std::endl;
}

std::string ParserRR::ParseTitle()
{
    return "";
}

std::string ParserRR::ParseAuthor()
{
    return "";
}

RR_chapter_parse ParserRR::ParseChapter(const RR_index_entry &entry)
{
    RR_chapter_parse output;
    std::string rr_url = "https://www.royalroad.com"+ entry.data_url;
    std::cout << "ParserRR ParseChapter: " << rr_url << std::endl;

    std::string chapter_result = CurlRequest(rr_url);
    xmlDocPtr chapter_doc_tree = htmlReadDoc((xmlChar*) chapter_result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (chapter_doc_tree == NULL)
    {
        std::cout << "Error: libxml HTMLparser unable to parse" << std::endl;
        output.has_error = true;
        return output;
    }

    xmlNodePtr root_node = xmlDocGetRootElement(chapter_doc_tree);
    xmlNodePtr current_node = root_node->children;
    xmlNodePtr length_node = NULL;
    size_t length = 0;

    ParserXmlNodeSeek chapter_seek = SeekToChapterContent(current_node);
    if (!chapter_seek.found)
    {
        std::cout << "Error: Failed seek" << std::endl;
        output.has_error = true;
        xmlFreeDoc(chapter_doc_tree);
        return output;
    }

    current_node = chapter_seek.seek_node;

    for (length_node = current_node->children; length_node; length_node = length_node->next)
    {
        if (length_node->type != XML_ELEMENT_NODE)
            continue;
        ++length;
    }

    output.RR_length = length;

    std::string chapter_name = GetRRChapterName(entry.data_url);

    if (chapter_name.empty())
    {
        std::cout << "Error: Unable to generate RR chapter name" << std::endl;
        output.has_error = true;
        xmlFreeDoc(chapter_doc_tree);
        return output;
    }

    std::string chapter_file_name = GetChapterFileName(entry.index_num + 1, chapter_name);

    FILE* chapter_file;
    chapter_file = fopen(chapter_file_name.c_str(), "w+");
    xmlElemDump(chapter_file, chapter_doc_tree, current_node);
    fclose(chapter_file);

    xmlFreeDoc(chapter_doc_tree);

    return output;
}

RR_index_entry ParserRR::ExtractIndexEntry(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;
    xmlNodePtr data_url_node = NULL;
    xmlNodePtr chapter_name_node = NULL;
    RR_index_entry index_entry;

    for (current_node = root_node->children; current_node; current_node = current_node->next)
    {
        if (current_node->type != XML_ELEMENT_NODE)
            continue;

        if (!xmlStrcmp(current_node->name, (const xmlChar *)"td"))
        {
            for (data_url_node = current_node->children; data_url_node; data_url_node = data_url_node->next)
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

    xmlFree(current_node);
    xmlFree(data_url_node);

    return index_entry;
}

void ParserRR::FindChapterNodes(xmlNodePtr root_node)
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
                        RR_index_entry index_entry = ExtractIndexEntry(index_node);
                        index_entry.index_num = index_num;
                        index_entries_.emplace_back(index_entry);
                        ++index_num;
                    }
                }

                xmlFree(index_node);
            }
        }

        FindChapterNodes(current_node->children);
    }

    xmlFree(current_node);
}

void ParserRR::FindMetaData(xmlNodePtr root_node)
{
    xmlNodePtr current_node = NULL;

    for (current_node = root_node; current_node; current_node = current_node->next)
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
                            ParserXmlAttributePayload attr_result = GetXmlAttributeContentByName(author_attr, "content");

                            if (attr_result.is_null || !attr_result.attribute_found)
                                continue;

                            author_ = attr_result.result;
                        }
                        else if (!xmlStrcmp(attr_content, (const xmlChar *) "og:url"))
                        {
                            xmlAttrPtr title_attr = attribute->next;
                            ParserXmlAttributePayload attr_result = GetXmlAttributeContentByName(title_attr, "content");

                            if (attr_result.is_null || !attr_result.attribute_found)
                                continue;

                            std::string unprocessed_title = attr_result.result;
                            size_t found = unprocessed_title.find_last_of("/\\");
                            title_ = unprocessed_title.substr(found + 1, unprocessed_title.size());
                        }
                    }

                    xmlFree(attr_content);
                }

                attribute = attribute->next;
            }

            xmlFree(attribute);
        }
    }

    xmlFree(current_node);
}

std::string ParserRR::GetRRChapterName(const std::string &data_url)
{
    auto pos = data_url.find_last_of("/");

    return data_url.substr(pos + 1);
}

ParserXmlNodeSeek ParserRR::SeekToChapterContent(xmlNodePtr root_node)
{
    ParserXmlNodeSeek chapter_seek;
    xmlNodePtr current_node = NULL;
    bool found = false;

    for (current_node = root_node; current_node; current_node = current_node->next)
    {
        if (current_node->type != XML_ELEMENT_NODE)
            continue;

        if (NodeHasAttributeContent(current_node, "chapter-inner chapter-content"))
        {
            chapter_seek.seek_node = current_node;
            found = true;
            break;
        }

        ParserXmlNodeSeek children_seek = SeekToChapterContent(current_node->children);

        if (children_seek.seek_node != NULL)
            chapter_seek.seek_node = children_seek.seek_node;

        found = found || children_seek.found;
    }

    chapter_seek.found = found;

    return chapter_seek;
}

ParserXmlNodeSeek ParserRR::SeekToNodeByName(xmlNodePtr root_node, const std::string &name)
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

} // namespace RR

} // namespace parsers
} // namespace core
} // namespace black_library
