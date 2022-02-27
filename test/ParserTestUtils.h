/**
 * ParserTestUtils.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSER_TEST_UTILS_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSER_TEST_UTILS_H__

#include <ConfigOperations.h>

namespace black_library {

namespace core {

namespace parsers {

static constexpr const char DefaultTestDBPath[] = "/tmp/catalog.db";

njson GenerateTestConfig()
{
    njson j;


    return j;
}

njson GenerateEmptyTestConfig()
{
    njson j;

    return j;
}

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
