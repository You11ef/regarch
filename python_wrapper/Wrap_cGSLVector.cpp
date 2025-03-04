//
// Wrap_cGSLVector.cpp
//
#include <boost/python.hpp>
#include <sstream>
#include <string>
#include <cstdint>    // for std::intptr_t
#include "StdAfxVectorAndMatrix.h"  // This includes cGSLVector.h, etc.
#include "cGSLVector.h"

namespace bp = boost::python;
using namespace VectorAndMatrixNameSpace;  // cGSLVector is here

//-------------------------------------------------------------
// 1) Helpers for indexing and printing
//-------------------------------------------------------------
static double gslvector_get(const cGSLVector& vec, int i)
{
    return vec[i];  // cGSLVector::operator[](int)
}

static void gslvector_set(cGSLVector& vec, int i, double value)
{
    vec[i] = value;
}

static std::string gslvector_print(cGSLVector& vec)
{
    std::ostringstream oss;
    vec.Print(oss);
    return oss.str();
}

//-------------------------------------------------------------
// 2) Wrap some friend functions (like Norm, Sum, etc.)
//-------------------------------------------------------------
static double NormWrapper(const cGSLVector& v) { return Norm(v); }
static double SumWrapper(const cGSLVector& v) { return Sum(v); }
static double MaxiWrapper(const cGSLVector& v) { return Maxi(v); }
static double MiniWrapper(const cGSLVector& v) { return Mini(v); }
static cGSLVector AbsWrapper(const cGSLVector& v) { return Abs(v); }
static bool IsNaNWrapper(const cGSLVector& v) { return IsNaN(v); }

//-------------------------------------------------------------
// 3) Optional: create from a raw pointer address
//    We must use a 64-bit type (intptr_t) instead of 'long' to avoid warnings
//-------------------------------------------------------------
static bp::object cGSLVector_init_from_gsl(const bp::object& rawPtr)
{
    // Suppose user passes an integer address from Python. We do:
    //   intptr_t pointerVal = bp::extract<intptr_t>(some_python_integer)
    // Then reinterpret_cast to 'gsl_vector*'.
    std::intptr_t pointerVal = bp::extract<std::intptr_t>(rawPtr);
    gsl_vector* ptr = reinterpret_cast<gsl_vector*>(pointerVal);

    // Now construct cGSLVector from that pointer (advanced usage).
    cGSLVector v(ptr);
    return bp::object(v);
}

//-------------------------------------------------------------
// 4) The export function
//    We only wrap the subset of features that we know are actually
//    implemented in the teacher's .lib files to avoid link errors.
//-------------------------------------------------------------
void export_cGSLVector()
{
    // cGSLVector main class
    bp::class_<cGSLVector>("cGSLVector",
        bp::init<int, double>(
            (bp::arg("theSize") = 0,
                bp::arg("theVal") = 0.0),
            "Construct a cGSLVector(size, initialValue)."
        )
    )
        // Basic methods
        .def("GetSize", &cGSLVector::GetSize,
            "Return the size of the vector.")

        // Operator[] via __getitem__/__setitem__ in Python
        .def("__getitem__", &gslvector_get,
            bp::args("index"),
            "Get element at index (0-based).")
        .def("__setitem__", &gslvector_set,
            bp::args("index", "value"),
            "Set element at index (0-based).")

        // ReAlloc with (int, double)
        .def("ReAlloc",
            (void (cGSLVector::*)(int, double)) & cGSLVector::ReAlloc,
            (bp::arg("theSize"), bp::arg("theVal") = 0.0),
            "Reallocate the vector with new size, fill with theVal.")

        // Expose the Delete method
        .def("Delete", &cGSLVector::Delete,
            "Delete the internal vector data.")

        // Expose the Set method
        .def("Set",
            &cGSLVector::Set,
            (bp::arg("value"), bp::arg("index")),
            "Set element at [index] to value.")

#ifndef _RDLL_
        .def("Print",
            &gslvector_print,
            "Return a string representation of the vector.")
#else
        .def("Print", &gslvector_print)
#endif
        ;

    // Expose friend free functions that we know exist in the .lib:
    bp::def("Norm", &NormWrapper, "Euclidian norm of the vector.");
    bp::def("Sum", &SumWrapper, "Sum of elements in the vector.");
    bp::def("Maxi", &MaxiWrapper, "Maximum element in the vector.");
    bp::def("Mini", &MiniWrapper, "Minimum element in the vector.");
    bp::def("Abs", &AbsWrapper, "Elementwise absolute value of the vector.");
    bp::def("IsNaN", &IsNaNWrapper, "True if any element is NaN, else False.");

    // Optional advanced usage: create a cGSLVector from a raw pointer address:
    bp::def("cGSLVector_init_from_gsl", &cGSLVector_init_from_gsl,
        "Construct a cGSLVector from a pointer (64-bit int) to a gsl_vector* (advanced).");
}
