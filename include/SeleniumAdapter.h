/**
 * SeleniumAdapter.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_SELENIUMADAPTER_H__
#define __BLACK_LIBRARY_CORE_PARSERS_SELENIUMADAPTER_H__

#include <experimental/filesystem>
#include <memory>
#include <string>
#include <sstream>

#include <Python.h>

#include <NetworkAdapter.h>

namespace black_library {

namespace core {

namespace parsers {

class SeleniumAdapter : public NetworkAdapter
{
public:
    SeleniumAdapter();
    ~SeleniumAdapter() {};

    NetworkRequestResult NetworkRequest(const std::string& url) override;

private:
    void PrintPythonError();
};

} // namespace parsers
} // namespace core
} // namespace black_library
#endif
