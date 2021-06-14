#include <catch2/catch_test_macros.hpp>

#include <Parser.h>
#include <ParserRR.h>

namespace black_library {

namespace core {

namespace parsers {

static constexpr const char RR_DUMMY_URL[] = "https://www.royalroad.com/fiction/00000/some-fiction";

namespace BlackLibraryCommon = black_library::core::common;

TEST_CASE( "Generic parser tests (pass)", "[single-file]" )
{
    Parser parser;
    parser.SetLocalFilePath("foo");
}

TEST_CASE( "RR parser tests (pass)", "[single-file]" )
{
    RR::ParserRR parser;
    parser.SetLocalFilePath("foo");
    REQUIRE(parser.GetParserType() == parser_t::RR_PARSER);
    REQUIRE(parser.GetSourceUrl() == BlackLibraryCommon::RR::source_url);
}

} // namespace parsers
} // namespace core
} // namespace black_library
