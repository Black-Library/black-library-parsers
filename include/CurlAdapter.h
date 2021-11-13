/**
 * NetworkAdapter.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_CURLADAPTER_H__
#define __BLACK_LIBRARY_CORE_PARSERS_CURLADAPTER_H__

#include <memory>
#include <string>

#include <curl/curl.h>

#include <NetworkAdapter.h>

namespace black_library {

namespace core {

namespace parsers {

class CurlAdapter : public NetworkAdapter
{
public:
    CurlAdapter();
    ~CurlAdapter() {};

    NetworkRequestResult NetworkRequest(const std::string& url) override;
};

size_t HandleCurlResponse(void* prt, size_t size, size_t nmemb, void* data);

} // namespace parsers
} // namespace core
} // namespace black_library
#endif