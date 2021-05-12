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
    std::string curl_result = CurlRequest(parser_job.url + "threadmarks");

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

    ParserXmlNodeSeek body_seek = SeekToNodeByName(current_node, "body");

    if (!body_seek.found)
    {
        std::cout << "Could not find chapter index, exiting" << std::endl;
        parser_result.has_error = true;
        xmlFreeDoc(doc_tree);
        return parser_result;
    }

    current_node = body_seek.seek_node;

    std::cout << GenerateXmlDocTreeString(current_node) << std::endl;

    std::cout << GetParserName(parser_type_) << ": Find chapter nodes" << std::endl;

    FindChapterNodes(current_node->children);

    xmlFreeDoc(doc_tree);

    std::cout << GetParserName(parser_type_) << ": Found " << index_entries_.size() << " nodes" << std::endl;

    size_t index = parser_job.start_chapter - 1;

    if (index > index_entries_.size())
    {
        std::cout << "Error: " <<  GetParserName(parser_type_) << " requested index greater than size" << std::endl;
        parser_result.has_error = true;
        xmlFreeDoc(doc_tree);
        return parser_result;
    }

    xmlCleanupParser();

    return parser_result;
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