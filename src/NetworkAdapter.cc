/**
 * NetworkAdapter.cc
 */

#include <NetworkAdapter.h>

namespace black_library {

namespace core {

namespace parsers {

NetworkAdapter::NetworkAdapter(size_t wait_time) 
{
    wait_time_ = wait_time;
};

NetworkAdapter::~NetworkAdapter() {};

} // namespace parsers
} // namespace core
} // namespace black_library
