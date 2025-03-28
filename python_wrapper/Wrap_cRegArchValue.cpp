#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>
#include "PythonConversion.h"  // Now provides conversion functions
#include <stdexcept>
#include <sstream>

using namespace boost::python;
using namespace RegArchLib;

namespace {  // Anonymous namespace for converter registration

    // Converter for cDVector from a Python list or tuple.
    struct cDVector_from_python {
        cDVector_from_python() {
            converter::registry::push_back(&convertible, &construct, type_id<cDVector>());
        }
        static void* convertible(PyObject* obj_ptr) {
            if (!PySequence_Check(obj_ptr))
                return 0;
            return obj_ptr;
        }
        static void construct(PyObject* obj_ptr,
            converter::rvalue_from_python_stage1_data* data) {
            void* storage = ((converter::rvalue_from_python_storage<cDVector>*)data)->storage.bytes;
            object pyObj(handle<>(borrowed(obj_ptr)));
            new (storage) cDVector(py_list_or_tuple_to_cDVector(pyObj));
            data->convertible = storage;
        }
    };

    // Converter for cDMatrix from a Python list of lists.
    struct cDMatrix_from_python {
        cDMatrix_from_python() {
            converter::registry::push_back(&convertible, &construct, type_id<cDMatrix>());
        }
        static void* convertible(PyObject* obj_ptr) {
            if (!PySequence_Check(obj_ptr))
                return 0;
            return obj_ptr;
        }
        static void construct(PyObject* obj_ptr,
            converter::rvalue_from_python_stage1_data* data) {
            void* storage = ((converter::rvalue_from_python_storage<cDMatrix>*)data)->storage.bytes;
            object pyObj(handle<>(borrowed(obj_ptr)));
            new (storage) cDMatrix(py_list_of_lists_to_cDMatrix(pyObj));
            data->convertible = storage;
        }
    };

    static cDVector_from_python register_cDVector_from_python;
    static cDMatrix_from_python register_cDMatrix_from_python;

} // end anonymous namespace

// Wrap ComputeMeanAndVar(...) and ComputeVar(...) to return doubles or a Python tuple.
static boost::python::tuple cRegArchValue_ComputeMeanAndVar(cRegArchValue& self)
{
    double m = 0.0, v = 0.0;
    self.ComputeMeanAndVar(m, v);
    return boost::python::make_tuple(m, v);
}

static double cRegArchValue_ComputeVar(cRegArchValue& self)
{
    double v = 0.0;
    self.ComputeVar(v);
    return v;
}

// Wrap PrintValue to return a string - a more Pythonic approach.
static std::string cRegArchValue_PrintValue(cRegArchValue& self, bool theHeader = true, const char* theSep = "\t")
{
    std::ostringstream oss;
    self.PrintValue(oss, theHeader, theSep);
    return oss.str();
}

// String representation method for Python.
static std::string cRegArchValue_str(cRegArchValue& self)
{
    std::ostringstream oss;
    self.PrintValue(oss, true, "\t");
    return oss.str();
}

// Function to provide info about the object.
static std::string cRegArchValue_repr(cRegArchValue& self)
{
    std::ostringstream oss;
    oss << "cRegArchValue with " << self.mYt.GetSize() << " observations";
    return oss.str();
}

// New helper method to set Xt directly from a Python list of lists
static void cRegArchValue_SetXtFromPyList(cRegArchValue& self, object pyList)
{
    // Convert to cDMatrix
    cDMatrix matrix = py_list_of_lists_to_cDMatrix(pyList);

    // Apply to Xt by calling ReAllocXt with the matrix object
    self.ReAllocXt(matrix);
}

// Corrected helper method to set a single element in Xt
static void cRegArchValue_SetXtElement(cRegArchValue& self, int row, int col, double value)
{
    // Check if the matrix is allocated (has non-zero dimensions)
    if (self.mXt.GetNRow() == 0 || self.mXt.GetNCol() == 0) {
        PyErr_SetString(PyExc_RuntimeError, "X matrix not allocated");
        throw_error_already_set();
        return;
    }

    // Check row and column bounds
    if (row >= 0 && row < static_cast<int>(self.mXt.GetNRow()) &&
        col >= 0 && col < static_cast<int>(self.mXt.GetNCol())) {
        self.mXt.Set(value, row, col);
    }
    else {
        PyErr_SetString(PyExc_IndexError, "Matrix indices out of range");
        throw_error_already_set();
    }
}

// --- Factory function to construct cRegArchValue from a cDVector by value ---
// This function uses your registered conversion for cDVector to allow passing a Python list.
cRegArchValue* new_cRegArchValue_from_cDVector(const cDVector& yt)
{
    // Allocate a new cDVector on the heap from the passed value.
    cDVector* p_yt = new cDVector(yt);
    // Create a new cRegArchValue using the constructor that takes a cDVector pointer.
    return new cRegArchValue(p_yt, NULL, NULL);
}

void export_cRegArchValue()
{
    class_<cRegArchValue>("cRegArchValue",
        "Container for GARCH data, residuals, etc.",
        // Existing constructors.
        init< optional<uint, cDMatrix*, cDMatrix*> >(
            (boost::python::arg("theSize") = 0, boost::python::arg("theXt") = (cDMatrix*)NULL, boost::python::arg("theXvt") = (cDMatrix*)NULL),
            "cRegArchValue(uint theSize=0, cDMatrix* theXt=nullptr, cDMatrix* theXvt=nullptr)"
        )
    )
        .def(init< optional<cDVector*, cDMatrix*, cDMatrix*> >(
            (boost::python::arg("theYt") = (cDVector*)NULL, boost::python::arg("theXt") = (cDMatrix*)NULL, boost::python::arg("theXvt") = (cDMatrix*)NULL),
            "cRegArchValue(cDVector* theYt=nullptr, cDMatrix* theXt=nullptr, cDMatrix* theXvt=nullptr)"
        ))
        // Add a constructor that accepts a cDVector by value.
        .def("__init__", make_constructor(new_cRegArchValue_from_cDVector,
            default_call_policies(), (boost::python::arg("theYt"))))
        // Public data members.
        .def_readwrite("mYt", &cRegArchValue::mYt, "Vector of data")
        .def_readwrite("mXt", &cRegArchValue::mXt, "Matrix of regressors if any")
        .def_readwrite("mXvt", &cRegArchValue::mXvt, "Matrix of regressors in the variance model if any")
        .def_readwrite("mMt", &cRegArchValue::mMt, "Vector of conditional mean")
        .def_readwrite("mHt", &cRegArchValue::mHt, "Vector of conditional variance")
        .def_readwrite("mUt", &cRegArchValue::mUt, "Vector of residuals")
        .def_readwrite("mEpst", &cRegArchValue::mEpst, "Vector of standardized residuals")
        // Methods.
        .def("Delete", &cRegArchValue::Delete, "Delete all data and free memory")
        .def("ReAlloc", (void (cRegArchValue::*)(uint)) & cRegArchValue::ReAlloc,
            (boost::python::arg("theSize")),
            "ReAlloc with a new size for Yt, Mt, etc.")
        .def("ReAlloc", (void (cRegArchValue::*)(cDVector&)) & cRegArchValue::ReAlloc,
            (boost::python::arg("theYt")),
            "ReAlloc with a cDVector for Yt.")
        .def("ReAllocXt", (void (cRegArchValue::*)(uint, uint)) & cRegArchValue::ReAllocXt,
            (boost::python::arg("theNRow"), boost::python::arg("theNCol")),
            "Reallocate the regressor matrix Xt with the specified dimensions")
        .def("ReAllocXt", (void (cRegArchValue::*)(cDMatrix&)) & cRegArchValue::ReAllocXt,
            (boost::python::arg("theXt")),
            "Reallocate the regressor matrix Xt using the provided matrix")
        .def("ReAllocXvt", (void (cRegArchValue::*)(uint, uint)) & cRegArchValue::ReAllocXvt,
            (boost::python::arg("theNRow"), boost::python::arg("theNCol")),
            "Reallocate the variance regressor matrix Xvt with the specified dimensions")
        .def("ReAllocXvt", (void (cRegArchValue::*)(cDMatrix&)) & cRegArchValue::ReAllocXvt,
            (boost::python::arg("theXvt")),
            "Reallocate the variance regressor matrix Xvt using the provided matrix")
        .def("ComputeMeanAndVar", &cRegArchValue_ComputeMeanAndVar,
            "Compute mean and variance, returns as (mean, variance) tuple")
        .def("ComputeVar", &cRegArchValue_ComputeVar,
            "Compute variance, returns as double")
        .def("PrintValue", &cRegArchValue_PrintValue,
            (boost::python::arg("theHeader") = true, boost::python::arg("theSep") = "\t"),
            "Return formatted data as a string")
        .def("__str__", &cRegArchValue_str)
        .def("__repr__", &cRegArchValue_repr)

        // New Pythonic helpers for matrix operations
        .def("set_xt", &cRegArchValue_SetXtFromPyList,
            boost::python::arg("x_data"),
            "Set X matrix data from a Python list of lists.")
        .def("set_xt_element", &cRegArchValue_SetXtElement,
            (boost::python::arg("row"), boost::python::arg("col"), boost::python::arg("value")),
            "Set a single element in the X matrix.")
        ;
}