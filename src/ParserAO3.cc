#include <ParserAO3.hh>

namespace librarycore {

namespace AO3 {

ParserAO3::ParserAO3()
{

}

ParserAO3::~ParserAO3()
{

}

Result ParserAO3::Parse()
{
    std::string url_adult = url + "?view_full_work=true&view_adult=true";
    std::string result = CurlRequest(url_adult);

    xmlDoc* doc = htmlReadDoc((xmlChar*) result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if(!doc)
    {
        return Result(1, "Unable to Parse");
    }

    xmlNode* root_node = xmlDocGetRootElement(doc);
    xmlNode* next = root_node->children;

    while (memcmp(next->name, "body", 4))
    {
        next = next->next;
    }

    xmlNode* workskin = GetElementAttr(next, "id", "workskin");
    xmlDocSetRootElement(doc, workskin);

    std::string title = url.substr(url.find_last_of("/") + 1, url.length() - 1);
    std::string des = local_des + title + ".html";

    FILE* file;
    file = fopen(des.c_str(), "w+");
    xmlDocFormatDump(file, doc, 1);
    fclose(file);

    xmlFreeDoc(doc);

    return Result(0, "Success");
}

Result ParserAO3::ParseChapter(int chap_num)
{
    return Result(1 + chap_num, "Not Avaliable");
}

Result ParserAO3::ParseChapter(std::string url)
{
    return Result(1, "Not Avaliable" + url);
}

Parser ParserAO3::Copy() {
  ParserAO3 parser;
  parser.SetUrl(this->GetUrl());
  parser.SetLocalFilePath(this->GetLocalDes());
  return parser;
}

} // namespace AO3

} // namespace librarycore
