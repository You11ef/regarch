//
// Wrap_cGSLMatrix.cpp
//
#include <boost/python.hpp>
#include <sstream>    // for std::ostringstream
#include <string>     // for std::string

// IMPORTANT: Do NOT do "using namespace std;" in this file.
// If you must include <complex> or <cmath>, do so, but avoid "using namespace std;"

#include "StdAfxVectorAndMatrix.h"  // where 'uint' etc. might be defined
#include "cGSLVector.h"             // ensure cGSLVector is known/wrapped
#include "cGSLMatrix.h"

// Create a shorter alias for boost::python to avoid confusion with std::arg
namespace bp = boost::python;

// We'll explicitly refer to the VectorAndMatrixNameSpace below.
using namespace VectorAndMatrixNameSpace;

//----------------------------------------------------------
// 1) Helpers for element get/set
//----------------------------------------------------------
static double gslmatrix_get(const cGSLMatrix& mat, int i, int j)
{
    return mat[i][j];
}
static void gslmatrix_set(cGSLMatrix& mat, int i, int j, double value)
{
    mat[i][j] = value;
}

//----------------------------------------------------------
// 2) Helper to wrap cGSLMatrix::Print(...) as a string
//----------------------------------------------------------
static std::string gslmatrix_print(cGSLMatrix& mat)
{
    std::ostringstream oss;
    mat.Print(oss);
    return oss.str();
}

//----------------------------------------------------------
// 3) Wrappers for free functions that operate on cGSLMatrix
//----------------------------------------------------------

// Svd(...) returns by reference: we wrap it to return (U, S, V) as a Python tuple
static bp::tuple Svd_wrapper(const cGSLMatrix& M)
{
    cGSLMatrix U, V;
    cGSLVector S;
    Svd(M, U, S, V);  // modifies U, S, V by reference
    return bp::make_tuple(U, S, V);
}

// Inv(...) no error
static cGSLMatrix Inv_noerr_wrapper(const cGSLMatrix& M)
{
    return Inv(M);
}

// Inv(...) with error
static bp::tuple Inv_err_wrapper(const cGSLMatrix& M)
{
    int errcode = 0;
    cGSLMatrix invMat = Inv(M, errcode);
    return bp::make_tuple(invMat, errcode);
}

// matrix * vector
static cGSLVector mat_mul_vec(const cGSLMatrix& M, const cGSLVector& v)
{
    return M * v; // friend operator*(cGSLMatrix, cGSLVector)
}

// vector * matrix
static cGSLMatrix vec_mul_mat(const cGSLVector& v, const cGSLMatrix& M)
{
    return v * M; // friend operator*(cGSLVector, cGSLMatrix)
}

//----------------------------------------------------------
// 4) The export function
//----------------------------------------------------------
void export_cGSLMatrix()
{
    //
    // Wrap class cGSLMatrix
    //
    bp::class_< cGSLMatrix >("cGSLMatrix",
        // constructor with named args:
        bp::init< int, int, double >(
            (bp::arg("theNRow") = 0,
                bp::arg("theNCol") = 0,
                bp::arg("theVal") = 0.0),
            "Construct a cGSLMatrix with (theNRow x theNCol) dimensions, "
            "all elements optionally set to theVal (default=0.0)."
        )
    )
        // Optionally, wrap copy constructors from cGSLMatrix or cGSLVector:
        .def(bp::init< const cGSLMatrix& >(
            bp::args("other"),
            "Copy-construct from another cGSLMatrix."))
        .def(bp::init< const cGSLVector& >(
            bp::args("vector"),
            "Construct a 1-column matrix from a cGSLVector."))

        // Basic methods
        .def("GetNRow", &cGSLMatrix::GetNRow, "Return the number of rows.")
        .def("GetNCol", &cGSLMatrix::GetNCol, "Return the number of columns.")
        .def("Delete", &cGSLMatrix::Delete, "Clear (delete) the matrix data.")

        // Access an element
        .def("get_item", &gslmatrix_get,
            bp::args("i", "j"),
            "Get element at row i, column j.")
        .def("set_item", &gslmatrix_set,
            bp::args("i", "j", "value"),
            "Set element at row i, column j to 'value'.")

        // ReAlloc overloads
        .def("ReAlloc",
            (void (cGSLMatrix::*)(uint, uint, double)) & cGSLMatrix::ReAlloc,
            (bp::arg("nRow"), bp::arg("nCol"), bp::arg("val") = 0.0),
            "Reallocate the matrix (nRow x nCol), fill with val.")
        .def("ReAlloc",
            (void (cGSLMatrix::*)(const cGSLVector&)) & cGSLMatrix::ReAlloc,
            bp::arg("vector"),
            "Reallocate so this matrix becomes a single-column copy of 'vector'.")
        .def("ReAlloc",
            (void (cGSLMatrix::*)(const cGSLMatrix&)) & cGSLMatrix::ReAlloc,
            bp::arg("matrix"),
            "Reallocate as a copy of another matrix (same shape, data).")

        // Set an element by (row,col)
        .def("Set",
            &cGSLMatrix::Set,
            (bp::arg("value"), bp::arg("row"), bp::arg("col")),
            "Set matrix[row,col] = value.")

#ifndef _RDLL_
        // Print -> string
        .def("Print",
            &gslmatrix_print,
            "Return a string representation of the matrix via Print().")
#else
        .def("Print",
            &gslmatrix_print,
            "Return a string representation of the matrix via Print().")
#endif

        // Set row/column from a vector
        .def("SetRow",
            &cGSLMatrix::SetRow,
            (bp::arg("rowIndex"), bp::arg("vector")),
            "Set entire row rowIndex from the given cGSLVector.")
        .def("SetColumn",
            &cGSLMatrix::SetColumn,
            (bp::arg("colIndex"), bp::arg("vector")),
            "Set entire column colIndex from the given cGSLVector.")

        // operator= variants as named methods
        .def("assign_matrix",
            (cGSLMatrix & (cGSLMatrix::*)(const cGSLMatrix&)) & cGSLMatrix::operator=,
            bp::args("other"),
            bp::return_internal_reference<>(),
            "Equivalent to operator= for another cGSLMatrix.")
        .def("assign_vector",
            (cGSLMatrix & (cGSLMatrix::*)(const cGSLVector&)) & cGSLMatrix::operator=,
            bp::args("vector"),
            bp::return_internal_reference<>(),
            "Equivalent to operator= for a cGSLVector (makes a 1-col matrix).")
        .def("assign_scalar",
            (cGSLMatrix & (cGSLMatrix::*)(double)) & cGSLMatrix::operator=,
            bp::args("val"),
            bp::return_internal_reference<>(),
            "Set *all* elements to the given scalar (like operator=(double)).")

        // In-place operators: +=, -=, *=, /=
        .def(bp::self += bp::self)
        .def(bp::self -= bp::self)
        .def("add_scalar_inplace",
            (cGSLMatrix & (cGSLMatrix::*)(double)) & cGSLMatrix::operator+=,
            bp::args("scalar"),
            bp::return_internal_reference<>(),
            "Matrix += scalar (in-place).")
        .def("sub_scalar_inplace",
            (cGSLMatrix & (cGSLMatrix::*)(double)) & cGSLMatrix::operator-=,
            bp::args("scalar"),
            bp::return_internal_reference<>(),
            "Matrix -= scalar (in-place).")
        .def("mul_matrix_inplace",
            (cGSLMatrix & (cGSLMatrix::*)(const cGSLMatrix&)) & cGSLMatrix::operator*=,
            bp::args("otherMatrix"),
            bp::return_internal_reference<>(),
            "Matrix = Matrix * otherMatrix (in-place).")
        .def("mul_scalar_inplace",
            (cGSLMatrix & (cGSLMatrix::*)(double)) & cGSLMatrix::operator*=,
            bp::args("scalar"),
            bp::return_internal_reference<>(),
            "Matrix *= scalar (in-place).")
        .def("div_scalar_inplace",
            (cGSLMatrix & (cGSLMatrix::*)(double)) & cGSLMatrix::operator/=,
            bp::args("scalar"),
            bp::return_internal_reference<>(),
            "Matrix /= scalar (in-place).")

        // Overloaded operators for matrix + matrix, matrix + double, etc.
        .def(bp::self + bp::self)
        .def(bp::self - bp::self)
        .def(bp::self * bp::self)
        .def(bp::self + double())
        .def(bp::self - double())
        .def(double() + bp::self)
        .def(double() - bp::self)
        .def(bp::self * double())
        .def(bp::self / double())
        .def(double() * bp::self)
        ;

    //
    // Wrap free functions (Zeros, Identity, Svd, Inv, etc.)
    //
    bp::def("Zeros",
        &Zeros,
        "Zeros(n, p) -> Return an n-by-p cGSLMatrix of all 0.0");

    bp::def("Identity",
        &Identity,
        "Identity(n) -> Return the n-by-n identity matrix.");

    // NOTE: we comment out Diag(...) and MatrixForMultNormal(...)
    // because the user's library or signature doesn't match and
    // it causes link errors. If you have a matching definition
    // in your library, uncomment these lines.
    //
    //    bp::def("Diag",
    //        &Diag,
    //        "Diag(vector) -> Return a diagonal matrix from the given cGSLVector."
    //    );

        // SVD => (U,S,V)
    bp::def("Svd",
        &Svd_wrapper,
        "Svd(M) -> (U, S, V)  (Singular Value Decomposition).");

    // Inv => no error
    bp::def("Inv",
        &Inv_noerr_wrapper,
        "Inv(M) -> Return the inverse of matrix M (throws if singular).");

    // Inv => with error code
    bp::def("InvWithError",
        &Inv_err_wrapper,
        "InvWithError(M) -> (InverseMatrix, errorCode). errorCode != 0 if fail.");

    // NOTE: we comment out MatrixForMultNormal(...) for the same reason:
    //
    //    bp::def("MatrixForMultNormal",
    //        &MatrixForMultNormal,
    //        "MatrixForMultNormal(covMat) -> cGSLMatrix used for sampling from a multivariate normal."
    //    );

    bp::def("ClearMatrix",
        &ClearMatrix,
        "ClearMatrix(M) sets all elements of M to 0.0.");

    // Transpose
    bp::def("Transpose",
        (cGSLMatrix(*)(const cGSLMatrix&)) & Transpose,
        "Transpose(M) -> Return a new cGSLMatrix that is the transpose of M.");

    bp::def("TransposeVector",
        (cGSLMatrix(*)(const cGSLVector&)) & Transpose,
        "TransposeVector(v) -> Return a 1xN cGSLMatrix from cGSLVector v.");

    // Abs returns cGSLMatrix
    bp::def("Abs",
        (cGSLMatrix(*)(const cGSLMatrix&)) & VectorAndMatrixNameSpace::Abs,
        "Abs(M) -> Return elementwise absolute value of M.");

    // Mini returns double
    bp::def("Mini",
        (double(*)(const cGSLMatrix&)) & VectorAndMatrixNameSpace::Mini,
        "Mini(M) -> Return the minimum element in M.");

    // Maxi returns double
    bp::def("Maxi",
        (double(*)(const cGSLMatrix&)) & VectorAndMatrixNameSpace::Maxi,
        "Maxi(M) -> Return the maximum element in M.");

    // IsNaN returns bool
    bp::def("IsNaN",
        (bool(*)(const cGSLMatrix&)) & VectorAndMatrixNameSpace::IsNaN,
        "IsNaN(M) -> True if any element of M is NaN, else False.");

    // matrix*vector, vector*matrix
    bp::def("mat_mul_vec",
        &mat_mul_vec,
        "mat_mul_vec(M, v) -> M*v (a cGSLVector).");
    bp::def("vec_mul_mat",
        &vec_mul_mat,
        "vec_mul_mat(v, M) -> v*M (a cGSLMatrix).");
}
