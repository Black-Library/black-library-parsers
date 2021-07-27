/**
 * SeleniumAdapter.cc
 */

#include <SeleniumAdapter.h>

namespace black_library {

namespace core {

namespace parsers {

NetworkRequestResult SeleniumAdapter::NetworkRequest(const std::string& url) {
    (void) url;
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
    PyList_Append(sysPath, PyUnicode_FromString(ss.str().c_str()));

    PyObject* pyModuleString = PyUnicode_FromString("SeleniumAdapter");
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
        result.debug_string = "Unable to get func.";
        return result;
    }

    PyObject* args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyUnicode_FromString(url.c_str()));
    if(!args)
    {
        result.debug_string = "Unable to create args.";
        return result;
    }

    PyObject* pyResult = PyObject_CallObject(networkRequestFunc, args);
    if(!pyResult)
    {
        result.debug_string = "Unable to get pyResult.";
        if (PyErr_Occurred()) {
            PyErr_PrintEx(0);
            PyErr_Clear(); // this will reset the error indicator so you can run Python code again
        }
        return result;
    }

    std::string html_raw = _PyUnicode_AsString(pyResult);

    result.has_error = false;
    result.html = html_raw;
    return result;
}

} // namespace parsers
} // namespace core
} // namespace black_library
