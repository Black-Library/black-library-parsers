/**
 * ParserSBF.cc
 */

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include <ParserSBF.h>

namespace black_library {

namespace core {

namespace parsers {

namespace SBF {

ParserSBF::ParserSBF() :
    Parser(SBF_PARSER)
{
    title_ = "SBF_Parser_title";
    nickname_ = "";
    source_url_ = black_library::core::common::SBF::source_url;
    author_ = "unknown author";
}

ParserSBF::~ParserSBF()
{
    done_ = true;
}

ParserResult ParserSBF::Parse(const ParserJob &parser_job)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    ParserResult parser_result;
    done_ = false;

    uuid_ = parser_job.uuid;

    std::cout << "Start " << GetParserName(parser_type_) << " Parse: " << parser_job.url << std::endl;
    std::string curl_result = CurlRequest(parser_job.url);

    xmlDocPtr doc_tree = htmlReadDoc((xmlChar*) curl_result.c_str(), NULL, NULL,
        HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (doc_tree == NULL)
    {
        std::cout << "Error: libxml HTMLparser unable to parse" << std::endl;
        parser_result.has_error = true;
        return parser_result;
    }

    // const xmlChar* encoding = doc_tree->encoding;

    xmlNodePtr root_node = xmlDocGetRootElement(doc_tree);
    xmlNodePtr current_node = root_node->children;

    std::cout << GenerateXmlDocTreeString(current_node) << std::endl;

    ParserResult result;
    return result;
}

void ParserSBF::FindChapterNodes(xmlNodePtr root_node)
{
    (void) root_node;
}

std::string ParserSBF::GetSBFChapterName(const std::string &data_url)
{
    (void) data_url;
    return "";
}

ParserIndexEntry ParserSBF::ExtractIndexEntry(xmlNodePtr root_node)
{
    (void) root_node;
    ParserIndexEntry entry;
    return entry;
}

void ParserSBF::FindMetaData(xmlNodePtr root_node)
{
    (void) root_node;
}

ParserChapterInfo ParserSBF::ParseChapter(const ParserIndexEntry &entry)
{
    (void) entry;
    ParserChapterInfo info;
    return info;
}

ParserXmlNodeSeek ParserSBF::SeekToChapterContent(xmlNodePtr root_node)
{
    (void) root_node;
    ParserXmlNodeSeek node_seek;
    return node_seek;
}

} // namespace SBF
} // namespace parsers
} // namespace core
} // namespace black_library