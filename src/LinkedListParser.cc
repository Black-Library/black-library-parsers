/**
 * LinkedListParser.cc
 */

#include <iostream>
#include <limits>

#include <LinkedListParser.h>
#include <ShortTimeGenerator.h>

namespace black_library {

namespace core {

namespace parsers {

namespace BlackLibraryCommon = black_library::core::common;

LinkedListParser::LinkedListParser(parser_t parser_type) : 
    Parser(parser_type),
    next_url_(),
    last_update_date_(0),
    target_start_index_(0),
    target_end_index_(0),
    reached_end_(false)
{
    parser_behavior_ = parser_behavior_t::LINKED_LIST;
}

int LinkedListParser::CalculateIndexBounds(const ParserJob &parser_job)
{
    target_start_index_ = parser_job.start_number - 1;
    target_end_index_ = parser_job.end_number - 1;

    if (target_end_index_ == 0)
    {
        target_end_index_ = std::numeric_limits<size_t>::max();
    }

    return 0;
}

void LinkedListParser::ExpendedAttempts()
{
    reached_end_ = true;
}

int LinkedListParser::PreParseLoop(xmlNodePtr root_node, const ParserJob &parser_job)
{
    if (parser_job.url == parser_job.last_url)
        next_url_ = GetFirstUrl(root_node, parser_job.url);
    else
        next_url_ = parser_job.last_url;

    std::cout << next_url_ << std::endl;

    if (next_url_.empty())
    {
        std::cout << "Error: PreParseLoop empty first url" << std::endl;
        return -1;
    }

    return 0;
}

bool LinkedListParser::ReachedEnd()
{
    return reached_end_;
}

void LinkedListParser::SaveLastUrl(ParserResult &parser_result)
{
    parser_result.metadata.last_url = next_url_;
}

void LinkedListParser::SaveUpdateDate(ParserResult &parser_result)
{
    parser_result.metadata.update_date = last_update_date_;
}

} // namespace parsers
} // namespace core
} // namespace black_library
