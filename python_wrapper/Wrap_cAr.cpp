#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// 1) Include your helper header that provides py_list_or_tuple_to_cDVector(...)
#include "PythonConversion.h"

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
    extract<double>   value_extract(value_or_vector);

    // Check if second argument is a string or an index
    extract<std::string> name_extract(index_or_name);
    extract<uint>        index_extract(index_or_name);

    if (vector_extract.check())
    {
        // The user directly provided a cDVector
        cDVector vector = vector_extract();

        if (name_extract.check()) {
            // Set(cDVector, string)
            std::string name = name_extract();
            self.Set(vector, name);
        }
        else if (index_extract.check()) {
            // Set(cDVector, uint)
            uint num = 0;
            if (!num_param.is_none())
                num = extract<uint>(num_param);
            self.Set(vector, num);
        }
        else {
            throw std::runtime_error("Invalid arguments for set method");
        }
    }
    else if (value_extract.check())
    {
        // The user provided a scalar (double)
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
    else
    {
        // Fallback for a Python list or tuple
        if (PyList_Check(value_or_vector.ptr()) || PyTuple_Check(value_or_vector.ptr()))
        {
            // Convert it to a cDVector
            cDVector vector = py_list_or_tuple_to_cDVector(value_or_vector);

            if (name_extract.check()) {
                self.Set(vector, name_extract());
            }
            else if (index_extract.check()) {
                uint num = 0;
                if (!num_param.is_none())
                    num = extract<uint>(num_param);
                self.Set(vector, num);
            }
            else {
                throw std::runtime_error("Invalid arguments for set method (second argument must be string or uint)");
            }
        }
        else {
            throw std::runtime_error("First argument must be a double, cDVector, or a Python list/tuple of floats");
        }
    }
}

// Helper function for get
static object cArGet(cAr& self, const object& index_or_name, const object& num_param = object())
{
    // Check if first argument is a string or an index
    extract<std::string> name_extract(index_or_name);
    extract<uint>        index_extract(index_or_name);

    if (name_extract.check())
    {
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
    else if (index_extract.check())
    {
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
    extract<uint>     size_extract(size_or_vector);

    uint param = 0;
    if (!num_param.is_none())
        param = extract<uint>(num_param);

    if (vector_extract.check())
    {
        // ReAlloc(cDVector, uint)
        cDVector vector = vector_extract();
        self.ReAlloc(vector, param);
    }
    else if (size_extract.check())
    {
        // ReAlloc(uint, uint)
        uint size = size_extract();
        self.ReAlloc(size, param);
    }
    else
    {
        // Fallback for Python list or tuple
        if (PyList_Check(size_or_vector.ptr()) || PyTuple_Check(size_or_vector.ptr()))
        {
            cDVector vector = py_list_or_tuple_to_cDVector(size_or_vector);
            self.ReAlloc(vector, param);
        }
        else {
            throw std::runtime_error("First argument must be a uint, cDVector, or a Python list/tuple of floats");
        }
    }
}

void export_cAr()
{
    class_<cAr, bases<cAbstCondMean> >("cAr",
        "Autoregressive (AR) conditional mean model.\n\n"
        "The AR(p) model represents the conditional mean as a linear function of past observations\n",
        init<optional<uint> >(
            (boost::python::arg("n_ar") = 0),
            "Create an AR(p) model with p=n_ar.\n\n"
            "Parameters:\n"
            "    n_ar (int, optional): The order of the AR process. Default is 0."
        )
    )
        // Additional constructors
        .def(init<const cDVector&>(
            boost::python::arg("ar_coeffs"),
            "Create an AR model with specified AR coefficients.\n\n"
            "Parameters:\n"
            "    ar_coeffs (cDVector): Vector of AR coefficients."
        ))
        .def(init<const cAr&>(
            boost::python::arg("other"),
            "Create a copy of an existing AR model.\n\n"
            "Parameters:\n"
            "    other (cAr): The AR model to copy."
        ))

        // Assignment operator
        .def("__assign__", &cAr::operator=,
            return_value_policy<reference_existing_object>(),
            "Assign another AR model to this one.")

        // Unified Python interfaces
        .def("set", &cArSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Set model parameters using various parameter formats.\n\n"
            "Examples:\n"
            "    set(value, index, param_num): Set a single parameter value by numerical index\n"
            "    set(value, name, index): Set a single parameter value by name\n"
            "    set(vector, param_num): Set multiple parameters from a vector by numerical index\n"
            "    set(vector, name): Set multiple parameters from a vector by name"
        )

        .def("get", &cArGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Get model parameters using various parameter formats.\n\n"
            "Examples:\n"
            "    get(index, param_num): Get a single parameter value by numerical index\n"
            "    get(param_num): Get all parameters as a vector by numerical index\n"
            "    get(name, index): Get a single parameter value by name\n"
            "    get(name): Get all parameters as a vector by name\n\n"
            "Returns:\n"
            "    A single parameter value (double) or a vector of parameters (cDVector)"
        )

        .def("realloc", &cArReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("param_num") = object()),
            "Reallocate memory for model parameters.\n\n"
            "Parameters:\n"
            "    size_or_vector: Either a size (uint) or a vector (cDVector)\n"
            "    param_num (int, optional): Parameter number (default is 0)."
        )

        // Basic housekeeping and legacy method names
        .def("delete", &cAr::Delete,
            "Free memory used by the AR model.")
        .def("print", &cArPrint,
            "Print the AR model parameters to standard output.")

        // SetDefaultInitPoint methods
        .def("set_default_init_point",
            static_cast<void (cAr::*)(double, double)>(&cAr::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Set default initial parameter values based on mean and variance.\n\n"
            "Parameters:\n"
            "    mean (double): Initial mean value\n"
            "    var (double): Initial variance value"
        )
        .def("set_default_init_point",
            static_cast<void (cAr::*)(cRegArchValue&)>(&cAr::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Set default initial parameter values based on RegArch values.\n\n"
            "Parameters:\n"
            "    value (cRegArchValue): RegArch values object"
        )

        // For backwards compatibility
        .def("SetValueIndex", static_cast<void (cAr::*)(double, uint, uint)>(&cAr::Set),
            (boost::python::arg("value"), boost::python::arg("index"), boost::python::arg("param_num")),
            "Set a parameter value by numerical index (legacy API).")
        .def("SetVectorIndex", static_cast<void (cAr::*)(const cDVector&, uint)>(&cAr::Set),
            (boost::python::arg("vector"), boost::python::arg("param_num")),
            "Set parameters from a vector by numerical index (legacy API).")
        .def("GetValueIndex", static_cast<double (cAr::*)(uint, uint)>(&cAr::Get),
            (boost::python::arg("index"), boost::python::arg("param_num")),
            "Get a parameter value by numerical index (legacy API).")
        .def("GetVectorIndex",
            static_cast<cDVector & (cAr::*)(uint)>(&cAr::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_num"),
            "Get parameters as a vector by numerical index (legacy API).")
        .def("SetValueName", static_cast<void (cAr::*)(double, std::string, uint)>(&cAr::Set),
            (boost::python::arg("value"), boost::python::arg("param_name"), boost::python::arg("index")),
            "Set a parameter value by name (legacy API).")
        .def("SetVectorName", static_cast<void (cAr::*)(const cDVector&, std::string)>(&cAr::Set),
            (boost::python::arg("vector"), boost::python::arg("param_name")),
            "Set parameters from a vector by name (legacy API).")
        .def("GetValueName", static_cast<double (cAr::*)(std::string, uint)>(&cAr::Get),
            (boost::python::arg("param_name"), boost::python::arg("index")),
            "Get a parameter value by name (legacy API).")
        .def("GetVectorName",
            static_cast<cDVector & (cAr::*)(std::string)>(&cAr::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Get parameters as a vector by name (legacy API).")
        .def("ReAllocSize", static_cast<void (cAr::*)(uint, uint)>(&cAr::ReAlloc),
            (boost::python::arg("size"), boost::python::arg("param_num")),
            "Reallocate memory for a parameter of specified size (legacy API).")
        .def("ReAllocVect", static_cast<void (cAr::*)(const cDVector&, uint)>(&cAr::ReAlloc),
            (boost::python::arg("vector"), boost::python::arg("param_num")),
            "Reallocate memory for a parameter based on vector's size (legacy API).")

        // Proxy parameter methods
        .def("realloc_proxy_mean_parameters", &cAr::ReAllocProxyMeanParameters,
            (boost::python::arg("old_n_param") = 0),
            "Reallocate proxy mean parameters.\n\n"
            "Parameters:\n"
            "    old_n_param (int, optional): Old number of parameters. Default is 0."
        )
        .def("update_proxy_mean_parameters", &cAr::UpdateProxyMeanParameters,
            "Update proxy mean parameters.")

        // Key cAr methods
        .def("compute_mean", &cAr::ComputeMean,
            (boost::python::arg("date"), boost::python::arg("data")),
            "Compute the conditional mean at the specified date.\n\n"
            "Parameters:\n"
            "    date (int): The date index\n"
            "    data (cRegArchValue): The data object\n\n"
            "Returns:\n"
            "    double: The computed conditional mean value"
        )
        .def("get_n_param", &cAr::GetNParam,
            "Get the number of parameters in the model.\n\n"
            "Returns:\n"
            "    int: Number of parameters"
        )
        .def("get_n_lags", &cAr::GetNLags,
            "Get the number of lags required for gradient computation.\n\n"
            "Returns:\n"
            "    int: Number of lags"
        )
        .def("compute_grad", &cAr::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("beg_index")),
            "Compute the gradient of the model at the specified date.\n\n"
            "Parameters:\n"
            "    date (int): The date index\n"
            "    data (cRegArchValue): The data object\n"
            "    grad_data (cRegArchGradient): The gradient object to store results\n"
            "    beg_index (int): The beginning index"
        )
        .def("compute_hess", &cAr::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("beg_index")),
            "Compute the Hessian of the model at the specified date.\n\n"
            "Parameters:\n"
            "    date (int): The date index\n"
            "    data (cRegArchValue): The data object\n"
            "    grad_data (cRegArchGradient): The gradient object\n"
            "    hess_data (cRegArchHessien): The Hessian object to store results\n"
            "    beg_index (int): The beginning index"
        )
        .def("compute_grad_and_hess", &cAr::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("beg_index")),
            "Compute both gradient and Hessian in one operation.\n\n"
            "Parameters:\n"
            "    date (int): The date index\n"
            "    data (cRegArchValue): The data object\n"
            "    grad_data (cRegArchGradient): The gradient object to store results\n"
            "    hess_data (cRegArchHessien): The Hessian object to store results\n"
            "    beg_index (int): The beginning index"
        )
        .def("reg_arch_param_to_vector", &cAr::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index") = 0),
            "Convert model parameters to a vector representation.\n\n"
            "Parameters:\n"
            "    dest_vect (cDVector): The destination vector\n"
            "    index (int, optional): The starting index. Default is 0."
        )
        .def("vector_to_reg_arch_param", &cAr::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Convert a vector representation back to model parameters.\n\n"
            "Parameters:\n"
            "    src_vect (cDVector): The source vector\n"
            "    index (int, optional): The starting index. Default is 0."
        )

        // Parameter name getters
        .def("get_param_name_char",
            static_cast<void (cAr::*)(uint, char**)>(&cAr::GetParamName),
            boost::python::arg("index"),
            "Get parameter name as a C-style string array.\n\n"
            "Parameters:\n"
            "    index (int): The parameter index\n"
            "Note: This method is primarily for internal use. Use get_param_name_string instead."
        )
        .def("get_param_name_string",
            static_cast<void (cAr::*)(uint, std::string[])>(&cAr::GetParamName),
            boost::python::arg("index"),
            "Get parameter name as a C++ string array.\n\n"
            "Parameters:\n"
            "    index (int): The parameter index"
        )
        ;
}
