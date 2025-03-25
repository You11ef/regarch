// not used


#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

void export_cRegArchHessien()
{
    class_<cRegArchHessien>("cRegArchHessien",
        "Data structure to store the Hessian matrices for empirical residuals, "
        "conditional means and variances.",
        init<optional<uint, uint, uint, uint> >(
            (boost::python::arg("n_past") = 0, boost::python::arg("n_mean_param") = 0, boost::python::arg("n_var_param") = 0, boost::python::arg("n_distr_param") = 0),
            "Initialize a RegArch Hessian structure with specific dimensions."
        )
    )
        .def("delete", &cRegArchHessien::Delete,
            "Delete and free resources.")
        .def("realloc",
            static_cast<void (cRegArchHessien::*)(uint, uint, uint, uint)>(&cRegArchHessien::ReAlloc),
            (boost::python::arg("n_past"), boost::python::arg("n_mean_param"), boost::python::arg("n_var_param"), boost::python::arg("n_distr_param")),
            "Reallocate memory for the Hessian structures.")
        .def("get_n_past", &cRegArchHessien::GetNPast,
            "Return the number of past lags.")
        .def("get_n_param", &cRegArchHessien::GetNParam,
            "Return the total number of parameters.")
        .def("get_n_mean_param", &cRegArchHessien::GetNMeanParam,
            "Return the number of mean parameters.")
        .def("get_n_var_param", &cRegArchHessien::GetNVarParam,
            "Return the number of variance parameters.")
        .def("get_n_distr_param", &cRegArchHessien::GetNDistrParameter,
            "Return the number of distribution parameters.")
        .def("compute_hess_sigmat_from_hess_vart", &cRegArchHessien::ComputeHessSigmatFromHessVart,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data")),
            "Compute Hessian of sigma from Hessian of variance.")
        .def("update", &cRegArchHessien::Update,
            "Update Hessian at current date and shift stored values.")

        // Expose public member matrices
        .def_readwrite("current_hess_var", &cRegArchHessien::mCurrentHessVar,
            "Current Hessian of conditional variance.")
        .def_readwrite("current_hess_sigma", &cRegArchHessien::mCurrentHessSigma,
            "Current Hessian of conditional standard deviation.")
        .def_readwrite("current_hess_mu", &cRegArchHessien::mCurrentHessMu,
            "Current Hessian of conditional mean.")
        .def_readwrite("current_hess_dens", &cRegArchHessien::mCurrentHessDens,
            "Current Hessian of conditional log density.")
        .def_readwrite("current_hess_eps", &cRegArchHessien::mCurrentHessEps,
            "Current Hessian of conditional standardized residuals.")
        .def_readwrite("current_grad_diff_log_density", &cRegArchHessien::mCurrentGradDiffLogDensity,
            "Current gradient of the derivative of log density with respect to x.")
        ;
}