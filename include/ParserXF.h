/**
 * ParserXF.h
 *
 * XenForo forum parser
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_XF_PARSER_XF_H__
#define __BLACK_LIBRARY_CORE_PARSERS_XF_PARSER_XF_H__

#include "IndexEntryParser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace XF {

class ParserXF : public IndexEntryParser
{
public:
    explicit ParserXF(parser_t parser_type);

protected:
    ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    void FindIndexEntries(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    ParserBehaviorInfo ParseBehavior();
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
