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
    source_ = AO3_PARSER;
    source_url_ = source_url;
}

ParserAO3::~ParserAO3()
{

}

void ParserAO3::Parse()
{
    std::string url_adult = source_url_ + "?view_full_work=true&view_adult=true";
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

    std::string title_ = source_url_.substr(source_url_.find_last_of("/") + 1, source_url_.length() - 1);
    std::string des = local_des_ + title_ + ".html";

    FILE* file;
    file = fopen(des.c_str(), "w+");
    xmlDocFormatDump(file, doc, 1);
    fclose(file);
}

void ParserAO3::ParseChapter()
{

}

Parser ParserAO3::Copy() {
  ParserAO3 parser;
  parser.SetUrl(this->GetUrl());
  parser.SetLocalFilePath(this->GetLocalDes());
  return parser;
}

} // namespace AO3

} // namespace parsers
} // namespace core
} // namespace black_library
