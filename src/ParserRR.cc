/**
 * ParserRR.cc
 */

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
    std::string title = "RR_Parser_base_name";
    bool start_found = false;

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

    while (current_node != NULL)
    {
        if (!xmlStrcmp(current_node->name, (const xmlChar *)"head"))
        {
            start_found = true;
            current_node = current_node->children;
            break;
        }

        current_node = current_node->next;
    }

    if (!start_found)
    {
        std::cout << "Could not find start, exiting" << std::endl;
        return;
    }

    int element_counter = 0;

    while (current_node != NULL)
    {
        if (!xmlStrcmp(current_node->name, (const xmlChar *)"mark"))
        {
            xmlChar *xmlCharHolder = xmlNodeListGetString(doc_tree, current_node->xmlChildrenNode, 1);
            author = std::string((char *)xmlCharHolder);
            xmlFree(xmlCharHolder);
        }
        else if (!xmlStrcmp(current_node->name, (const xmlChar *)"title"))
        {
            xmlChar *xmlCharHolder = xmlNodeListGetString(doc_tree, current_node->xmlChildrenNode, 1);
            title = std::string((char *)xmlCharHolder);
            xmlFree(xmlCharHolder);
        }
        else if (!xmlStrcmp(current_node->name, (const xmlChar *)"head"))
        {
            title = "head";
        }

        current_node = current_node->next;
        ++element_counter;
    }

    std::cout << "\tAuthor: " << author << std::endl;
    std::cout << "\tTitle: " << title << std::endl;
    std::cout << "\tElement_counter: " << element_counter << std::endl;

    // xmlNodePtr workskin = GetElementAttr(next, "id", "workskin");
    // xmlDocSetRootElement(doc_tree, workskin);

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

} // namespace RR

} // namespace parsers
} // namespace core
} // namespace black_library
