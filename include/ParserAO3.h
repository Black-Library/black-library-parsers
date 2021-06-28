/**
 * ParserAO3.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_AO3_PARSER_AO3_H__
#define __BLACK_LIBRARY_CORE_PARSERS_AO3_PARSER_AO3_H__

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

namespace AO3 {

class ParserAO3 : public Parser
{
public:
    explicit ParserAO3();
    ~ParserAO3();

    ParserResult Parse(const ParserJob &parser_job);
    void Stop();

protected:
    int ParseIndexEntry();
    std::string AppendTargetUrl(const std::string &job_url);
    void FindIndexEntries(xmlNodePtr root_node);
    void FindMetaData(xmlNodePtr root_node);
    virtual ParserIndexEntryInfo ParseIndexEntry(const ParserIndexEntry &index_entry);
    ParserTimeResult GetPublishedTime(xmlNodePtr root_node);

private:
    
};

} // namespace AO3

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
