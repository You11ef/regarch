#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace VectorAndMatrixNameSpace;

// Helpers for operator[](int) as get/set in Python
static double GSLVector_get_item(const cGSLVector& self, int index)
{
    // Calls operator[](int) const
    return self[index];
}

static void GSLVector_set_item(cGSLVector& self, int index, double value)
{
    self[index] = value;
}

// Assignment operator helpers
static cGSLVector& cGSLVector_assignDouble(cGSLVector& self, double val)
{
    return self = val;
}

static cGSLVector& cGSLVector_assignVector(cGSLVector& self, const cGSLVector& rhs)
{
    return self = rhs;
}

static cGSLVector& cGSLVector_assignMatrix(cGSLVector& self, const cGSLMatrix& mat)
{
    return self = mat; // calls operator=(const cGSLMatrix&)
}

// Overloads for ReAlloc
static void (cGSLVector::* ReAlloc_size_val)(int, double) =
&cGSLVector::ReAlloc;  // ReAlloc(int, double)

static void (cGSLVector::* ReAlloc_size_dptr)(int, double*) =
&cGSLVector::ReAlloc;  // ReAlloc(int, double*)

static void (cGSLVector::* ReAlloc_vectRef)(const cGSLVector&) =
&cGSLVector::ReAlloc;  // ReAlloc(const cGSLVector&)

static void (cGSLVector::* ReAlloc_gslPtr)(gsl_vector*) =
&cGSLVector::ReAlloc;  // ReAlloc(gsl_vector*)

// Friend free functions for arithmetic operators
static cGSLVector cGSLVector_addDoubleLeft(double lhs, const cGSLVector& rhs)
{
    // Calls friend operator+(double, const cGSLVector&)
    return operator+(lhs, rhs);
}

static cGSLVector cGSLVector_addDoubleRight(const cGSLVector& lhs, double rhs)
{
    // A simple implementation: make a copy of lhs and add rhs elementwise.
    cGSLVector tmp(lhs);
    return tmp += rhs; // uses member operator+=(double)
}

// Helper for __str__
static std::string GSLVector_str(const cGSLVector& self)
{
    std::ostringstream oss;
    cGSLVector& nonConst = const_cast<cGSLVector&>(self);
    nonConst.Print(oss);
    return oss.str();
}

// Export function for cGSLVector
void export_cGSLVector()
{
    // Export cGSLVector as a class.
    class_<cGSLVector>("cGSLVector",
        "Wrapper for cGSLVector (the GSL-based vector class).",
        init< optional<int, double> >(
            (boost::python::arg("theSize") = 0, boost::python::arg("theVal") = 0.0),
            "Constructor cGSLVector(int theSize=0, double theVal=0.0)."
        )
    )
        .def(init<int, double*>((boost::python::arg("theSize"), boost::python::arg("theVect")),
            "Constructor cGSLVector(int theSize, double* theVect)."))

        // operator[] as get/set
        .def("get", &GSLVector_get_item)
        .def("set", &GSLVector_set_item)
        .def("__getitem__", &GSLVector_get_item)
        .def("__setitem__", &GSLVector_set_item)

        // Basic methods
        .def("GetSize", &cGSLVector::GetSize)
        .def("GetGSLVector", &cGSLVector::GetGSLVector,
            return_value_policy<reference_existing_object>(),
            "Returns the underlying gsl_vector* (opaque to Python).")
        .def("Delete", &cGSLVector::Delete)
        .def("Set", &cGSLVector::Set, (boost::python::arg("theValue"), boost::python::arg("theIndex")),
            "Set the value at position theIndex.")

        // ReAlloc overloads
        .def("ReAlloc", ReAlloc_size_val, (boost::python::arg("theSize"), boost::python::arg("theVal") = 0.0))
        .def("ReAlloc", ReAlloc_size_dptr)
        .def("ReAlloc", ReAlloc_vectRef)
        .def("ReAlloc", ReAlloc_gslPtr)

        // Assignment operators
        .def("AssignDouble", &cGSLVector_assignDouble,
            return_internal_reference<>(),
            "Assign a double value to all elements (self = double).")
        .def("AssignVector", &cGSLVector_assignVector,
            return_internal_reference<>(),
            "Assign another cGSLVector (self = other vector).")
        .def("AssignMatrix", &cGSLVector_assignMatrix,
            return_internal_reference<>(),
            "Assign from a cGSLMatrix (1-column assignment).")

        // In-place operators
        .def("AddDoubleInPlace", +[](cGSLVector& self, double val) {
        self += val;
        return &self;
            }, return_internal_reference<>(), "In-place addition with a double (self += double).")

        .def("AddVectorInPlace", +[](cGSLVector& self, const cGSLVector& rhs) {
        self += rhs;
        return &self;
            }, return_internal_reference<>(), "In-place addition with another vector (self += vector).")

        .def("SubtractDoubleInPlace", +[](cGSLVector& self, double val) {
        self -= val;
        return &self;
            }, return_internal_reference<>(), "In-place subtraction with a double (self -= double).")

        .def("SubtractVectorInPlace", +[](cGSLVector& self, const cGSLVector& rhs) {
        self -= rhs;
        return &self;
            }, return_internal_reference<>(), "In-place subtraction with another vector (self -= vector).")

        .def("MultiplyInPlace", +[](cGSLVector& self, double val) {
        self *= val;
        return &self;
            }, return_internal_reference<>(), "In-place multiplication with a double (self *= double).")

        .def("DivideInPlace", +[](cGSLVector& self, double val) {
        self /= val;
        return &self;
            }, return_internal_reference<>(), "In-place division with a double (self /= double).")

        // **str** method
        .def("__str__", &GSLVector_str)
        .def("__repr__", &GSLVector_str)
        ;

    // Export add operators as free functions 
    def("AddDoubleLeft", &cGSLVector_addDoubleLeft, "Add a scalar to each element of a vector");
    def("AddDoubleRight", &cGSLVector_addDoubleRight, "Add a scalar to each element of a vector");

    // Export convenience functions with explicit casts to resolve ambiguities
    def("Norm", static_cast<double(*)(const cGSLVector&)>(&Norm), "Calculate Euclidean norm of a vector");
    def("Sum", static_cast<double(*)(const cGSLVector&)>(&Sum), "Calculate sum of vector elements");

    // Fix for Maxi and Mini - add explicit casts
    def("Maxi", static_cast<double(*)(const cGSLVector&)>(&Maxi), "Find maximum value in vector");
    def("Mini", static_cast<double(*)(const cGSLVector&)>(&Mini), "Find minimum value in vector");

    def("Abs", static_cast<cGSLVector(*)(const cGSLVector&)>(&Abs), "Get absolute values of vector elements");
    def("IsNaN", static_cast<bool(*)(const cGSLVector&)>(&IsNaN), "Check if vector contains NaN values");
}