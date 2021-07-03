/**
 * IndexEntryParser.cc
 */

#include <functional>
#include <iostream>

#include <IndexEntryParser.h>
#include <ShortTimeGenerator.h>

namespace black_library {

namespace core {

namespace parsers {

namespace BlackLibraryCommon = black_library::core::common;

IndexEntryParser::IndexEntryParser(parser_t parser_type) : 
    Parser(parser_type),
    index_entries_()
{
    parser_behavior_ = parser_behavior_t::INDEX_ENTRY;
}

ParserIndexEntry IndexEntryParser::ExtractIndexEntry(xmlNodePtr root_node)
{
    (void) root_node;
    ParserIndexEntry index_entry;
    return index_entry;
}

int IndexEntryParser::CalculateIndexBounds(const ParserJob &parser_job)
{
    if (parser_job.start_number > index_entries_.size())
    {
        index_ = 0;
    }
    else
    {
        index_ = parser_job.start_number - 1;
    }

    if (parser_job.start_number <= parser_job.end_number && index_entries_.size() >= parser_job.end_number)
    {
        end_index_ = parser_job.end_number - 1;
    }
    else
    {
        end_index_ = index_entries_.size() - 1;
    }

    if (index_ > index_entries_.size() || index_entries_.empty())
    {
        std::cout << "Error: " <<  GetParserName(parser_type_) << " requested start index greater than detected entries" << std::endl;
        return -1;
    }

    return 0;
}

void IndexEntryParser::ExpendedAttempts()
{
    ++index_;
}

void IndexEntryParser::FindIndexEntries(xmlNodePtr root_node)
{
    (void) root_node;
}

int IndexEntryParser::PreParseLoop(xmlNodePtr root_node)
{
    std::cout << GetParserName(parser_type_) << ": Find index entry nodes" << std::endl;

    FindIndexEntries(root_node);

    std::cout << GetParserName(parser_type_) << ": Found " << index_entries_.size() << " nodes" << std::endl;

    if (index_entries_.size() <= 0)
    {
        std::cout << "Error: no index entries" << std::endl;
        return -1;
    }

    return 0;
}

bool IndexEntryParser::ReachedEnd()
{
    return index_ > end_index_;
}

void IndexEntryParser::SaveLastUrl(ParserResult &parser_result)
{
    parser_result.metadata.last_url = index_entries_[index_entries_.size() - 1].data_url;
}

void IndexEntryParser::SaveUpdateDate(ParserResult &parser_result)
{
    for (const auto & index_entry : index_entries_)
    {
        if (index_entry.time_published > parser_result.metadata.update_date)
            parser_result.metadata.update_date = index_entry.time_published;
    }
}

} // namespace parsers
} // namespace core
} // namespace black_library
