/**
 * SeleniumAdapter.cc
 */

#include <SeleniumAdapter.h>

namespace black_library {

namespace core {

namespace parsers {

NetworkRequestResult SeleniumAdapter::NetworkRequest(const std::string& url) {
    NetworkRequestResult result;

    Py_Initialize();

    PyObject* sysPath = PySys_GetObject("path");
    if(!sysPath)
    {
        result.debug_string = "Unable to get PATH.";
        return result;
    }

    std::stringstream ss;
    ss << absolute(std::experimental::filesystem::current_path()).string();
    ss << "/ext/black-library-parsers/src";
    PyList_Append(sysPath, PyBytes_FromString(ss.str().c_str()));

    PyObject* pyModuleString = PyBytes_FromString("SeleniumAdapter");
    if(!pyModuleString)
    {
        result.debug_string = "Unable to parse PyString.";
        return result;
    }

    PyObject* pyModule = PyImport_Import(pyModuleString);
    if(!pyModule)
    {
        result.debug_string = "Unable to find module.";
        return result;
    }

    PyObject* networkRequestFunc = PyObject_GetAttrString(pyModule, "NetworkRequest");
    if(!networkRequestFunc)
    {
        result.debug_string = "Unable to parse func name.";
        return result;
    }

    PyObject* args = PyTuple_Pack(1, PyBytes_FromString(url.c_str()));
    if(!args)
    {
        result.debug_string = "Unable to create args.";
        return result;
    }

    PyObject* pyResult = PyObject_CallObject(networkRequestFunc, args);
    if(!pyResult)
    {
        result.debug_string = "Unable to get pyResult.";
        return result;
    }

    std::string html_raw = PyBytes_AsString(pyResult);

    result.has_error = false;
    result.html = html_raw;
    return result;
}

} // namespace parsers
} // namespace core
} // namespace black_library
