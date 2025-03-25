// not used


#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

// Helper function to call the Print method.
void cPolynome_Print(cPolynome& self) {
    self.Print();
}

// Arithmetic helper functions that return by value instead of by reference.
cPolynome addPolynome(const cPolynome& a, const cPolynome& b) {
    cPolynome result(a);
    result += b;
    return result;
}

cPolynome subPolynome(const cPolynome& a, const cPolynome& b) {
    cPolynome result(a);
    result -= b;
    return result;
}

cPolynome mulPolynome(const cPolynome& a, const cPolynome& b) {
    cPolynome result(a);
    result *= b;
    return result;
}

cPolynome mulScalarPolynome(const cPolynome& p, double val) {
    cPolynome result(p);
    result *= val;
    return result;
}

// Helper for BackwardPolOp to provide its result by value.
double BackwardPolOpWrapper(const cPolynome& self, const cDVector& theYt, uint theIndex0 = 0, double thePow = 1.0) {
    return self.BackwardPolOp(theYt, theIndex0, thePow);
}

// Wrapper functions for free functions that return references.
// (They return a copy in our wrapper so that we can return by value.)
cPolynome TrunkMultWrapper(cPolynome& theP, cPolynome& theQ, uint theMaxDegree) {
    cPolynome result = TrunkMult(theP, theQ, theMaxDegree);
    return result;
}

cPolynome TrunkPolyWrapper(cPolynome& theP, uint theMaxDegree) {
    cPolynome result = TrunkPoly(theP, theMaxDegree);
    return result;
}

// Export function for the cPolynome module.
void export_cPolynom() {
    // Wrap the cPolynome class.
    class_<cPolynome>("cPolynome",
        "Polynomial class used in RegArch models.",
        init<optional<int> >(
            (boost::python::arg("theDegree") = -1),
            "Construct a cPolynome with a given degree (default -1)."
        )
    )
        // Also expose a constructor taking a degree and a pointer to coefficients.
        .def(init<int, double*>(
            (boost::python::arg("theDegree"), boost::python::arg("theCoeff")),
            "Construct a cPolynome with a given degree and coefficient array."
        ))
        .def(init<const cPolynome&>())
        .def("Delete", &cPolynome::Delete)
        .def("Resize", &cPolynome::Resize, (boost::python::arg("theDegree")))
        .def("Print", cPolynome_Print)

        // Wrap operator[] as __getitem__ and __setitem__.
        .def("__getitem__", +[](cPolynome& self, int index) -> double {
        return self[index];
            })
        .def("__setitem__", +[](cPolynome& self, int index, double value) {
        self[index] = value;
            })

        // Wrap in-place operators with return_internal_reference policy.
        .def("__iadd__", &cPolynome::operator+=, return_internal_reference<>())
        .def("__isub__", &cPolynome::operator-=, return_internal_reference<>())
        .def("__imul__", +[](cPolynome& self, const cPolynome& other) -> cPolynome& {
        return self *= other;
            }, return_internal_reference<>())
        .def("__imul__", +[](cPolynome& self, double val) -> cPolynome& {
        return self *= val;
            }, return_internal_reference<>())

        // Wrap binary operators using our helper functions.
        .def("__add__", addPolynome)
        .def("__sub__", subPolynome)
        .def("__mul__", mulPolynome)
        .def("__mul__", mulScalarPolynome)
        .def("__rmul__", mulScalarPolynome)

        // Wrap the BackwardPolOp method.
        .def("BackwardPolOp", BackwardPolOpWrapper,
            (boost::python::arg("theYt"), boost::python::arg("theIndex0") = 0, boost::python::arg("thePow") = 1.0))
        ;

    // Wrap free functions.
    def("ComputeDeltaPowD", ComputeDeltaPowD,
        (boost::python::arg("theD"), boost::python::arg("theDegree"), boost::python::arg("theRes")));
    def("ComputeLogDelta", ComputeLogDelta,
        (boost::python::arg("theDegree"), boost::python::arg("theRes")));
    def("IncrPowDiv", IncrPowDiv,
        (boost::python::arg("theNum"), boost::python::arg("theDen"), boost::python::arg("theOrder"),
            boost::python::arg("theQuot"), boost::python::arg("theRest")));

    // Wrap functions that return references via our wrappers.
    def("TrunkMult", TrunkMultWrapper,
        (boost::python::arg("theP"), boost::python::arg("theQ"), boost::python::arg("theMaxDegree")));
    def("TrunkPoly", TrunkPolyWrapper,
        (boost::python::arg("theP"), boost::python::arg("theMaxDegree")));
}