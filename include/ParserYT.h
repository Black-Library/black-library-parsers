/**
 * ParserYTR.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_YT_PARSER_YT_H__
#define __BLACK_LIBRARY_CORE_PARSERS_YT_PARSER_YT_H__

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace YT {

class ParserYT : public Parser
{
public:
    explicit ParserYT();

protected:
    ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    void FindIndexEntries(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    ParseSectionInfo ParseSection();
    std::string PreprocessTargetUrl(const std::string &job_url);

    std::string GetYTIndexEntryTitle(const ParserIndexEntry &index_entry);
    ParserXmlNodeSeek SeekToIndexEntryContent(xmlNodePtr root_node);

    bool is_playlist;

};

} // namespace YT
} // namespace parsers
} // namespace core
} // namespace black_library

#endif
