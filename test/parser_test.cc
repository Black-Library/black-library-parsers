#include <catch2/catch_test_macros.hpp>

#include <Parser.h>
#include <ParserRR.h>

namespace black_library {

namespace core {

namespace parsers {

static constexpr const char RR_DUMMY_URL[] = "https://www.royalroad.com/fiction/00000/some-fiction";

TEST_CASE( "Generic parser tests (pass)", "[single-file]" )
{
    Parser parser;
    parser.SetLocalFilePath("foo");
    parser.SetUrl("dummy_url");
}

TEST_CASE( "RR parser tests (pass)", "[single-file]" )
{
    RR::ParserRR parser;
    parser.SetLocalFilePath("foo");
    parser.SetUrl(RR_DUMMY_URL);
    REQUIRE(parser.GetParserType() == RR_PARSER);
    REQUIRE(parser.GetSourceUrl() == black_library::core::common::RR::source_url);
}

} // namespace parsers
} // namespace core
} // namespace black_library
