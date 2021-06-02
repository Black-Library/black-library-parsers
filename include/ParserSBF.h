/**
 * ParserSBF.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_SBF_PARSER_SBF_H__
#define __BLACK_LIBRARY_CORE_PARSERS_SBF_PARSER_SBF_H__

#include <vector>

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace SBF {

class ParserSBF : public Parser
{
public:
    explicit ParserSBF();
    ~ParserSBF();

protected:
    std::string GetSBFChapterName(const std::string &chapter_name);
    std::string GetTargetId(const std::string &data_url);

    std::string AppendTargetUrl(const std::string &job_url);
    ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    void FindChapterNodes(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    ParserChapterInfo ParseChapter(const ParserIndexEntry &entry);
    ParserXmlNodeSeek SeekToChapterContent(xmlNodePtr root_node, const std::string &target_id);

};

} // namespace SBF
} // namespace parsers
} // namespace core
} // namespace black_library

#endif
