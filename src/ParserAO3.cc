#include <ParserAO3.h>

namespace AO3 {

ParserAO3::ParserAO3()
{

}

ParserAO3::~ParserAO3()
{

}

void ParserAO3::parse()
{
    std::string url_adult = url + "?view_full_work=true&view_adult=true";
    std::string result = curlRequest(url_adult);

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

    xmlNode* workskin = getElementAttr(next, "id", "workskin");
    xmlDocSetRootElement(doc, workskin);

    std::string title = url.substr(url.find_last_of("/") + 1, url.length() - 1);
    std::string des = local_des + title + ".html";

    FILE* file;
    file = fopen(des.c_str(), "w+");
    xmlDocFormatDump(file, doc, 1);
    fclose(file);
}

void ParserAO3::parseChapter()
{

}

} // namespace AO3