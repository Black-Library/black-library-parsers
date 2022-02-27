/**
 * ParserFFN.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_FFN_PARSER_FFN_H__
#define __BLACK_LIBRARY_CORE_PARSERS_FFN_PARSER_FFN_H__

#include "IndexEntryParser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace FFN {

class ParserFFN : public IndexEntryParser
{
public:
    explicit ParserFFN();

protected:
    ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    void FindIndexEntries(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    ParseSectionInfo ParseSection();
    std::string PreprocessTargetUrl(const ParserJob &parser_job);

    ParserTimeResult GetPublishedTime(xmlNodePtr root_node);

    std::string ParseTitle();
    std::string ParseAuthor();

private:
};

} // namespace FFN

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
