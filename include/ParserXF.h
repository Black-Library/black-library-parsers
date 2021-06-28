/**
 * ParserXF.h
 *
 * XenForo forum parser
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_XF_PARSER_XF_H__
#define __BLACK_LIBRARY_CORE_PARSERS_XF_PARSER_XF_H__

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace XF {

class ParserXF : public Parser
{
public:
    explicit ParserXF(parser_t parser_type);
    ~ParserXF();

protected:
    ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    void FindIndexEntries(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    ParserIndexEntryInfo ParseIndexEntry(const ParserIndexEntry &index_entry);
    std::string PreprocessTargetUrl(const std::string &job_url);

    std::string GetTargetId(const std::string &data_url);
    std::string GetXFIndexEntryTitle(const ParserIndexEntry &index_entry);
    ParserXmlNodeSeek SeekToIndexEntryContent(xmlNodePtr root_node, const std::string &target_id);

};

} // namespace XF
} // namespace parsers
} // namespace core
} // namespace black_library

#endif
