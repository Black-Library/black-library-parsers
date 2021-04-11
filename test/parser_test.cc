#include <catch2/catch_test_macros.hpp>
#include <Parser.h>

namespace black_library {

namespace core {

namespace parsers {

TEST_CASE( "Generic parser tests (pass)", "[single-file]" )
{
    Parser parser;
    parser.SetLocalFilePath("foo");
    parser.SetParserIndex(0);
    parser.SetParserType();
    parser.SetUrl();
}

} // namespace parsers
} // namespace core
} // namespace black_library
