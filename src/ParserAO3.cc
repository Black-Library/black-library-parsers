/**
 * ParserAO3.cc
 */

#include <ParserAO3.h>

namespace black_library {

namespace core {

namespace parsers {

namespace AO3 {

ParserAO3::ParserAO3()
{
    parser_type_ = AO3_PARSER;
    source_url_ = AO3::source_url;
}

ParserAO3::~ParserAO3()
{

}

void ParserAO3::Parse()
{
    std::string url_adult = url_ + "?view_full_work=true&view_adult=true";
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

    std::string title_ = url_.substr(url_.find_last_of("/") + 1, url_.length() - 1);
    std::string des = local_des_ + title_ + ".html";

    FILE* file;
    file = fopen(des.c_str(), "w+");
    xmlDocFormatDump(file, doc, 1);
    fclose(file);
}

Parser ParserAO3::Copy()
{
    ParserAO3 parser;
    parser.SetParserType(this->GetParserType());
    parser.SetSourceUrl(this->GetSourceUrl());
    return parser;
}

std::string ParserAO3::ParseTitle()
{
    return "";
}

std::string ParserAO3::ParseAuthor()
{
    return "";
}

void ParserAO3::ParseChapter()
{

}

} // namespace AO3

} // namespace parsers
} // namespace core
} // namespace black_library
