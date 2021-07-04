#include <experimental/filesystem>

#include <Python.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>

int main(int argc, char *argv[]) {

    std::stringstream ss;
    ss << absolute(std::experimental::filesystem::current_path()).string();
    ss << "/ext/black-library-parsers/practice";
    const size_t cSize = strlen(ss.str()+1);
    std::wstring str(cSize, ss.str().c_str());

    Py_SetPath(str);
    Py_Initialize();

    PyObject* pyModuleString = PyBytes_FromString("test");
    if(!pyModuleString)
    {
        std::cout << "Unable to parse PyString." << std::endl;
        return 1;
    }

    PyObject* pyModule = PyImport_Import(pyModuleString);
    if(!pyModule)
    {
        std::cout << "Unable to find module." << std::endl;
        return 1;
    }

    PyObject* networkRequestFunc = PyObject_GetAttrString(pyModule, "test");
    if(!networkRequestFunc)
    {
        std::cout << "Unable to parse func name." << std::endl;
        return 1;
    }

    PyObject* args = PyTuple_Pack(0, PyBytes_FromString((char*) "test"));
    if(!args)
    {
        std::cout << "Unable to create args." << std::endl;
        return 1;
    }

    PyObject* pyResult = PyObject_CallObject(networkRequestFunc, args);
    if(!pyResult)
    {
        std::cout << "Unable to get pyResult." << std::endl;
        return 1;
    }

    double number = PyFloat_AsDouble(pyResult);

    std::cout << number << std::endl;
    return 0;
}
