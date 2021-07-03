/**
 * LinkedListParser.cc
 */

#include <iostream>

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
    reached_end_(false)
{
    parser_behavior_ = parser_behavior_t::LINKED_LIST;
}

void LinkedListParser::ExpendedAttempts()
{
    reached_end_ = true;
}

int LinkedListParser::PreParseLoop(xmlNodePtr root_node)
{
    next_url_ = GetFirstUrl(root_node);

    if (next_url_.empty())
    {
        std::cout << "Error: PreParseLoop empty first url";
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
