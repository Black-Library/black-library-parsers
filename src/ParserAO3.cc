/**
 * ParserAO3.cc
 */

#include <iostream>

#include <ParserAO3.h>

namespace black_library {

namespace core {

namespace parsers {

namespace AO3 {

ParserAO3::ParserAO3()
{
    title_ = "AO3_Parser_title";
    nickname_ = "";
    source_url_ = black_library::core::common::AO3::source_url;
    author_ = "unknown author";
}

ParserAO3::~ParserAO3()
{

}

ParserResult ParserAO3::Parse(const ParserJob &parser_job)
{
    ParserResult parser_result;
    std::string url_adult = parser_job.url + "?view_full_work=true&view_adult=true";
    std::string result = CurlRequest(url_adult);

    xmlDocPtr doc = htmlReadDoc((xmlChar*) result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if(!doc)
    {
        std::cout << "Unable to Parse" << std::endl;
        parser_result.has_error = true;
        return parser_result;
    }

    xmlNodePtr root_node = xmlDocGetRootElement(doc);
    xmlNodePtr next = root_node->children;

    while (memcmp(next->name, "body", 4))
    {
        next = next->next;
    }

    xmlNodePtr workskin = black_library::core::parsers::GetXmlElementAttr(next, "id", "workskin");
    xmlDocSetRootElement(doc, workskin);

    std::string title_ = parser_job.url.substr(parser_job.url.find_last_of("/") + 1, parser_job.url.length() - 1);
    std::string des = local_des_ + title_ + ".html";

    FILE* file;
    file = fopen(des.c_str(), "w+");
    xmlDocFormatDump(file, doc, 1);
    fclose(file);

    return parser_result;
}

int ParserAO3::ParseChapter()
{
    return 0;
}

} // namespace AO3

} // namespace parsers
} // namespace core
} // namespace black_library
