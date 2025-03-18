#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

// Helper function for Print (to call Print(std::cout))
static void cArchPrint(cArch& self) {
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cArch() {
    class_<cArch, bases<cAbstCondVar> >("cArch",
        "Pure ARCH(p) conditional variance model.\n\n"
        "The ARCH(p) model is defined as: h_t = omega + alpha_1*e_{t-1}^2 + ... + alpha_p*e_{t-p}^2",
        init<optional<uint> >(
            (boost::python::arg("n_arch") = 0),
            "cArch(uint n_arch=0)\n\n"
            "Creates an ARCH(p) model with p=n_arch."
        )
    )
        // Copy constructor
        .def(init<const cArch&>(boost::python::arg("other")))

        // Basic housekeeping
        .def("delete", &cArch::Delete,
            "Releases memory used by the model.")
        .def("print", &cArchPrint,
            "Prints model parameters to standard output.")

        // SetDefaultInitPoint methods - Pythonic naming
        .def("set_default_init_point",
            static_cast<void (cArch::*)(double, double)>(&cArch::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Sets default initial parameter values based on mean and variance.")
        .def("set_default_init_point",
            static_cast<void (cArch::*)(cRegArchValue&)>(&cArch::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Sets default initial parameter values based on RegArch values.")

        // ReAlloc methods - Pythonic naming
        .def("realloc",
            static_cast<void (cArch::*)(uint, uint)>(&cArch::ReAlloc),
            (boost::python::arg("size"), boost::python::arg("param_num") = 0),
            "Reallocates memory for a parameter of the specified size.")
        .def("realloc_vect",
            static_cast<void (cArch::*)(const cDVector&, uint)>(&cArch::ReAlloc),
            (boost::python::arg("vect_param"), boost::python::arg("param_num") = 0),
            "Reallocates memory for a parameter based on the vector's size.")

        // Set methods - Pythonic naming
        .def("set_value_index",
            static_cast<void (cArch::*)(double, uint, uint)>(&cArch::Set),
            (boost::python::arg("value"), boost::python::arg("index"), boost::python::arg("param_num")),
            "Sets a parameter value by numerical index.")
        .def("set_vector_index",
            static_cast<void (cArch::*)(const cDVector&, uint)>(&cArch::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_num")),
            "Sets parameters from a vector by numerical index.")

        // Get methods - Pythonic naming
        .def("get_value_index",
            static_cast<double (cArch::*)(uint, uint)>(&cArch::Get),
            (boost::python::arg("index"), boost::python::arg("param_num")),
            "Gets a parameter value by numerical index.")
        .def("get_vector_index",
            static_cast<cDVector & (cArch::*)(uint)>(&cArch::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_num"),
            "Gets parameters as a vector by numerical index.")

        // Set methods with param names - Pythonic naming
        .def("set_value_name",
            static_cast<void (cArch::*)(double, std::string, uint)>(&cArch::Set),
            (boost::python::arg("value"), boost::python::arg("param_name"), boost::python::arg("index")),
            "Sets a parameter value by name.")
        .def("set_vector_name",
            static_cast<void (cArch::*)(const cDVector&, std::string)>(&cArch::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_name")),
            "Sets parameters from a vector by name.")

        // Get methods with param names - Pythonic naming
        .def("get_value_name",
            static_cast<double (cArch::*)(std::string, uint)>(&cArch::Get),
            (boost::python::arg("param_name"), boost::python::arg("index")),
            "Gets a parameter value by name.")
        .def("get_vector_name",
            static_cast<cDVector & (cArch::*)(std::string)>(&cArch::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Gets parameters as a vector by name.")

        // For backwards compatibility
        .def("SetDefaultInitPoint", static_cast<void (cArch::*)(double, double)>(&cArch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue", static_cast<void (cArch::*)(cRegArchValue&)>(&cArch::SetDefaultInitPoint))
        .def("ReAlloc", static_cast<void (cArch::*)(uint, uint)>(&cArch::ReAlloc))
        .def("ReAllocVect", static_cast<void (cArch::*)(const cDVector&, uint)>(&cArch::ReAlloc))
        .def("SetValueIndex", static_cast<void (cArch::*)(double, uint, uint)>(&cArch::Set))
        .def("SetVectorIndex", static_cast<void (cArch::*)(const cDVector&, uint)>(&cArch::Set))
        .def("GetValueIndex", static_cast<double (cArch::*)(uint, uint)>(&cArch::Get))
        .def("GetVectorIndex", static_cast<cDVector & (cArch::*)(uint)>(&cArch::Get), return_value_policy<reference_existing_object>())
        .def("SetValueName", static_cast<void (cArch::*)(double, std::string, uint)>(&cArch::Set))
        .def("SetVectorName", static_cast<void (cArch::*)(const cDVector&, std::string)>(&cArch::Set))
        .def("GetValueName", static_cast<double (cArch::*)(std::string, uint)>(&cArch::Get))
        .def("GetVectorName", static_cast<cDVector & (cArch::*)(std::string)>(&cArch::Get), return_value_policy<reference_existing_object>())

        // Proxy parameter methods
        .def("realloc_proxy_var_parameters", &cArch::ReAllocProxyVarParameters,
            (boost::python::arg("old_n_param") = 0),
            "Reallocates proxy variance parameters.")
        .def("update_proxy_var_parameters", &cArch::UpdateProxyVarParameters,
            "Updates proxy variance parameters.")

        // Core functionality - Pythonic naming
        .def("compute_var", &cArch::ComputeVar,
            (boost::python::arg("date"), boost::python::arg("data")),
            "Computes conditional variance at the specified date given the data.")
        .def("get_n_param", &cArch::GetNParam,
            "Returns the number of parameters in the model.")
        .def("get_n_lags", &cArch::GetNLags,
            "Returns the number of lags required for gradient computation.")
        .def("compute_grad", &cArch::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("residuals") = object()),
            "Computes the gradient of the model at the specified date.")
        .def("compute_hess", &cArch::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("residuals") = object()),
            "Computes the Hessian of the model at the specified date.")
        .def("compute_grad_and_hess", &cArch::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("residuals") = object()),
            "Computes both gradient and Hessian in one operation.")
        .def("reg_arch_param_to_vector", &cArch::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index") = 0),
            "Converts model parameters to a vector representation.")
        .def("vector_to_reg_arch_param", &cArch::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Converts a vector representation back to model parameters.")

        // Core functionality - Original naming
        .def("ComputeVar", &cArch::ComputeVar)
        .def("GetNParam", &cArch::GetNParam)
        .def("GetNLags", &cArch::GetNLags)
        .def("ComputeGrad", &cArch::ComputeGrad)
        .def("RegArchParamToVector", &cArch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cArch::VectorToRegArchParam)
        .def("ComputeHess", &cArch::ComputeHess)
        .def("ComputeGradAndHess", &cArch::ComputeGradAndHess)

        // Parameter name getters
        .def("get_param_name_char",
            static_cast<void (cArch::*)(uint, char**)>(&cArch::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C-style string.")
        .def("get_param_name_string",
            static_cast<void (cArch::*)(uint, std::string[])>(&cArch::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C++ string.")
        .def("GetParamNameChar", static_cast<void (cArch::*)(uint, char**)>(&cArch::GetParamName))
        .def("GetParamNameString", static_cast<void (cArch::*)(uint, std::string[])>(&cArch::GetParamName))
        ;
}