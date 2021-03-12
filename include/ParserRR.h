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
    // TODO add date added
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
    void FindChapterNodes(xmlNode *root_node);
    RR_index_entry ExtractIndexEntry(xmlNode *root_node);

private:
    std::vector<RR_index_entry> index_entries_;
};

} // namespace RR

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
