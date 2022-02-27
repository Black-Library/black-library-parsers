/**
 * NetworkAdapter.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_NETWORKADAPTER_H__
#define __BLACK_LIBRARY_CORE_PARSERS_NETWORKADAPTER_H__

#include <iostream>
#include <string>
#include <sstream>

#include "ParserCommon.h"

namespace black_library {

namespace core {

namespace parsers {

class NetworkAdapter
{
public:
    NetworkAdapter(size_t wait_time = 1);
    virtual ~NetworkAdapter();

    virtual NetworkRequestResult NetworkRequest(const std::string& url) = 0;
protected:
    size_t wait_time_;
};

} // namespace parsers
} // namespace core
} // namespace black_library
#endif
