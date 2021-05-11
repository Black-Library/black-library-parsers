/**
 * ParserSBF.cc
 */

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

ParserResult ParserSBF::Parse(const ParserJob &parser_job)
{
    (void) parser_job;
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