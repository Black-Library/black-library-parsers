/**
 * SourceInformation.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_SOURCE_INFORMATION_H__
#define __BLACK_LIBRARY_CORE_PARSERS_SOURCE_INFORMATION_H__

#include <string>

namespace black_library {

namespace core {

namespace parsers {

typedef enum {
    AO3_PARSER,
    FFN_PARSER,
    SBF_PARSER,
    RR_PARSER,

    _NUM_PARSERS_TYPE
} parser_t;

typedef uint8_t parser_rep;

namespace AO3 {
    extern std::string name;
    extern std::string short_name;
    extern std::string source_url;
} // namespace AO3

namespace FFN {
    extern std::string name;
    extern std::string short_name;
    extern std::string source_url;
} // namespace FFN

namespace RR {
    extern std::string name;
    extern std::string short_name;
    extern std::string source_url;
} // namespace RR

namespace SBF {
    extern std::string name;
    extern std::string short_name;
    extern std::string source_url;
} // namespace SBF

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
