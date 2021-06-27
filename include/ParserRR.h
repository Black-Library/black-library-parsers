/**
 * ParserRR.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_RR_PARSER_RR_H__
#define __BLACK_LIBRARY_CORE_PARSERS_RR_PARSER_RR_H__

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
    std::string GetRRIndexEntryTitle(const ParserIndexEntry &index_entry);

    ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    void FindSectionNodes(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    ParserIndexEntryInfo ParseIndexEntry(const ParserIndexEntry &index_entry);
    ParserXmlNodeSeek SeekToIndexEntryContent(xmlNodePtr root_node);

};

} // namespace RR
} // namespace parsers
} // namespace core
} // namespace black_library

#endif
