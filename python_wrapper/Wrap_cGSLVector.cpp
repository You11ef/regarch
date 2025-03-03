// Wrap_cGSLVector.cpp
#include <boost/python.hpp>
#include <sstream>             // for std::ostringstream
#include "StdAfxVectorAndMatrix.h"  // This umbrella header includes cGSLVector.h and sets up typedefs and namespaces.

using namespace boost::python;
using namespace VectorAndMatrixNameSpace;  // cGSLVector is declared here

// Helper function for read access: returns the value at index i.
double gslvector_get(const cGSLVector& vec, int i) {
    return vec[i];  // Uses operator[] defined in cGSLVector.
}

// Helper function for write access: sets the element at index i.
void gslvector_set(cGSLVector& vec, int i, double value) {
    vec[i] = value;
}

// Updated helper function: now accepts a non-const reference.
// It creates an output string stream, calls Print with it, and returns the resulting string.
std::string gslvector_print(cGSLVector& vec) {
    std::ostringstream oss;
    vec.Print(oss);
    return oss.str();
}

void export_cGSLVector() {
    class_<cGSLVector>("cGSLVector", init<int, double>(
        (boost::python::arg("theSize") = 0, boost::python::arg("theVal") = 0.0),
        "Construct a cGSLVector with a given size and initial value"))
        .def("GetSize", &cGSLVector::GetSize, "Return the size of the vector")
        // Expose indexing using helper functions:
        .def("__getitem__", gslvector_get)
        .def("__setitem__", gslvector_set)
        // Expose ReAlloc (casting to the proper overload)
        .def("ReAlloc", (void (cGSLVector::*)(int, double)) & cGSLVector::ReAlloc,
            "Reallocate the vector with a new size and initial value")
        .def("Delete", &cGSLVector::Delete, "Delete the vector")
        // Expose our Print helper that returns a string.
        .def("Print", gslvector_print, "Return a string representation of the vector")
        ;
}
