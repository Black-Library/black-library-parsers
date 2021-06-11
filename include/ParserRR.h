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
    ~ParserRR();

protected:
    std::string GetRRChapterName(const ParserIndexEntry &index_entry);

    ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    void FindChapterNodes(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    ParserChapterInfo ParseChapter(const ParserIndexEntry &index_entry);
    ParserXmlNodeSeek SeekToChapterContent(xmlNodePtr root_node);

};

} // namespace RR
} // namespace parsers
} // namespace core
} // namespace black_library

#endif
