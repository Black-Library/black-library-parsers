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

struct RR_index_entry {
    std::string data_url;
    std::string chapter_name;
    uint16_t index_num;
    // TODO add date added
};

struct RR_chapter_seek {
    xmlNodePtr seek_node = NULL;
    bool chapter_found;
};

class ParserRR : public Parser
{
public:
    ParserRR();

    void Parse();
    Parser Copy();

protected:
    std::string ParseTitle();
    std::string ParseAuthor();
    void ParseChapter();
    void FindChapterNodes(xmlNodePtr root_node);
    RR_index_entry ExtractIndexEntry(xmlNodePtr root_node);
    RR_chapter_seek SeekToChapterContent(xmlNodePtr root_node);
    bool NodeHasAttributeContent(xmlNodePtr root_node, const std::string &target_content);

private:
    std::vector<RR_index_entry> index_entries_;
};

} // namespace RR

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
