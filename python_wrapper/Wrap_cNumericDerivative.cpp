#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

void export_cNumericDerivative()
{
    class_<cNumericDerivative>("cNumericDerivative",
        "Numerical derivative computation helper for RegArch models.\n"
        "Used to compute gradients and Hessians numerically when analytical formulas are not available.",
        init<optional<uint, uint, double, uint> >(
            (boost::python::arg("n_param") = 0,
                boost::python::arg("n_distr_param") = 0,
                boost::python::arg("h") = 1e-2,
                boost::python::arg("n_obs") = 0),
            "Initialize a numerical derivative helper with specified parameters."
        )
    )
        .def(init<uint, uint, double, cDVector&, cDMatrix*, cDMatrix*>(
            (boost::python::arg("n_param"),
                boost::python::arg("n_distr_param"),
                boost::python::arg("h"),
                boost::python::arg("yt"),
                boost::python::arg("xt") = static_cast<cDMatrix*>(0),
                boost::python::arg("xvt") = static_cast<cDMatrix*>(0)),
            "Initialize a numerical derivative helper with data vectors and matrices."
        ))

        // Access methods
        .def("get_n_param", &cNumericDerivative::GetNParam,
            "Returns the number of parameters.")
        .def("get_n_distr_param", &cNumericDerivative::GetNDistrParam,
            "Returns the number of distribution parameters.")
        .def("get_h", &cNumericDerivative::Geth,
            "Returns the step size h used for numerical differentiation.")

        // Expose public member variables
        .def_readwrite("h1", &cNumericDerivative::mh1,
            "Array of step sizes for first-order derivatives.")
        .def_readwrite("h2", &cNumericDerivative::mh2,
            "Matrix of step sizes for second-order derivatives.")
        .def_readwrite("value_for_grad", &cNumericDerivative::mValueForGrad,
            "RegArch value objects used in gradient computation.")
        .def_readwrite("value_for_hess", &cNumericDerivative::mValueForHess,
            "RegArch value objects used in Hessian computation.")
        .def_readwrite("log_dens_for_grad", &cNumericDerivative::mLogDensForGrad,
            "Log density values for gradient computation.")
        .def_readwrite("log_dens_for_hess", &cNumericDerivative::mLogDensForHess,
            "Log density values for Hessian computation.")
        .def_readwrite("grad_diff_for_grad", &cNumericDerivative::mGradDiffForGrad,
            "Gradient difference values for gradient computation.")
        ;
}