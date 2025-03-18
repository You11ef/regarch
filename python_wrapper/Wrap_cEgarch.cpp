#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>


using namespace boost::python;
using namespace RegArchLib;

static void Egarch_Print(cEgarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cEgarch()
{
    class_<cEgarch, bases<cAbstCondVar> >("cEgarch",
        "EGARCH conditional variance model.\n\n"
        "The EGARCH (Exponential GARCH) model is defined as:\n"
        "ln h(t) = Constant + Sum_{i=1 to p} Arch(i)*(Teta*Eps(t-i) + Gamma*|Eps(t-i)-E|Eps(t-i)|) + Sum_{j=1 to q} Garch(j)* ln h(t-j)\n\n"
        "where p is the ARCH order and q is the GARCH order. This model captures asymmetric effects in volatility.",
        init< optional<int, int> >(
            (boost::python::arg("n_arch") = 0, boost::python::arg("n_garch") = 0),
            "cEgarch(int n_arch=0, int n_garch=0)\n\n"
            "Creates an EGARCH(p,q) model with p=n_arch and q=n_garch."
        )
    )
        // Additional constructor with residuals
        .def(init<cAbstResiduals*, optional<int, int> >(
            (boost::python::arg("residuals"), boost::python::arg("n_arch") = 0, boost::python::arg("n_garch") = 0),
            "cEgarch(cAbstResiduals* residuals, int n_arch=0, int n_garch=0)\n\n"
            "Creates an EGARCH(p,q) model with specified residuals distribution."
        ))
        // Copy constructor
        .def(init<const cEgarch&>(
            boost::python::arg("other"),
            "cEgarch(cEgarch other)\n\n"
            "Copy constructor - creates a copy of another EGARCH model."
        ))

        // Pythonic naming
        .def("delete", &cEgarch::Delete,
            "Releases memory used by the model.")
        .def("print", &Egarch_Print,
            "Prints model parameters to standard output.")

        // Initialization - Pythonic naming
        .def("set_default_init_point",
            static_cast<void (cEgarch::*)(double, double)>(&cEgarch::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Sets default initial parameter values based on mean and variance.")
        .def("set_default_init_point_value",
            static_cast<void (cEgarch::*)(cRegArchValue&)>(&cEgarch::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Sets default initial parameter values based on RegArch values.")

        // ReAlloc methods - Pythonic naming
        .def("realloc",
            static_cast<void (cEgarch::*)(uint, uint)>(&cEgarch::ReAlloc),
            (boost::python::arg("size"), boost::python::arg("param_num") = 0),
            "Reallocates memory for model parameters.")
        .def("realloc_vect",
            static_cast<void (cEgarch::*)(const cDVector&, uint)>(&cEgarch::ReAlloc),
            (boost::python::arg("vect_param"), boost::python::arg("param_num") = 0),
            "Reallocates memory for model parameters based on vector size.")

        // Set methods - Pythonic naming
        .def("set_value_index",
            static_cast<void (cEgarch::*)(double, uint, uint)>(&cEgarch::Set),
            (boost::python::arg("value"), boost::python::arg("index"), boost::python::arg("param_num")),
            "Sets a parameter value by numerical index.")
        .def("set_vector_index",
            static_cast<void (cEgarch::*)(const cDVector&, uint)>(&cEgarch::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_num")),
            "Sets parameters from a vector by numerical index.")

        // Get methods - Pythonic naming
        .def("get_value_index",
            static_cast<double (cEgarch::*)(uint, uint)>(&cEgarch::Get),
            (boost::python::arg("index"), boost::python::arg("param_num")),
            "Gets a parameter value by numerical index.")
        .def("get_vector_index",
            static_cast<cDVector & (cEgarch::*)(uint)>(&cEgarch::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_num"),
            "Gets parameters as a vector by numerical index.")

        // Named parameters - Pythonic naming
        .def("set_value_name",
            static_cast<void (cEgarch::*)(double, std::string, uint)>(&cEgarch::Set),
            (boost::python::arg("value"), boost::python::arg("param_name"), boost::python::arg("index")),
            "Sets a parameter value by name.")
        .def("set_vector_name",
            static_cast<void (cEgarch::*)(const cDVector&, std::string)>(&cEgarch::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_name")),
            "Sets parameters from a vector by name.")
        .def("get_value_name",
            static_cast<double (cEgarch::*)(std::string, uint)>(&cEgarch::Get),
            (boost::python::arg("param_name"), boost::python::arg("index") = 0),
            "Gets a parameter value by name.")
        .def("get_vector_name",
            static_cast<cDVector & (cEgarch::*)(std::string)>(&cEgarch::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Gets parameters as a vector by name.")

        // Proxy parameter methods - Pythonic naming
        .def("realloc_proxy_var_parameters", &cEgarch::ReAllocProxyVarParameters,
            (boost::python::arg("old_n_param") = 0),
            "Reallocates proxy variance parameters.")
        .def("update_proxy_var_parameters", &cEgarch::UpdateProxyVarParameters,
            "Updates proxy variance parameters.")

        // EGARCH specific functions - Pythonic naming
        .def("set_esp_abs_eps", &cEgarch::SetEspAbsEps,
            boost::python::arg("esp_abs_eps"),
            "Sets the expected absolute value of epsilon.")
        .def("sync_size_param", &cEgarch::SyncSizeParam,
            boost::python::arg("param_num"),
            "Synchronizes parameter sizes.")

        // Core functionality - Pythonic naming
        .def("compute_var", &cEgarch::ComputeVar,
            (boost::python::arg("date"), boost::python::arg("data")),
            "Computes the conditional variance at the specified date given the data.")
        .def("get_n_param", &cEgarch::GetNParam,
            "Returns the total number of parameters in the model.")
        .def("get_n_lags", &cEgarch::GetNLags,
            "Returns the number of lags required for computation.")
        .def("compute_grad", &cEgarch::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("residuals")),
            "Computes the gradient of the model at the specified date.")
        .def("compute_hess", &cEgarch::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("residuals")),
            "Computes the Hessian of the model at the specified date.")
        .def("compute_grad_and_hess", &cEgarch::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("residuals")),
            "Computes both gradient and Hessian in one operation.")
        .def("reg_arch_param_to_vector", &cEgarch::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index") = 0),
            "Converts model parameters to a vector representation.")
        .def("vector_to_reg_arch_param", &cEgarch::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Converts a vector representation back to model parameters.")
        .def("get_param_name_char",
            static_cast<void (cEgarch::*)(uint, char**)>(&cEgarch::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C-style string.")
        .def("get_param_name_string",
            static_cast<void (cEgarch::*)(uint, std::string[])>(&cEgarch::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C++ string.")

        // Original C++ naming for backwards compatibility
        .def("Delete", &cEgarch::Delete)
        .def("Print", &Egarch_Print)
        .def("SetDefaultInitPoint", static_cast<void (cEgarch::*)(double, double)>(&cEgarch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue", static_cast<void (cEgarch::*)(cRegArchValue&)>(&cEgarch::SetDefaultInitPoint))
        .def("ReAlloc", static_cast<void (cEgarch::*)(uint, uint)>(&cEgarch::ReAlloc))
        .def("ReAllocVect", static_cast<void (cEgarch::*)(const cDVector&, uint)>(&cEgarch::ReAlloc))
        .def("SetValueIndex", static_cast<void (cEgarch::*)(double, uint, uint)>(&cEgarch::Set))
        .def("SetVectorIndex", static_cast<void (cEgarch::*)(const cDVector&, uint)>(&cEgarch::Set))
        .def("GetValueIndex", static_cast<double (cEgarch::*)(uint, uint)>(&cEgarch::Get))
        .def("GetVectorIndex", static_cast<cDVector & (cEgarch::*)(uint)>(&cEgarch::Get),
            return_value_policy<reference_existing_object>())
        .def("SetValueName", static_cast<void (cEgarch::*)(double, std::string, uint)>(&cEgarch::Set))
        .def("SetVectorName", static_cast<void (cEgarch::*)(const cDVector&, std::string)>(&cEgarch::Set))
        .def("GetValueName", static_cast<double (cEgarch::*)(std::string, uint)>(&cEgarch::Get))
        .def("GetVectorName", static_cast<cDVector & (cEgarch::*)(std::string)>(&cEgarch::Get),
            return_value_policy<reference_existing_object>())
        .def("SyncSizeParam", &cEgarch::SyncSizeParam)
        .def("ReAllocProxyVarParameters", &cEgarch::ReAllocProxyVarParameters)
        .def("UpdateProxyVarParameters", &cEgarch::UpdateProxyVarParameters)
        .def("ComputeVar", &cEgarch::ComputeVar)
        .def("GetNParam", &cEgarch::GetNParam)
        .def("GetNLags", &cEgarch::GetNLags)
        .def("RegArchParamToVector", &cEgarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cEgarch::VectorToRegArchParam)
        .def("ComputeGrad", &cEgarch::ComputeGrad)
        .def("ComputeHess", &cEgarch::ComputeHess)
        .def("ComputeGradAndHess", &cEgarch::ComputeGradAndHess)
        .def("SetEspAbsEps", &cEgarch::SetEspAbsEps)
        .def("GetParamNameChar", static_cast<void (cEgarch::*)(uint, char**)>(&cEgarch::GetParamName))
        .def("GetParamNameString", static_cast<void (cEgarch::*)(uint, std::string[])>(&cEgarch::GetParamName))

        // Assignment operator
        .def("__assign__", &cEgarch::operator=, return_internal_reference<>(),
            "Assignment operator - copies another EGARCH model.")
        ;
}