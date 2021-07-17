#include <experimental/filesystem>

#include <Python.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>

int main(int argc, char *argv[]) {
    Py_Initialize();

    PyObject* sysPath = PySys_GetObject("path");
    if(!sysPath)
    {
        std::cout << "Unable to get PATH." << std::endl;
        return 1;
    }

    std::stringstream ss;
    ss << absolute(std::experimental::filesystem::current_path()).string();
    //ss << "/ext/black-library-parsers/practice";
    std::cout << ss.str() << std::endl;
    PyList_Append(sysPath, PyUnicode_FromString(ss.str().c_str()));

    PyObject* pyModuleString = PyUnicode_FromString("testPy");
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

    PyObject* pFunc = PyObject_GetAttrString(pyModule, "testFunc");
    if(!pFunc)
    {
        std::cout << "Unable to get func." << std::endl;
        return 1;
    }

    PyObject* args = PyTuple_New(2);
    PyTuple_SetItem(args, 0, PyLong_FromLong(2));
    PyTuple_SetItem(args, 1, PyUnicode_FromString("WORKED"));
    if(!args)
    {
        std::cout << "Unable to create args." << std::endl;
        return 1;
    }

    PyObject* pyResult = PyObject_CallObject(pFunc, args);
    if(!pyResult)
    {
        std::cout << "Unable to get pyResult." << std::endl;
        return 1;
    }

    double number = PyFloat_AsDouble(pyResult);

    std::cout << number << std::endl;
    return 0;
}
