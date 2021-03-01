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
    std::cout << "foo" << std::endl;
    std::string url_adult = url_ + "?view_full_work=true&view_adult=true";
    std::string result = CurlRequest(url_adult);
    std::string author;
    std::string title;

    xmlDocPtr doc_tree = htmlReadDoc((xmlChar*) result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if(doc_tree == NULL)
    {
        std::cout << "Error: libxml HTMLparser unable to parse" << std::endl;
        return;
    }

    xmlNodePtr root_node = xmlDocGetRootElement(doc_tree);
    xmlNodePtr current_node = root_node->children;

    while (current_node != NULL)
    {
        if (!strcmp((char *)current_node->name, "name"))
        {
            author = std::string((char *)xmlNodeListGetString(doc_tree, current_node->xmlChildrenNode, 1));
        }
        // else if (!strcmp(current_node->name, "name") && (current_node->ns == ns))
        // {

        // }

        current_node = current_node->next;
    }

    std::cout << "\tAuthor: " << author << std::endl;

    // xmlNodePtr workskin = GetElementAttr(next, "id", "workskin");
    // xmlDocSetRootElement(doc_tree, workskin);

    // std::string des = local_des_ + "/" + title + ".html";

    // FILE* file;
    // file = fopen(des.c_str(), "w+");
    // xmlDocFormatDump(file, doc_tree, 1);

    xmlFreeDoc(doc_tree);

    // fclose(file);
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
