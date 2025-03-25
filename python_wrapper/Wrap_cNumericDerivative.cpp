#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// If you want to convert Python lists to cDVector, etc.:
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

/*!
 * Export function for cNumericDerivative, bridging to Python.
 * This class is used to help with numerical gradient & Hessian computations,
 * so it typically includes direct data fields and small methods.
 */
void export_cNumericDerivative()
{
    class_<cNumericDerivative>("cNumericDerivative",
        "Numerical derivative computation helper for RegArch models.\n\n"
        "Used to compute gradients and Hessians numerically when analytical formulas are not available.\n\n"
        "Constructor Overloads:\n"
        "  - cNumericDerivative(n_param=0, n_distr_param=0, h=1e-2, n_obs=0)\n"
        "  - cNumericDerivative(n_param, n_distr_param, h, cDVector& yt, cDMatrix* xt=nullptr, cDMatrix* xvt=nullptr)",
        init< optional<uint, uint, double, uint> >(
            (boost::python::arg("n_param") = 0,
                boost::python::arg("n_distr_param") = 0,
                boost::python::arg("h") = 1e-2,
                boost::python::arg("n_obs") = 0),
            "Initialize a numerical derivative helper with specified parameters.\n\n"
            "Parameters:\n"
            "  n_param (int): number of main parameters\n"
            "  n_distr_param (int): number of distribution parameters\n"
            "  h (float): step size for numeric differencing\n"
            "  n_obs (int): number of observations (if relevant)\n"
        )
    )
        // Additional constructor with cDVector & cDMatrix arguments
        .def(init<uint, uint, double, cDVector&, cDMatrix*, cDMatrix*>(
            (boost::python::arg("n_param"),
                boost::python::arg("n_distr_param"),
                boost::python::arg("h"),
                boost::python::arg("yt"),
                boost::python::arg("xt") = static_cast<cDMatrix*>(nullptr),
                boost::python::arg("xvt") = static_cast<cDMatrix*>(nullptr)),
            "Initialize a numerical derivative helper with data vectors/matrices.\n\n"
            "Parameters:\n"
            "  n_param (int): number of main parameters\n"
            "  n_distr_param (int): number of distribution parameters\n"
            "  h (float): step size for numeric differencing\n"
            "  yt (cDVector): typically the time-series data or response vector\n"
            "  xt (cDMatrix*, optional): if using exogenous regressors\n"
            "  xvt (cDMatrix*, optional): an additional matrix for advanced usage\n"
        ))

        // Access methods
        .def("get_n_param", &cNumericDerivative::GetNParam,
            "Returns the number of parameters (n_param).")
        .def("get_n_distr_param", &cNumericDerivative::GetNDistrParam,
            "Returns the number of distribution parameters (n_distr_param).")
        .def("get_h", &cNumericDerivative::Geth,
            "Returns the step size h used for numerical differentiation.")

        // Expose public member variables directly
        .def_readwrite("h1", &cNumericDerivative::mh1,
            "Array of step sizes for first-order derivatives (e.g., one h per parameter).")
        .def_readwrite("h2", &cNumericDerivative::mh2,
            "Matrix of step sizes for second-order derivatives (e.g., one h per param pair).")
        .def_readwrite("value_for_grad", &cNumericDerivative::mValueForGrad,
            "RegArchValue object(s) used in gradient computation.")
        .def_readwrite("value_for_hess", &cNumericDerivative::mValueForHess,
            "RegArchValue object(s) used in Hessian computation.")
        .def_readwrite("log_dens_for_grad", &cNumericDerivative::mLogDensForGrad,
            "Log density values stored for gradient computations.")
        .def_readwrite("log_dens_for_hess", &cNumericDerivative::mLogDensForHess,
            "Log density values stored for Hessian computations.")
        .def_readwrite("grad_diff_for_grad", &cNumericDerivative::mGradDiffForGrad,
            "Gradient difference values used in gradient computation. (internal usage)")
        ;
}
