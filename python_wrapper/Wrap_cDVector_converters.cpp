// not used


#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

namespace bp = boost::python;
using namespace VectorAndMatrixNameSpace; // Or whichever namespace has cDVector

//
// 1. Converting cDVector --> Python list
//
struct cDVector_to_python
{
    static PyObject* convert(const cDVector& vec)
    {
        // We'll return a plain Python list of floats:
        Py_ssize_t size = static_cast<Py_ssize_t>(vec.GetSize());
        PyObject* pyList = PyList_New(size);
        for (Py_ssize_t i = 0; i < size; ++i)
        {
            double val = vec[i];
            PyObject* pyVal = PyFloat_FromDouble(val);
            // PyList_SET_ITEM steals a reference to pyVal
            PyList_SET_ITEM(pyList, i, pyVal);
        }
        return pyList;
    }
};

//
// 2. Converting Python list/tuple --> cDVector
//
//    We implement a "from_python" converter, so that Boost.Python can do
//    `extract<cDVector>(some_python_list)` and get a real cDVector.
//
struct cDVector_from_python
{
    // Called once at module init to register this converter with the type system.
    cDVector_from_python()
    {
        bp::converter::registry::push_back(
            &convertible, // callable: checks if PyObject* is list-like
            &construct,   // callable: does the actual cDVector construction
            bp::type_id<cDVector>()
        );
    }

    static void* convertible(PyObject* obj_ptr)
    {
        // We'll allow either a Python list or a Python tuple:
        if (PyList_Check(obj_ptr) || PyTuple_Check(obj_ptr))
            return obj_ptr; // Indicate "yes, we can convert it."
        return nullptr;     // Indicate "no."
    }

    static void construct(PyObject* obj_ptr,
        bp::converter::rvalue_from_python_stage1_data* data)
    {
        // Grab pointer to memory into which we construct cDVector
        void* storage = (
            (bp::converter::rvalue_from_python_storage<cDVector>*)
            data)->storage.bytes;

        Py_ssize_t len = 0;
        bool isList = PyList_Check(obj_ptr);
        if (isList)
            len = PyList_Size(obj_ptr);
        else
            len = PyTuple_Size(obj_ptr);

        cDVector tmp(len);
        for (Py_ssize_t i = 0; i < len; ++i)
        {
            PyObject* item = isList
                ? PyList_GetItem(obj_ptr, i)
                : PyTuple_GetItem(obj_ptr, i);
            double val = bp::extract<double>(item);
            tmp[i] = val;
        }

        // Construct cDVector in-place:
        new (storage) cDVector(tmp);
        data->convertible = storage; // Done
    }
};

//
// 3. A helper function to actually *register* these converters 
//    with Boost.Python. Call this once in your module init.
//
void export_cDVector_converters()
{
    // Register the "cDVector -> Python list" converter:
    //bp::to_python_converter<cDVector, cDVector_to_python>();

    // Register the "Python list -> cDVector" converter:
    static cDVector_from_python s_cDVector_from_python;
}
