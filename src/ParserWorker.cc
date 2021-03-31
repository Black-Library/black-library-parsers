/**
 * ParserWorker.cc
 */

#include <ParserWorker.h>

namespace black_library {

namespace core {

namespace parsers {

ParserWorker::ParserWorker(parser_rep parser_type, std::shared_ptr<Parser> parser_ptr) :
    parsers_(),
    pool_(_MANAGED_PARSER_COUNT),
    job_queue_(),
    parser_type_(parser_type),
    done_(true)
{
    for (size_t i = 0; i < _MANAGED_PARSER_COUNT; ++i)
    {
        parsers_.emplace_back(std::static_pointer_cast<Parser>(parser_ptr));
    }
}

} // namespace parsers
} // namespace core
} // namespace black_library
