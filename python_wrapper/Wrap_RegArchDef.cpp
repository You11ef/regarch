#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>


using namespace boost::python;
using namespace RegArchLib;

// Wrap the macros as functions with both integer and double overloads
template <typename T>
T regarch_MIN(T a, T b) {
    return a < b ? a : b;
}

template <typename T>
T regarch_MAX(T a, T b) {
    return a > b ? a : b;
}

template <typename T>
int regarch_SIGN(T a) {
    return a > 0 ? 1 : -1;
}

BOOST_PYTHON_FUNCTION_OVERLOADS(min_overloads, regarch_MIN<double>, 2, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(max_overloads, regarch_MAX<double>, 2, 2)

void export_RegArchDef()
{
    // Expose the eCondMeanEnum with a docstring for the overall enum.
    enum_<eCondMeanEnum>("eCondMeanEnum", "Enumeration of different types of conditional means.")
        .value("eUnknown", eUnknown)
        .value("eConst", eConst)
        .value("eAr", eAr)
        .value("eMa", eMa)
        .value("eLinReg", eLinReg)
        .value("eStdDevInMean", eStdDevInMean)
        .value("eVarInMean", eVarInMean)
        .value("eArfima", eArfima)
        ;

    // Expose the eCondVarEnum.
    enum_<eCondVarEnum>("eCondVarEnum", "Enumeration of different types of conditional variances.")
        .value("eNotKnown", eNotKnown)
        .value("eCste", eCste)
        .value("eArch", eArch)
        .value("eGarch", eGarch)
        .value("eNgarch", eNgarch)
        .value("eEgarch", eEgarch)
        .value("eAparch", eAparch)
        .value("eTarch", eTarch)
        .value("eFigarch", eFigarch)
        .value("eUgarch", eUgarch)
        .value("eTsgarch", eTsgarch)
        .value("eGtarch", eGtarch)
        .value("eNagarch", eNagarch)
        .value("eSqrgarch", eSqrgarch)
        ;

    // Expose the eDistrTypeEnum.
    enum_<eDistrTypeEnum>("eDistrTypeEnum", "Enumeration of different types of conditional distributions.")
        .value("eUndefined", eUndefined)
        .value("eNormal", eNormal)
        .value("eStudent", eStudent)
        .value("eGed", eGed)
        .value("eMixNorm", eMixNorm)
        ;

    // Expose numeric constants as module attributes.
    scope().attr("PI") = PI;
    scope().attr("SQRT_PI") = SQRT_PI;
    scope().attr("SQRT_2_PI") = SQRT_2_PI;
    scope().attr("LOG_SQRT_2_PI") = LOG_SQRT_2_PI;
    scope().attr("LOG_PI") = LOG_PI;
    scope().attr("MAX_COND_MEAN") = MAX_COND_MEAN;
    scope().attr("MAX_COND_VAR") = MAX_COND_VAR;

    // Export wrapped MIN, MAX, and SIGN as functions with documentation.
    // Integer versions
    def("MIN_int", regarch_MIN<int>, "MIN_int(a, b): Returns the minimum of two integers a and b.");
    def("MAX_int", regarch_MAX<int>, "MAX_int(a, b): Returns the maximum of two integers a and b.");
    def("SIGN_int", regarch_SIGN<int>, "SIGN_int(a): Returns 1 if a > 0, otherwise returns -1.");

    // Float versions
    def("MIN_float", regarch_MIN<double>, "MIN_float(a, b): Returns the minimum of two floats a and b.");
    def("MAX_float", regarch_MAX<double>, "MAX_float(a, b): Returns the maximum of two floats a and b.");
    def("SIGN_float", regarch_SIGN<double>, "SIGN_float(a): Returns 1 if a > 0, otherwise returns -1.");

    // Generic versions (will dispatch based on type)
    def("MIN", regarch_MIN<int>, "MIN(a, b): Returns the minimum of two values a and b.");
    def("MIN", regarch_MIN<double>, "MIN(a, b): Returns the minimum of two values a and b.");
    def("MAX", regarch_MAX<int>, "MAX(a, b): Returns the maximum of two values a and b.");
    def("MAX", regarch_MAX<double>, "MAX(a, b): Returns the maximum of two values a and b.");
    def("SIGN", regarch_SIGN<int>, "SIGN(a): Returns 1 if a > 0, otherwise returns -1.");
    def("SIGN", regarch_SIGN<double>, "SIGN(a): Returns 1 if a > 0, otherwise returns -1.");
}