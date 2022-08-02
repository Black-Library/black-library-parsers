#include <catch2/catch_test_macros.hpp>

#include <FileOperations.h>

#include <ParserManager.h>

#include <ParserTestUtils.h>

namespace black_library {

namespace core {

namespace parsers {

namespace BlackLibraryCommon = black_library::core::common;

TEST_CASE( "Generic parser manager tests (pass)", "[single-file]" )
{
    BlackLibraryCommon::MakeDirectories(DefaultTestStoragePath);
    njson config = GenerateParserTestConfig();
    ParserManager ParserManager(config);
    BlackLibraryCommon::RemovePath(DefaultTestStoragePath);
}

TEST_CASE( "Parser manager current jobs tests (pass)", "[single-file]" )
{
    BlackLibraryCommon::MakeDirectories(DefaultTestStoragePath);
    njson config = GenerateParserTestConfig();
    ParserManager ParserManager(config);
    ParserManager.GetCurrentJobList();
    ParserManager.AddJob("some-uuid-0", RR_DUMMY_URL, RR_DUMMY_URL);
    BlackLibraryCommon::RemovePath(DefaultTestStoragePath);
}

} // namespace parsers
} // namespace core
} // namespace black_library
