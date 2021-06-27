/**
 * ParserFFN.cc
 */

#include <ParserFFN.h>

namespace black_library {

namespace core {

namespace parsers {

namespace FFN {

ParserFFN::ParserFFN()
{
    parser_type_ = FFN_PARSER;
    source_url_ = FF::source_url;
}

ParserFFN::~ParserFFN()
{

}

void ParserFFN::Parse(const ParserJob &parser_job)
{
    std::string url_adult = parser_job.url + "?view_full_work=true&view_adult=true";
    std::string result = CurlRequest(url_adult);

    xmlDoc* doc = htmlReadDoc((xmlChar*) result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if(!doc)
    {
        std::cout << "Unable to Parse" << std::endl;
        return;
    }

    xmlNode* root_node = xmlDocGetRootElement(doc);
    xmlNode* next = root_node->children;

    while (memcmp(next->name, "body", 4))
    {
        next = next->next;
    }

    xmlNode* workskin = GetElementAttr(next, "id", "workskin");
    xmlDocSetRootElement(doc, workskin);

    std::string title_ = parser_job.url.substr(parser_job.url.find_last_of("/") + 1, parser_job.url.length() - 1);
    std::string des = local_des_ + title_ + ".html";

    FILE* file;
    file = fopen(des.c_str(), "w+");
    xmlDocFormatDump(file, doc, 1);
    fclose(file);
}

std::string ParserFFN::ParseTitle()
{
    return "";
}

std::string ParserFFN::ParseAuthor()
{
    return "";
}

int ParserFFN::ParseIndexEntry()
{
    return 0;
}

} // namespace FFN

} // namespace parsers
} // namespace core
} // namespace black_library
