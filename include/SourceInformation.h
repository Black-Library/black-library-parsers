#ifndef SOURCEINFORMATION_H
#define SOURCEINFORMATION_H

#include <string>

namespace librarycore {

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
    extern std::string url;
} // namespace AO3

namespace FFN {
    extern std::string name;
    extern std::string short_name;
    extern std::string url;
} // namespace FFN

namespace SBF {
    extern std::string name;
    extern std::string short_name;
    extern std::string url;
} // namespace SBF

namespace RR {
    extern std::string name;
    extern std::string short_name;
    extern std::string url;
} // namespace RR

} // namespace librarycore
#endif
