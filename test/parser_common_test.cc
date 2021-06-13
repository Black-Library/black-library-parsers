#include <catch2/catch_test_macros.hpp>

#include <SourceInformation.h>
#include <StringOperations.h>

#include <ParserCommon.h>

namespace black_library {

namespace core {

namespace parsers {

static constexpr const char RR_DUMMY_URL[] = "https://www.royalroad.com/fiction/00000/some-fiction";

namespace BlackLibraryCommon = black_library::core::common;

bool ContainsUsingFind(const std::string &haystack, const std::string &needle)
{
    if (haystack.find(needle) != std::string::npos)
        return true;

    return false;
}

TEST_CASE( "ContainsString generic tests (pass)", "[single-file]" )
{
    REQUIRE( BlackLibraryCommon::ContainsString("haystack", "aystac") == true );
    REQUIRE( ContainsUsingFind("haystack", "aystac") == true );
    REQUIRE( BlackLibraryCommon::ContainsString("haystack", "notneedle") == false );
    REQUIRE( ContainsUsingFind("haystack", "notneedle") == false );
    REQUIRE( BlackLibraryCommon::ContainsString("haystack", "") == true );
    REQUIRE( ContainsUsingFind("haystack", "") == true );
    REQUIRE( BlackLibraryCommon::ContainsString("", "aystac") == false );
    REQUIRE( ContainsUsingFind("", "aystac") == false );
}

TEST_CASE( "ContainsString parser url tests (pass)", "[single-file]" )
{
    REQUIRE( BlackLibraryCommon::ContainsString( RR_DUMMY_URL, BlackLibraryCommon::RR::source_url) == true );
    REQUIRE( BlackLibraryCommon::ContainsString( RR_DUMMY_URL, BlackLibraryCommon::SBF::source_url) == false );
    REQUIRE( BlackLibraryCommon::ContainsString( BlackLibraryCommon::RR::source_url, RR_DUMMY_URL) == false );
}

TEST_CASE( "TrimWhitespace generic tests (pass)", "[single-file]" )
{
    REQUIRE( BlackLibraryCommon::TrimWhitespace(" \t\r\n\0") == "" );
    REQUIRE( BlackLibraryCommon::TrimWhitespace(" \t\r\n\0\0\n\r\t ") == "" );
    REQUIRE( BlackLibraryCommon::TrimWhitespace(" a\t\rb\nc") == "a\t\rb\nc" );
    REQUIRE( BlackLibraryCommon::TrimWhitespace("a\t\rb\nc ") == "a\t\rb\nc" );
    REQUIRE( BlackLibraryCommon::TrimWhitespace("\t\t\ta\r") == "a" );
    REQUIRE( BlackLibraryCommon::TrimWhitespace(" a ") == "a" );
    REQUIRE( BlackLibraryCommon::TrimWhitespace("\ta\t") == "a" );
    REQUIRE( BlackLibraryCommon::TrimWhitespace("\ra\r") == "a" );
    REQUIRE( BlackLibraryCommon::TrimWhitespace("\na\n") == "a" );
    REQUIRE( BlackLibraryCommon::TrimWhitespace("") == "" );
}

} // namespace parsers
} // namespace core
} // namespace black_library
