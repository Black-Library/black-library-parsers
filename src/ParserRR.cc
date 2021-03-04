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

    // xmlOutputBufferPtr outputbuf;
    // outputbuf = (xmlOutputBufferPtr) xmlMalloc(sizeof(xmlOutputBuffer));
    // if (outputbuf == NULL)
    // {
    //     std::cout << "Error: failed to allocate buffer" << std::endl;
    //     xmlFree(outputbuf);
    //     return;
    // }
    // memset(outputbuf, 0, (sizeof(xmlOutputBuffer)));

    // xmlNodeDumpOutput(outputbuf, doc_tree, current_node, 1, 0, "UTF-8");
    // xmlFree(outputbuf);


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

        current_node = current_node->next;
    }

    std::cout << "\tAuthor: " << author << std::endl;
    std::cout << "\tTitle: " << title << std::endl;

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
