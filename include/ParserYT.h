/**
 * ParseYTR.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_YT_PARSER_YT_H__
#define __BLACK_LIBRARY_CORE_PARSERS_YT_PARSER_YT_H__

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace YT {

class ParseYT : public Parser
{
public:
    explicit ParseYT();
    ~ParseYT();

protected:
    std::string GetYTIndexName(const ParserIndexEntry &index_entry);

    ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    void FindIndexNodes(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    ParserIndexEntryInfo ParseIndex(const ParserIndexEntry &index_entry);
    ParserXmlNodeSeek SeekToIndexContent(xmlNodePtr root_node);

};

} // namespace YT
} // namespace parsers
} // namespace core
} // namespace black_library

#endif
