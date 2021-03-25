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

struct RR_xml_node_seek {
    xmlNodePtr seek_node = NULL;
    bool found = false;
};

struct RR_index_entry {
    std::string data_url;
    std::string chapter_name;
    uint16_t index_num;
    // TODO add date added
};

struct RR_chapter_parse {
    size_t RR_length = 0;
    bool has_error = false;
};

class ParserRR : public Parser
{
public:
    explicit ParserRR();

    void Parse();
    void Parse(size_t start_chapter);
    void Stop();

protected:
    std::string ParseTitle();
    std::string ParseAuthor();
    RR_chapter_parse ParseChapter(const RR_index_entry &entry);

    RR_index_entry ExtractIndexEntry(xmlNodePtr root_node);
    void FindChapterNodes(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    std::string GetRRChapterName(const std::string &data_url);
    RR_xml_node_seek SeekToChapterContent(xmlNodePtr root_node);
    RR_xml_node_seek SeekToNodeByName(xmlNodePtr root_node, const std::string &name);
    

private:
    std::vector<RR_index_entry> index_entries_;
};

} // namespace RR

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
