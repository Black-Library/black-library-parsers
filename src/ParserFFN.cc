#include <ParserFFN.hh>

namespace librarycore {
namespace FFN {

ParserFFN::ParserFFN()
{

}

ParserFFN::~ParserFFN()
{

}

Result ParserFFN::Parse()
{
    std::string work = "";
    int chap_num = 1;
    std::string url_current = url + "/" + std::to_string(chap_num++);

    Result result = ParseChapter(url_current);
    while(result.GetCode() == 0) {
        work += result.GetMessage();
        std::cout << "CHAPTER: " << chap_num << std::endl;
        std::cout << result.GetMessage() << std::endl;
        url_current = url + "/" + std::to_string(chap_num++);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        result = ParseChapter(url_current);
    }

    xmlDoc* doc = htmlReadDoc((xmlChar*) work.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if(!doc)
    {
        return Result(1, "Unable to Parse");
    }

    std::string title = url.substr(url.find_last_of("/") + 1, url.length() - 1);
    std::string des = local_des + title + ".html";

    FILE* file;
    file = fopen(des.c_str(), "w+");
    xmlDocFormatDump(file, doc, 1);
    fclose(file);

    xmlFreeDoc(doc);

    return Result(0, "Success");
}

Result ParserFFN::ParseChapter(int chap_num) {
    return Result(1 + chap_num, "Not Avaliable");
}

Result ParserFFN::ParseChapter(std::string url)
{
    std::string result = CurlRequest(url);

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

    xmlNode* workskin = GetElementAttr(next, "aria-label", "story content");
    xmlDocSetRootElement(doc, workskin);

    xmlChar *xmlbuff;
    int buffersize;
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    std::string str((char*) xmlbuff);

    xmlFree(xmlbuff);
    xmlFreeDoc(doc);

    return Result(0, str);
}

Parser ParserFFN::Copy() {
  ParserFFN parser;
  parser.SetUrl(this->GetUrl());
  parser.SetLocalFilePath(this->GetLocalDes());
  return parser;
}

} // namespace FFN
} // namespace librarycore
