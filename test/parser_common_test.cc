#include <catch2/catch_test_macros.hpp>

#include <ParserCommon.h>
#include <SourceInformation.h>

namespace black_library {

namespace core {

namespace parsers {

static constexpr const char RR_URL[] = "https://www.royalroad.com/fiction/00000/some-fiction";

bool ContainsUsingFind(const std::string &haystack, const std::string &needle)
{
    if (haystack.find(needle) != std::string::npos)
        return true;

    return false;
}

TEST_CASE( "ContainsString generic tests (pass)", "[single-file]" ) {
    REQUIRE( ContainsString("haystack", "aystac") == true );
    REQUIRE( ContainsUsingFind("haystack", "aystac") == true );
    REQUIRE( ContainsString("haystack", "notneedle") == false );
    REQUIRE( ContainsUsingFind("haystack", "notneedle") == false );
    REQUIRE( ContainsString("haystack", "") == true );
    REQUIRE( ContainsUsingFind("haystack", "") == true );
    REQUIRE( ContainsString("", "aystac") == false );
    REQUIRE( ContainsUsingFind("", "aystac") == false );
}

TEST_CASE( "ContainsString parser url tests (pass)", "[single-file]" ) {
    REQUIRE( ContainsString( RR_URL, RR::source_url) == true );
    REQUIRE( ContainsString( RR_URL, SBF::source_url) == false );
    REQUIRE( ContainsString( RR::source_url, RR_URL) == false );
}

TEST_CASE( "TrimWhitespace generic tests (pass)", "[single-file]" ) {
    REQUIRE( TrimWhitespace(" \t\r\n\0") == "" );
    REQUIRE( TrimWhitespace(" \t\r\n\0\0\n\r\t ") == "" );
    REQUIRE( TrimWhitespace(" a\t\rb\nc") == "a\t\rb\nc" );
    REQUIRE( TrimWhitespace("a\t\rb\nc ") == "a\t\rb\nc" );
    REQUIRE( TrimWhitespace("\t\t\ta\r") == "a" );
    REQUIRE( TrimWhitespace(" a ") == "a" );
    REQUIRE( TrimWhitespace("\ta\t") == "a" );
    REQUIRE( TrimWhitespace("\ra\r") == "a" );
    REQUIRE( TrimWhitespace("\na\n") == "a" );
    // TODO: improve encoding to make this a reasonable test
    // REQUIRE( TrimWhitespace("\0a\0") == "a" );
    REQUIRE( TrimWhitespace("") == "" );
}

} // namespace parsers
} // namespace core
} // namespace black_library
