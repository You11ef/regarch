

#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

void export_cRegArchGradient()
{
    class_<cRegArchGradient>("cRegArchGradient",
        "Data structure to store histories of gradients for empirical residuals, "
        "conditional means and variances as stacks.",
        init<optional<uint, uint, uint, uint> >(
            (boost::python::arg("n_past") = 0, boost::python::arg("n_mean_param") = 0, boost::python::arg("n_var_param") = 0, boost::python::arg("n_distr_param") = 0),
            "Initialize a RegArch gradient structure with specific dimensions."
        )
    )
        .def("delete", &cRegArchGradient::Delete,
            "Delete and free resources.")
        .def("realloc",
            static_cast<void (cRegArchGradient::*)(uint, uint, uint, uint)>(&cRegArchGradient::ReAlloc),
            (boost::python::arg("n_past"), boost::python::arg("n_mean_param"), boost::python::arg("n_var_param"), boost::python::arg("n_distr_param")),
            "Reallocate memory for the gradient structures.")
        .def("reinitialize", &cRegArchGradient::ReInitialize,
            "Reset gradient values to zero or default values.")
        .def("get_n_past", &cRegArchGradient::GetNPast,
            "Return the number of past lags.")
        .def("get_n_param", &cRegArchGradient::GetNParam,
            "Return the total number of parameters.")
        .def("get_n_mean_param", &cRegArchGradient::GetNMeanParam,
            "Return the number of mean parameters.")
        .def("get_n_var_param", &cRegArchGradient::GetNVarParam,
            "Return the number of variance parameters.")
        .def("get_n_distr_param", &cRegArchGradient::GetNDistrParameter,
            "Return the number of distribution parameters.")
        .def("update", &cRegArchGradient::Update,
            "Update gradient at current date and shift stored values.")

        // Expose public member vectors
        .def_readwrite("current_grad_var", &cRegArchGradient::mCurrentGradVar,
            "Current gradient of conditional variance.")
        .def_readwrite("current_grad_sigma", &cRegArchGradient::mCurrentGradSigma,
            "Current gradient of conditional standard deviation.")
        .def_readwrite("current_grad_mu", &cRegArchGradient::mCurrentGradMu,
            "Current gradient of conditional mean.")
        .def_readwrite("current_grad_log_dens", &cRegArchGradient::mCurrentGradLogDens,
            "Current gradient of conditional log density.")
        .def_readwrite("current_grad_eps", &cRegArchGradient::mCurrentGradEps,
            "Current gradient of conditional standardized residuals.")
        .def_readwrite("current_diff_log_density", &cRegArchGradient::mCurrentDiffLogDensity,
            "Current derivative of the log density with respect to x.")
        ;
}