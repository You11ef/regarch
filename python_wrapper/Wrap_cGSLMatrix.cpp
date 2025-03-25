

#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>


using namespace boost::python;
using namespace VectorAndMatrixNameSpace;

// Helper functions for cGSLMatrix in Python
static double GSLMatrix_get_item(const cGSLMatrix& self, int row, int col)
{
    // uses operator[](row) then index col
    return self[row][col];
}

static void GSLMatrix_set_item(cGSLMatrix& self, int row, int col, double val)
{
    self[row][col] = val;
}

static std::string GSLMatrix_str(const cGSLMatrix& self)
{
    std::ostringstream oss;
    cGSLMatrix& nonConst = const_cast<cGSLMatrix&>(self);
    nonConst.Print(oss);
    return oss.str();
}

// Assignment operator helpers
static cGSLMatrix& GSLMatrix_assignDouble(cGSLMatrix& self, double val)
{
    return self = val;
}

static cGSLMatrix& GSLMatrix_assignMatrix(cGSLMatrix& self, const cGSLMatrix& rhs)
{
    return self = rhs;
}

static cGSLMatrix& GSLMatrix_assignVector(cGSLMatrix& self, const cGSLVector& vec)
{
    return self = vec;
}

// Helper functions for matrix row access
//
// This helper class allows Python-style indexing: matrix[row][col]
struct MatrixRowHelper {
    cGSLMatrix& matrix;
    int row;

    MatrixRowHelper(cGSLMatrix& mat, int r) : matrix(mat), row(r) {}

    double getitem(int col) {
        if (col < 0 || col >= static_cast<int>(matrix.GetNCol())) {
            PyErr_SetString(PyExc_IndexError, "Column index out of range");
            throw_error_already_set();
        }
        return matrix[row][col];
    }

    void setitem(int col, double value) {
        if (col < 0 || col >= static_cast<int>(matrix.GetNCol())) {
            PyErr_SetString(PyExc_IndexError, "Column index out of range");
            throw_error_already_set();
        }
        matrix[row][col] = value;
    }
};

static object GSLMatrix_getitem(cGSLMatrix& self, int row)
{
    if (row < 0 || row >= static_cast<int>(self.GetNRow())) {
        PyErr_SetString(PyExc_IndexError, "Row index out of range");
        throw_error_already_set();
    }

    // Create a MatrixRowHelper and expose its methods to Python
    MatrixRowHelper helper(self, row);
    return class_<MatrixRowHelper>("MatrixRowHelper", no_init)
        .def("__getitem__", &MatrixRowHelper::getitem)
        .def("__setitem__", &MatrixRowHelper::setitem)
        .def("__repr__", +[](const MatrixRowHelper& h) {
        std::ostringstream oss;
        oss << "Row " << h.row << " of matrix";
        return oss.str();
            })
        (helper);
}

// Overloads for ReAlloc
static void (cGSLMatrix::* ReAlloc_rc_val)(uint, uint, double)
= &cGSLMatrix::ReAlloc;

static void (cGSLMatrix::* ReAlloc_cGSLVector)(const cGSLVector&)
= &cGSLMatrix::ReAlloc;

static void (cGSLMatrix::* ReAlloc_cGSLMatrix)(const cGSLMatrix&)
= &cGSLMatrix::ReAlloc;

// Export function for cGSLMatrix
void export_cGSLMatrix()
{
    // First register the row helper class
    class_<MatrixRowHelper>("MatrixRowHelper", no_init)
        .def("__getitem__", &MatrixRowHelper::getitem)
        .def("__setitem__", &MatrixRowHelper::setitem)
        ;

    // Now register the main matrix class
    class_<cGSLMatrix>("cGSLMatrix",
        "Wrapper for cGSLMatrix (GSL-based matrix).",
        init< optional<int, int, double> >(
            (boost::python::arg("theNRow") = 0, boost::python::arg("theNCol") = 0, boost::python::arg("theVal") = 0.0),
            "Constructor cGSLMatrix(int nRow=0, int nCol=0, double val=0.0)."
        )
    )
        .def(init<const cGSLVector&>((boost::python::arg("theVect")),
            "Constructor cGSLMatrix(const cGSLVector& theVect)."))

        .def(init<const cGSLMatrix&>((boost::python::arg("theMat")),
            "Constructor cGSLMatrix(const cGSLMatrix& theMat)."))

        // Basic methods
        .def("Delete", &cGSLMatrix::Delete)
        .def("GetNRow", &cGSLMatrix::GetNRow)
        .def("GetNCol", &cGSLMatrix::GetNCol)

        // Overload ReAlloc
        .def("ReAlloc", ReAlloc_rc_val,
            (boost::python::arg("theNRow"), boost::python::arg("theNCol"), boost::python::arg("theVal") = 0.0))
        .def("ReAlloc", ReAlloc_cGSLVector)
        .def("ReAlloc", ReAlloc_cGSLMatrix)

        // Access methods
        .def("get", &GSLMatrix_get_item)
        .def("set", &GSLMatrix_set_item)

        // Python-style indexing
        .def("__getitem__", &GSLMatrix_getitem)

        // Direct Set method
        .def("Set", &cGSLMatrix::Set,
            (boost::python::arg("theValue"), boost::python::arg("theRow"), boost::python::arg("theCol")))

        // Assignment operators
        .def("AssignDouble", &GSLMatrix_assignDouble,
            return_internal_reference<>(),
            "Assign a double value to all elements (self = double).")
        .def("AssignMatrix", &GSLMatrix_assignMatrix,
            return_internal_reference<>(),
            "Assign another cGSLMatrix (self = other matrix).")
        .def("AssignVector", &GSLMatrix_assignVector,
            return_internal_reference<>(),
            "Assign from a cGSLVector (creates a column matrix).")

        // In-place operators
        .def("AddDoubleInPlace", +[](cGSLMatrix& self, double val) {
        self += val;
        return &self;
            }, return_internal_reference<>())

        .def("AddMatrixInPlace", +[](cGSLMatrix& self, const cGSLMatrix& rhs) {
        self += rhs;
        return &self;
            }, return_internal_reference<>())

        .def("SubtractDoubleInPlace", +[](cGSLMatrix& self, double val) {
        self -= val;
        return &self;
            }, return_internal_reference<>())

        .def("SubtractMatrixInPlace", +[](cGSLMatrix& self, const cGSLMatrix& rhs) {
        self -= rhs;
        return &self;
            }, return_internal_reference<>())

        .def("MultiplyDoubleInPlace", +[](cGSLMatrix& self, double val) {
        self *= val;
        return &self;
            }, return_internal_reference<>())

        .def("MultiplyMatrixInPlace", +[](cGSLMatrix& self, const cGSLMatrix& rhs) {
        self *= rhs;
        return &self;
            }, return_internal_reference<>())

        .def("DivideInPlace", +[](cGSLMatrix& self, double val) {
        self /= val;
        return &self;
            }, return_internal_reference<>())

        // Row/column manipulation
        .def("SetRow", &cGSLMatrix::SetRow)
        .def("SetColumn", &cGSLMatrix::SetColumn)

        // String representation
        .def("__str__", &GSLMatrix_str)
        .def("__repr__", &GSLMatrix_str)
        ;

    // Export matrix operations with explicit casts to resolve ambiguities
    def("Transpose", static_cast<cGSLMatrix(*)(const cGSLMatrix&)>(Transpose),
        "Transpose a matrix");
    def("TransposeVector", static_cast<cGSLMatrix(*)(const cGSLVector&)>(Transpose),
        "Create a row matrix as transpose of a vector");
    def("Inv", static_cast<cGSLMatrix(*)(const cGSLMatrix&)>(Inv),
        "Invert a matrix");
    def("Identity", Identity,
        "Create identity matrix of specified size");
    def("Zeros", Zeros,
        "Create zero matrix of specified dimensions");

    // Removed Diag function that's causing linking issues

    def("Abs", static_cast<cGSLMatrix(*)(const cGSLMatrix&)>(Abs),
        "Take absolute value of each element in matrix");
    def("Maxi", static_cast<double(*)(const cGSLMatrix&)>(Maxi),
        "Find maximum value in matrix");
    def("Mini", static_cast<double(*)(const cGSLMatrix&)>(Mini),
        "Find minimum value in matrix");
    def("IsNaN", static_cast<bool(*)(const cGSLMatrix&)>(IsNaN),
        "Check if matrix contains NaN values");
    def("ClearMatrix", ClearMatrix,
        "Clear a matrix (set all elements to zero)");

}