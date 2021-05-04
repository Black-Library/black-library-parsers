/**
 * ParserRR.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_RR_PARSER_RR_H__
#define __BLACK_LIBRARY_CORE_PARSERS_RR_PARSER_RR_H__

#include <vector>

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace RR {

class ParserRR : public Parser
{
public:
    explicit ParserRR();

    ParserResult Parse();
    ParserResult Parse(size_t start_chapter);
    void Stop();

protected:
    std::string ParseTitle();
    std::string ParseAuthor();
    ParserChapterInfo ParseChapter(const ParserIndexEntry &entry);

    ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    void FindChapterNodes(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    std::string GetRRChapterName(const std::string &data_url);
    ParserXmlNodeSeek SeekToChapterContent(xmlNodePtr root_node);
    ParserXmlNodeSeek SeekToNodeByName(xmlNodePtr root_node, const std::string &name);
    
private:
    std::vector<ParserIndexEntry> index_entries_;
};

} // namespace RR

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
