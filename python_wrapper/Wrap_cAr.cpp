#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

// Helper function for printing
static void cArPrint(cAr& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// Helper function to allow Python-style overloaded methods
static void cArSet(cAr& self, const object& value_or_vector, const object& index_or_name, const object& num_param = object())
{
    // Check if first argument is a vector or a scalar
    extract<cDVector> vector_extract(value_or_vector);
    extract<double> value_extract(value_or_vector);

    // Check if second argument is a string or an index
    extract<std::string> name_extract(index_or_name);
    extract<uint> index_extract(index_or_name);

    if (vector_extract.check()) {
        cDVector vector = vector_extract();

        if (name_extract.check()) {
            // Set(cDVector, string)
            std::string name = name_extract();
            self.Set(vector, name);
        }
        else if (index_extract.check()) {
            // Set(cDVector, uint)
            uint num = extract<uint>(num_param);
            self.Set(vector, num);
        }
        else {
            throw std::runtime_error("Invalid arguments for set method");
        }
    }
    else if (value_extract.check()) {
        double value = value_extract();

        if (name_extract.check()) {
            // Set(double, string, uint)
            std::string name = name_extract();
            uint index = 0;
            if (!num_param.is_none())
                index = extract<uint>(num_param);
            self.Set(value, name, index);
        }
        else if (index_extract.check()) {
            // Set(double, uint, uint)
            uint index = index_extract();
            uint num = 0;
            if (!num_param.is_none())
                num = extract<uint>(num_param);
            self.Set(value, index, num);
        }
        else {
            throw std::runtime_error("Invalid arguments for set method");
        }
    }
    else {
        throw std::runtime_error("First argument must be a double or cDVector");
    }
}

// Helper function for get
static object cArGet(cAr& self, const object& index_or_name, const object& num_param = object())
{
    // Check if first argument is a string or an index
    extract<std::string> name_extract(index_or_name);
    extract<uint> index_extract(index_or_name);

    if (name_extract.check()) {
        std::string name = name_extract();

        if (num_param.is_none()) {
            // Get(string) -> cDVector&
            return object(boost::ref(self.Get(name)));
        }
        else {
            // Get(string, uint) -> double
            uint index = extract<uint>(num_param);
            return object(self.Get(name, index));
        }
    }
    else if (index_extract.check()) {
        uint index = index_extract();

        if (num_param.is_none()) {
            // Get(uint) -> cDVector&
            return object(boost::ref(self.Get(index)));
        }
        else {
            // Get(uint, uint) -> double
            uint num = extract<uint>(num_param);
            return object(self.Get(index, num));
        }
    }
    else {
        throw std::runtime_error("First argument must be a string or uint");
    }
}

// Helper for realloc
static void cArReAlloc(cAr& self, const object& size_or_vector, const object& num_param = object())
{
    // Check if first argument is a vector or a scalar
    extract<cDVector> vector_extract(size_or_vector);
    extract<uint> size_extract(size_or_vector);

    uint param = 0;
    if (!num_param.is_none())
        param = extract<uint>(num_param);

    if (vector_extract.check()) {
        // ReAlloc(cDVector, uint)
        cDVector vector = vector_extract();
        self.ReAlloc(vector, param);
    }
    else if (size_extract.check()) {
        // ReAlloc(uint, uint)
        uint size = size_extract();
        self.ReAlloc(size, param);
    }
    else {
        throw std::runtime_error("First argument must be a uint or cDVector");
    }
}

void export_cAr()
{
    class_<cAr, bases<cAbstCondMean> >("cAr",
        "AR(p) conditional mean model.\n\n"
        "This class implements an autoregressive (AR) model for the conditional mean.\n"
        "The AR(p) model is defined as: y_t = c + phi_1*y_{t-1} + ... + phi_p*y_{t-p} + e_t",
        init<optional<uint> >(
            (boost::python::arg("n_ar") = 0),
            "cAr(uint n_ar=0)\n\n"
            "Creates an AR(p) model with p=n_ar."
        )
    )
        // Additional constructors
        .def(init<const cDVector&>(
            boost::python::arg("ar_coeffs"),
            "cAr(cDVector ar_coeffs)\n\n"
            "Creates an AR model with coefficients directly specified by the vector."
        ))
        .def(init<const cAr&>(
            boost::python::arg("other"),
            "cAr(cAr other)\n\n"
            "Copy constructor - creates a copy of another AR model."
        ))

        // Assignment operator
        .def("__assign__", &cAr::operator=,
            return_value_policy<reference_existing_object>(),
            "Assigns another AR model to this one.")

        // Unified Python interfaces
        .def("set", &cArSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Sets model parameters.\n"
            "Can be called as:\n"
            "  set(value, index, [param_num])\n"
            "  set(value, name, [index])\n"
            "  set(vector, param_num)\n"
            "  set(vector, name)")

        .def("get", &cArGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Gets model parameters.\n"
            "Can be called as:\n"
            "  get(index, param_num) -> double\n"
            "  get(param_num) -> cDVector\n"
            "  get(name, index) -> double\n"
            "  get(name) -> cDVector")

        .def("realloc", &cArReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("param_num") = object()),
            "Reallocates memory for model parameters.\n"
            "Can take either a size (uint) or a vector (cDVector).")

        // Basic housekeeping and legacy method names
        .def("delete", &cAr::Delete,
            "Releases memory used by the model.")
        .def("print", &cArPrint,
            "Prints model parameters to standard output.")

        // SetDefaultInitPoint methods
        .def("set_default_init_point",
            static_cast<void (cAr::*)(double, double)>(&cAr::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Sets default initial parameter values based on mean and variance.")
        .def("set_default_init_point",
            static_cast<void (cAr::*)(cRegArchValue&)>(&cAr::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Sets default initial parameter values based on RegArch values.")

        // For backwards compatibility
        .def("SetValueIndex", static_cast<void (cAr::*)(double, uint, uint)>(&cAr::Set))
        .def("SetVectorIndex", static_cast<void (cAr::*)(const cDVector&, uint)>(&cAr::Set))
        .def("GetValueIndex", static_cast<double (cAr::*)(uint, uint)>(&cAr::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cAr::*)(uint)>(&cAr::Get),
            return_value_policy<reference_existing_object>())
        .def("SetValueName", static_cast<void (cAr::*)(double, std::string, uint)>(&cAr::Set))
        .def("SetVectorName", static_cast<void (cAr::*)(const cDVector&, std::string)>(&cAr::Set))
        .def("GetValueName", static_cast<double (cAr::*)(std::string, uint)>(&cAr::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cAr::*)(std::string)>(&cAr::Get),
            return_value_policy<reference_existing_object>())
        .def("ReAllocSize", static_cast<void (cAr::*)(uint, uint)>(&cAr::ReAlloc))
        .def("ReAllocVect", static_cast<void (cAr::*)(const cDVector&, uint)>(&cAr::ReAlloc))

        // Proxy parameter methods
        .def("realloc_proxy_mean_parameters", &cAr::ReAllocProxyMeanParameters,
            (boost::python::arg("old_n_param") = 0),
            "Reallocates proxy mean parameters.")
        .def("update_proxy_mean_parameters", &cAr::UpdateProxyMeanParameters,
            "Updates proxy mean parameters.")

        // Key cAr methods
        .def("compute_mean", &cAr::ComputeMean,
            (boost::python::arg("date"), boost::python::arg("data")),
            "Computes the conditional mean at the specified date given the data.")
        .def("get_n_param", &cAr::GetNParam,
            "Returns the number of parameters in the model.")
        .def("get_n_lags", &cAr::GetNLags,
            "Returns the number of lags required for gradient computation.")
        .def("compute_grad", &cAr::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("beg_index")),
            "Computes the gradient of the model at the specified date.")
        .def("compute_hess", &cAr::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("beg_index")),
            "Computes the Hessian of the model at the specified date.")
        .def("compute_grad_and_hess", &cAr::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("beg_index")),
            "Computes both gradient and Hessian in one operation.")
        .def("reg_arch_param_to_vector", &cAr::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index") = 0),
            "Converts model parameters to a vector representation.")
        .def("vector_to_reg_arch_param", &cAr::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Converts a vector representation back to model parameters.")

        // Parameter name getters
        .def("get_param_name_char",
            static_cast<void (cAr::*)(uint, char**)>(&cAr::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C-style string.")
        .def("get_param_name_string",
            static_cast<void (cAr::*)(uint, std::string[])>(&cAr::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C++ string.")
        ;
}