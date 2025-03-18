#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;
using namespace VectorAndMatrixNameSpace;

// -----------------------------------------------------
// 1. Overload disambiguation typedefs for simulation and LLH functions
typedef double (*LLHFunc1)(const cRegArchModel&, cDVector*, cDMatrix*);
typedef double (*LLHFunc2)(const cRegArchModel&, cRegArchValue&);

typedef void (*SimulFuncFull)(const unsigned int, const cRegArchModel&, cDVector&, cDMatrix*, cDMatrix*);

// IMPORTANT: Add typedef for the overload of RegArchSimul that takes a cRegArchValue&
typedef void (*SimulFunc2)(const unsigned int, const cRegArchModel&, cRegArchValue&);

// -----------------------------------------------------
// 2. Wrapper function for RegArchSimul that accepts default parameters
//    We use boost::python::object for the optional parameters.
void RegArchSimul_default(unsigned int theNSample,
    const cRegArchModel& theModel,
    cDVector& theYt,
    object theXt = object(),
    object theXvt = object())
{
    cDMatrix* xt = NULL;
    cDMatrix* xvt = NULL;

    // If theXt is provided (and not None), extract the pointer.
    if (!(theXt == object()))
    {
        xt = extract<cDMatrix*>(theXt);
    }

    // Similarly for theXvt.
    if (!(theXvt == object()))
    {
        xvt = extract<cDMatrix*>(theXvt);
    }

    // Call the original RegArchSimul with the optional pointers (which will be NULL if not provided).
    RegArchSimul(theNSample, theModel, theYt, xt, xvt);
}

// -----------------------------------------------------
// 3. For all other functions, we use explicit typedefs as needed.
typedef void (*FillValueFunc)(uint, const cRegArchModel&, cRegArchValue&);
typedef void (*FillValueForNumericGradFunc)(uint, const cRegArchModel&, cRegArchValue&, cNumericDerivative&);
typedef void (*FillValueForNumericGradAndHessFunc)(uint, const cRegArchModel&, cRegArchValue&, cNumericDerivative&);
typedef void (*RegArchGradLtFunc)(int, cRegArchModel&, cRegArchValue&, cRegArchGradient&, cDVector&);
typedef void (*RegArchLtAndGradLtFunc)(int, cRegArchModel&, cRegArchValue&, cRegArchGradient&, double&, cDVector&);
typedef void (*NumericRegArchGradLtFunc)(uint, cRegArchModel&, cRegArchValue*, cDVector&, double);
typedef void (*RegArchGradLLHFunc)(cRegArchModel&, cRegArchValue&, cDVector&);
typedef void (*RegArchLLHAndGradLLHFunc)(cRegArchModel&, cRegArchValue&, double&, cDVector&);
typedef void (*NumericRegArchHessLtFunc)(int, cRegArchModel&, cRegArchValue*, cRegArchGradient*, cDMatrix&, double);
typedef void (*RegArchHessLtFunc)(int, cRegArchModel&, cRegArchValue&, cRegArchGradient&, cRegArchHessien&, cDMatrix&);
typedef void (*RegArchGradAndHessLtFunc)(int, cRegArchModel&, cRegArchValue&, cRegArchGradient&, cRegArchHessien&, cDVector&, cDMatrix&);
typedef void (*RegArchLtGradAndHessLtFunc)(int, cRegArchModel&, cRegArchValue&, double&, cRegArchGradient&, cRegArchHessien&, cDVector&, cDMatrix&);
typedef void (*RegArchHessLLHFunc)(cRegArchModel&, cRegArchValue&, cDMatrix&);
typedef void (*NumericRegArchGradLLHFunc)(cRegArchModel&, cRegArchValue&, cDVector&, double);
typedef void (*NumericRegArchHessLLHFunc)(cRegArchModel&, cRegArchValue&, cDMatrix&, double);

typedef void (*NumericComputeCovFunc)(cRegArchModel&, cRegArchValue&, cDMatrix&);
typedef void (*RegArchComputeCovFunc)(cRegArchModel&, cRegArchValue&, cDMatrix&);
typedef void (*RegArchComputeCovErrFunc)(cRegArchModel&, cRegArchValue&, cDMatrix&, int&);
typedef void (*RegArchComputeIFunc)(cRegArchModel&, cRegArchValue&, cDMatrix&);
typedef void (*NumericRegArchHessLLHoldFunc)(cRegArchModel&, cRegArchValue&, cDMatrix&, double);
typedef void (*RegArchComputeIAndJFunc)(cRegArchModel&, cRegArchValue&, cDMatrix&, cDMatrix&);
typedef void (*RegArchStatTableFunc)(cRegArchModel&, cRegArchValue&, cDMatrix&);

// -----------------------------------------------------
// 4. Export function
void export_RegArchCompute()
{
    // Export simulation functions:
    // We export our wrapper with default arguments under the name "RegArchSimul".
    def("RegArchSimul", RegArchSimul_default,
        (boost::python::arg("theNSample"), boost::python::arg("theModel"), boost::python::arg("theYt"),
            boost::python::arg("theXt") = object(), boost::python::arg("theXvt") = object()));
    // Also export the overload that takes a cRegArchValue for simulation:
    def("RegArchSimul_from_value", static_cast<SimulFunc2>(RegArchSimul));

    // Export LLH functions:
    def("RegArchLLH", static_cast<LLHFunc1>(RegArchLLH));
    def("RegArchLLH_from_value", static_cast<LLHFunc2>(RegArchLLH));

    // Export FillValue functions:
    def("FillValue", static_cast<FillValueFunc>(FillValue));
    def("FillValueForNumericGrad", static_cast<FillValueForNumericGradFunc>(FillValueForNumericGrad));
    def("FillValueForNumericGradAndHess", static_cast<FillValueForNumericGradAndHessFunc>(FillValueForNumericGradAndHess));

    // Export gradient and Hessian functions:
    def("RegArchGradLt", static_cast<RegArchGradLtFunc>(RegArchGradLt));
    def("RegArchLtAndGradLt", static_cast<RegArchLtAndGradLtFunc>(RegArchLtAndGradLt));
    def("NumericRegArchGradLt", static_cast<NumericRegArchGradLtFunc>(NumericRegArchGradLt));
    def("RegArchGradLLH", static_cast<RegArchGradLLHFunc>(RegArchGradLLH));
    def("RegArchLLHAndGradLLH", static_cast<RegArchLLHAndGradLLHFunc>(RegArchLLHAndGradLLH));
    def("NumericRegArchHessLt", static_cast<NumericRegArchHessLtFunc>(NumericRegArchHessLt));
    def("RegArchHessLt", static_cast<RegArchHessLtFunc>(RegArchHessLt));
    def("RegArchGradAndHessLt", static_cast<RegArchGradAndHessLtFunc>(RegArchGradAndHessLt));
    def("RegArchLtGradAndHessLt", static_cast<RegArchLtGradAndHessLtFunc>(RegArchLtGradAndHessLt));
    def("RegArchHessLLH", static_cast<RegArchHessLLHFunc>(RegArchHessLLH));
    def("NumericRegArchGradLLH", static_cast<NumericRegArchGradLLHFunc>(NumericRegArchGradLLH));
    // def("NumericRegArchHessLLH", static_cast<NumericRegArchHessLLHFunc>(NumericRegArchHessLLH)); // Omitted

    // Export covariance and other compute functions:
    def("NumericComputeCov", static_cast<NumericComputeCovFunc>(NumericComputeCov));
    def("RegArchComputeCov", static_cast<RegArchComputeCovFunc>(RegArchComputeCov));
    def("RegArchComputeCov_err", static_cast<RegArchComputeCovErrFunc>(RegArchComputeCov));
    def("RegArchComputeI", static_cast<RegArchComputeIFunc>(RegArchComputeI));
    def("NumericRegArchHessLLHold", static_cast<NumericRegArchHessLLHoldFunc>(NumericRegArchHessLLHold));
    def("RegArchComputeIAndJ", static_cast<RegArchComputeIAndJFunc>(RegArchComputeIAndJ));
    def("RegArchStatTable", static_cast<RegArchStatTableFunc>(RegArchStatTable));
}
