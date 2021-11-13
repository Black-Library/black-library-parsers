/**
 * SeleniumAdapter.cc
 */

#include <SeleniumAdapter.h>

#include <LogOperations.h>

namespace black_library {

namespace core {

namespace parsers {

namespace BlackLibraryCommon = black_library::core::common;

SeleniumAdapter::SeleniumAdapter() {
    BlackLibraryCommon::InitRotatingLogger("SeleniumAdapter", "/mnt/black-library/log/", false);
}

NetworkRequestResult SeleniumAdapter::NetworkRequest(const std::string& url) {
    NetworkRequestResult result;

    Py_Initialize();

    PyObject* sysPath = PySys_GetObject("path");
    if (!sysPath)
    {
        BlackLibraryCommon::LogError("SeleniumAdapter", "Unable to get Python PATH.");
        PrintPythonError();
        return result;
    }

    std::stringstream ss;
    ss << absolute(std::experimental::filesystem::current_path()).string();
    ss << "/ext/black-library-parsers/src";
    PyList_Append(sysPath, PyUnicode_FromString(ss.str().c_str()));

    PyObject* pyModuleString = PyUnicode_FromString("SeleniumAdapter");
    if (!pyModuleString)
    {
        BlackLibraryCommon::LogError("SeleniumAdapter", "Unable to parse PyString.");
        PrintPythonError();
        return result;
    }

    PyObject* pyModule = PyImport_Import(pyModuleString);
    if (!pyModule)
    {
        BlackLibraryCommon::LogError("SeleniumAdapter", "Unable to find module.");
        PrintPythonError();
        return result;
    }

    PyObject* networkRequestFunc = PyObject_GetAttrString(pyModule, "NetworkRequest");
    if (!networkRequestFunc)
    {
        BlackLibraryCommon::LogError("SeleniumAdapter", "Unable to get func.");
        PrintPythonError();
        return result;
    }

    PyObject* args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyUnicode_FromString(url.c_str()));
    if (!args)
    {
        BlackLibraryCommon::LogError("SeleniumAdapter", "Unable to create args.");
        PrintPythonError();
        return result;
    }

    PyObject* pyResult = PyObject_CallObject(networkRequestFunc, args);
    if (!pyResult)
    {
        BlackLibraryCommon::LogError("SeleniumAdapter", "Unable to get pyResult.");
        PrintPythonError();
        return result;
    }

    std::string html_raw = _PyUnicode_AsString(pyResult);

    result.has_error = false;
    result.html = html_raw;
    return result;
}

void SeleniumAdapter::PrintPythonError() {
    if (PyErr_Occurred()) {
        PyErr_PrintEx(0);
        PyErr_Clear(); // this will reset the error indicator so you can run Python code again
    }
}

} // namespace parsers
} // namespace core
} // namespace black_library
