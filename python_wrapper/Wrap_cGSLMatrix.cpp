// Wrap_cGSLMatrix.cpp
#include <boost/python.hpp>
#include <sstream>             // for std::ostringstream
#include "StdAfxVectorAndMatrix.h"  // Ensure proper definitions (e.g. uint, ostream, etc.)
#include "cGSLMatrix.h"

using namespace boost::python;
using namespace VectorAndMatrixNameSpace;  // cGSLMatrix is in this namespace

// Helper function: get element (i,j) from the matrix.
// It assumes that operator[](int) returns a pointer to the row.
double gslmatrix_get(const cGSLMatrix& mat, int i, int j) {
    double* row = mat[i];  // operator[] returns a pointer to row i.
    return row[j];
}

// Helper function: set element (i,j) in the matrix.
void gslmatrix_set(cGSLMatrix& mat, int i, int j, double value) {
    double* row = mat[i];
    row[j] = value;
}

// Updated helper function: now accepts a non-const reference.
// It creates an output string stream, calls Print, and returns the resulting string.
std::string gslmatrix_print(cGSLMatrix& mat) {
    std::ostringstream oss;
    mat.Print(oss);
    return oss.str();
}

void export_cGSLMatrix() {
    class_<cGSLMatrix>("cGSLMatrix", init<int, int, double>(
        (boost::python::arg("theNRow") = 0, boost::python::arg("theNCol") = 0, boost::python::arg("theVal") = 0.0),
        "Construct a cGSLMatrix with given number of rows, columns, and initial value"))
        .def("GetNRow", &cGSLMatrix::GetNRow, "Return the number of rows")
        .def("GetNCol", &cGSLMatrix::GetNCol, "Return the number of columns")
        // Expose element access using helper functions:
        .def("get_item", gslmatrix_get, "Get matrix element at (i,j)")
        .def("set_item", gslmatrix_set, "Set matrix element at (i,j)")
        // Expose ReAlloc with an explicit cast (note: we use uint, which should now be defined)
        .def("ReAlloc", (void (cGSLMatrix::*)(uint, uint, double)) & cGSLMatrix::ReAlloc,
            "Reallocate the matrix with new dimensions and initial value")
        .def("Delete", &cGSLMatrix::Delete, "Delete the matrix")
#ifndef _RDLL_
        .def("Print", gslmatrix_print, "Return a string representation of the matrix")
#else
        .def("Print", gslmatrix_print, "Return a string representation of the matrix")
#endif
        ;
}
