/**
 * ParserYT.cc
 */

#include <iostream>

#include <FileOperations.h>
#include <TimeOperations.h>

#include <ParserYT.h>

namespace black_library {

namespace core {

namespace parsers {

namespace YT {

namespace BlackLibraryCommon = black_library::core::common;

ParserYT::ParserYT() :
    Parser(parser_t::YT_PARSER)
{
    title_ = "YT_parser_title";
    source_name_ = BlackLibraryCommon::YT::source_name;
    source_url_ = BlackLibraryCommon::YT::source_url;
    author_ = "unknown-author";

    is_playlist = false;
}

ParserYT::~ParserYT()
{
    done_ = true;
}

ParserIndexEntry ParserYT::ExtractIndexEntry(xmlNodePtr root_node)
{
    (void) root_node;
    ParserIndexEntry index_entry;

    return index_entry;
}

void ParserYT::FindIndexEntries(xmlNodePtr root_node)
{
    (void) root_node;
}

void ParserYT::FindMetaData(xmlNodePtr root_node)
{
    std::cout << GenerateXmlDocTreeString(root_node) << std::endl;
}

ParserIndexEntryInfo ParserYT::ParseIndexEntry(const ParserIndexEntry &index_entry)
{
    (void) index_entry;

    ParserIndexEntryInfo output;
    
    output.has_error = false;

    return output;
}

std::string ParserYT::PreprocessTargetUrl(const std::string &job_url)
{
    return job_url;
}

std::string ParserYT::GetYTIndexEntryTitle(const ParserIndexEntry &index_entry)
{
    auto pos = index_entry.data_url.find_last_of("/");

    return index_entry.data_url.substr(pos + 1);
}

ParserXmlNodeSeek ParserYT::SeekToIndexEntryContent(xmlNodePtr root_node)
{
    ParserXmlNodeSeek index_entry_content_seek;
    
    (void) root_node;

    return index_entry_content_seek;
}

} // namespace YT
} // namespace parsers
} // namespace core
} // namespace black_library
