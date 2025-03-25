#include "PythonConversion.h"
#include <stdexcept>
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/handle.hpp>

using namespace boost::python;
using namespace RegArchLib;

cDVector py_list_or_tuple_to_cDVector(const object& pyObj)
{
    if (!PySequence_Check(pyObj.ptr()))
        throw std::runtime_error("Object is not a Python sequence (list or tuple).");

    Py_ssize_t n = PySequence_Size(pyObj.ptr());
    cDVector result(n);

    for (Py_ssize_t i = 0; i < n; i++)
    {
        // Use a handle to wrap the new reference returned by PySequence_GetItem.
        object item(handle<>(PySequence_GetItem(pyObj.ptr(), i)));
        double val = extract<double>(item);
        result[i] = val;
    }
    return result;
}

cDMatrix py_list_of_lists_to_cDMatrix(const object& pyObj)
{
    if (!PySequence_Check(pyObj.ptr()))
        throw std::runtime_error("Object is not a Python sequence (expected list of lists).");

    Py_ssize_t nRows = PySequence_Size(pyObj.ptr());
    if (nRows == 0)
        throw std::runtime_error("The provided list is empty.");

    object firstRow(handle<>(PySequence_GetItem(pyObj.ptr(), 0)));
    if (!PySequence_Check(firstRow.ptr()))
        throw std::runtime_error("The first element is not a sequence (expected list for a row).");

    Py_ssize_t nCols = PySequence_Size(firstRow.ptr());
    cDMatrix result(nRows, nCols);

    for (Py_ssize_t i = 0; i < nRows; i++)
    {
        object row(handle<>(PySequence_GetItem(pyObj.ptr(), i)));
        if (!PySequence_Check(row.ptr()) || PySequence_Size(row.ptr()) != nCols)
            throw std::runtime_error("All rows must be sequences of the same size.");

        for (Py_ssize_t j = 0; j < nCols; j++)
        {
            object item(handle<>(PySequence_GetItem(row.ptr(), j)));
            double val = extract<double>(item);
            result[i][j] = val;  // Using operator[] for both dimensions.
        }
    }
    return result;
}
