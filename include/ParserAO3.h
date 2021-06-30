/**
 * ParserAO3.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_AO3_PARSER_AO3_H__
#define __BLACK_LIBRARY_CORE_PARSERS_AO3_PARSER_AO3_H__

#include "IndexEntryParser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace AO3 {

class ParserAO3 : public IndexEntryParser
{
public:
    explicit ParserAO3();
    ~ParserAO3();

protected:
    void FindIndexEntries(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    virtual ParserIndexEntryInfo ParseBehavior();
    std::string PreprocessTargetUrl(const std::string &job_url);

    ParserTimeResult GetPublishedTime(xmlNodePtr root_node);

private:
    
};

} // namespace AO3

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
