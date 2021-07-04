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
    NetworkAdapter();
    virtual ~NetworkAdapter();

    virtual NetworkRequestResult NetworkRequest(const std::string& url) = 0;
};

} // namespace parsers
} // namespace core
} // namespace black_library
#endif
