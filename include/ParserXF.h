/**
 * ParserXF.h
 *
 * XenForo forum parser
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_XF_PARSER_XF_H__
#define __BLACK_LIBRARY_CORE_PARSERS_XF_PARSER_XF_H__

#include "LinkedListParser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace XF {

class ParserXF : public LinkedListParser
{
public:
    explicit ParserXF(parser_t parser_type);

protected:
    virtual void FindMetaData(xmlNodePtr root_node);
    virtual ParseSectionInfo ParseSection();
    virtual std::string PreprocessTargetUrl(const std::string &job_url);

    std::string GetTargetPost(xmlNodePtr root_node);
    std::string GetXFTitle(const std::string &title);
    ParserXmlNodeSeek SeekToSectionContent(xmlNodePtr root_node, const std::string &target_id);

};

} // namespace XF
} // namespace parsers
} // namespace core
} // namespace black_library

#endif
