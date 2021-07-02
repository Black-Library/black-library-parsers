/**
 * IndexEntryParser.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_INDEX_ENTRY_PARSER_H__
#define __BLACK_LIBRARY_CORE_PARSERS_INDEX_ENTRY_PARSER_H__

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

class IndexEntryParser : public Parser
{
public:
    explicit IndexEntryParser(parser_t parser_type);

protected:
    int CalculateIndexBounds(const ParserJob &parser_job);
    int PreParseLoop(xmlNodePtr root_node) override;
    bool ReachedEnd();
    void SaveLastUrl(ParserResult &parser_result);
    void SaveUpdateDate(ParserResult &parser_result);

    virtual ParserIndexEntry ExtractIndexEntry(xmlNodePtr root_node);
    virtual void FindIndexEntries(xmlNodePtr root_node);

    std::vector<ParserIndexEntry> index_entries_;
};

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
