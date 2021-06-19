/**
 * ParserSVF.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_SVF_PARSER_SVF_H__
#define __BLACK_LIBRARY_CORE_PARSERS_SVF_PARSER_SVF_H__

#include <vector>

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace SVF {

class ParserSVF : public Parser
{
public:
    explicit ParserSVF();
    ~ParserSVF();

protected:
    std::string GetSVFChapterName(const ParserIndexEntry &index_entry);
    std::string GetTargetId(const std::string &data_url);

    std::string AppendTargetUrl(const std::string &job_url);
    ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    void FindChapterNodes(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    ParserChapterInfo ParseChapter(const ParserIndexEntry &index_entry);
    ParserXmlNodeSeek SeekToChapterContent(xmlNodePtr root_node, const std::string &target_id);

};

} // namespace SVF
} // namespace parsers
} // namespace core
} // namespace black_library

#endif
