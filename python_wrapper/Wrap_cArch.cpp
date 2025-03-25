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

// Helper function for unified parameter setting
static void cArchSet(cArch& self, const object& value_or_vector, const object& index_or_name, const object& num_param = object())
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
            uint num = 0;
            if (!num_param.is_none())
                num = extract<uint>(num_param);
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

// Helper function for unified parameter getting
static object cArchGet(cArch& self, const object& index_or_name, const object& num_param = object())
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

// Helper for unified realloc
static void cArchReAlloc(cArch& self, const object& size_or_vector, const object& num_param = object())
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

void export_cArch() {
    class_<cArch, bases<cAbstCondVar> >("cArch",
        "ARCH (AutoRegressive Conditional Heteroskedasticity) variance model.\n\n"
        "The ARCH(p) model represents the conditional variance as a function of past squared residuals\n",
        init<optional<uint> >(
            (boost::python::arg("n_arch") = 0),
            "Create an ARCH(p) model with p=n_arch.\n\n"
            "Parameters:\n"
            "    n_arch (int, optional): The order of the ARCH process. Default is 0."
        )
    )
        // Copy constructor
        .def(init<const cArch&>(
            boost::python::arg("other"),
            "Create a copy of an existing ARCH model.\n\n"
            "Parameters:\n"
            "    other (cArch): The ARCH model to copy."
        ))

        // Assignment operator (could be added)
        // .def("__assign__", &cArch::operator=, return_value_policy<reference_existing_object>())

        // Basic housekeeping with Pythonic naming
        .def("delete", &cArch::Delete,
            "Free memory used by the ARCH model.")
        .def("print", &cArchPrint,
            "Print the ARCH model parameters to standard output.")

        // Unified Python interfaces
        .def("set", &cArchSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Set model parameters using various parameter formats.\n\n"
            "This method provides a unified interface for setting parameters in different ways:\n\n"
            "Examples:\n"
            "    set(value, index, param_num): Set a single parameter value by numerical index\n"
            "    set(value, name, index): Set a single parameter value by name\n"
            "    set(vector, param_num): Set multiple parameters from a vector by numerical index\n"
            "    set(vector, name): Set multiple parameters from a vector by name"
        )

        .def("get", &cArchGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Get model parameters using various parameter formats.\n\n"
            "This method provides a unified interface for getting parameters in different ways:\n\n"
            "Examples:\n"
            "    get(index, param_num): Get a single parameter value by numerical index\n"
            "    get(param_num): Get all parameters as a vector by numerical index\n"
            "    get(name, index): Get a single parameter value by name\n"
            "    get(name): Get all parameters as a vector by name\n\n"
            "Returns:\n"
            "    A single parameter value (double) or a vector of parameters (cDVector)"
        )

        .def("realloc", &cArchReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("param_num") = object()),
            "Reallocate memory for model parameters.\n\n"
            "This method provides a unified interface for reallocating parameters in different ways:\n\n"
            "Parameters:\n"
            "    size_or_vector: Either a size (uint) or a vector (cDVector)\n"
            "    param_num (int, optional): Parameter number. Default is 0."
        )

        // SetDefaultInitPoint methods - Pythonic naming
        .def("set_default_init_point",
            static_cast<void (cArch::*)(double, double)>(&cArch::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Set default initial parameter values based on mean and variance.\n\n"
            "Parameters:\n"
            "    mean (double): Initial mean value\n"
            "    var (double): Initial variance value"
        )
        .def("set_default_init_point",
            static_cast<void (cArch::*)(cRegArchValue&)>(&cArch::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Set default initial parameter values based on RegArch values.\n\n"
            "Parameters:\n"
            "    value (cRegArchValue): RegArch values object"
        )

        // ReAlloc methods - individual exposure for backward compatibility
        .def("realloc_size",
            static_cast<void (cArch::*)(uint, uint)>(&cArch::ReAlloc),
            (boost::python::arg("size"), boost::python::arg("param_num") = 0),
            "Reallocate memory for a parameter of the specified size.\n\n"
            "Parameters:\n"
            "    size (int): The new size\n"
            "    param_num (int, optional): Parameter number. Default is 0."
        )
        .def("realloc_vect",
            static_cast<void (cArch::*)(const cDVector&, uint)>(&cArch::ReAlloc),
            (boost::python::arg("vect_param"), boost::python::arg("param_num") = 0),
            "Reallocate memory for a parameter based on the vector's size.\n\n"
            "Parameters:\n"
            "    vect_param (cDVector): The parameter vector\n"
            "    param_num (int, optional): Parameter number. Default is 0."
        )

        // Set methods - individual exposure
        .def("set_value_index",
            static_cast<void (cArch::*)(double, uint, uint)>(&cArch::Set),
            (boost::python::arg("value"), boost::python::arg("index"), boost::python::arg("param_num")),
            "Set a parameter value by numerical index.\n\n"
            "Parameters:\n"
            "    value (double): The parameter value\n"
            "    index (int): The parameter index\n"
            "    param_num (int): Parameter number"
        )
        .def("set_vector_index",
            static_cast<void (cArch::*)(const cDVector&, uint)>(&cArch::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_num")),
            "Set parameters from a vector by numerical index.\n\n"
            "Parameters:\n"
            "    vect_param (cDVector): The parameter vector\n"
            "    param_num (int): Parameter number"
        )

        // Get methods - individual exposure
        .def("get_value_index",
            static_cast<double (cArch::*)(uint, uint)>(&cArch::Get),
            (boost::python::arg("index"), boost::python::arg("param_num")),
            "Get a parameter value by numerical index.\n\n"
            "Parameters:\n"
            "    index (int): The parameter index\n"
            "    param_num (int): Parameter number\n\n"
            "Returns:\n"
            "    double: The parameter value"
        )
        .def("get_vector_index",
            static_cast<cDVector & (cArch::*)(uint)>(&cArch::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_num"),
            "Get parameters as a vector by numerical index.\n\n"
            "Parameters:\n"
            "    param_num (int): Parameter number\n\n"
            "Returns:\n"
            "    cDVector: The parameter vector"
        )

        // Set methods with param names
        .def("set_value_name",
            static_cast<void (cArch::*)(double, std::string, uint)>(&cArch::Set),
            (boost::python::arg("value"), boost::python::arg("param_name"), boost::python::arg("index")),
            "Set a parameter value by name.\n\n"
            "Parameters:\n"
            "    value (double): The parameter value\n"
            "    param_name (str): The parameter name\n"
            "    index (int): The parameter index"
        )
        .def("set_vector_name",
            static_cast<void (cArch::*)(const cDVector&, std::string)>(&cArch::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_name")),
            "Set parameters from a vector by name.\n\n"
            "Parameters:\n"
            "    vect_param (cDVector): The parameter vector\n"
            "    param_name (str): The parameter name"
        )

        // Get methods with param names
        .def("get_value_name",
            static_cast<double (cArch::*)(std::string, uint)>(&cArch::Get),
            (boost::python::arg("param_name"), boost::python::arg("index")),
            "Get a parameter value by name.\n\n"
            "Parameters:\n"
            "    param_name (str): The parameter name\n"
            "    index (int): The parameter index\n\n"
            "Returns:\n"
            "    double: The parameter value"
        )
        .def("get_vector_name",
            static_cast<cDVector & (cArch::*)(std::string)>(&cArch::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Get parameters as a vector by name.\n\n"
            "Parameters:\n"
            "    param_name (str): The parameter name\n\n"
            "Returns:\n"
            "    cDVector: The parameter vector"
        )

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
            "Reallocate proxy variance parameters.\n\n"
            "Parameters:\n"
            "    old_n_param (int, optional): Old number of parameters. Default is 0."
        )
        .def("update_proxy_var_parameters", &cArch::UpdateProxyVarParameters,
            "Update proxy variance parameters.")

        // Core functionality - Pythonic naming
        .def("compute_var", &cArch::ComputeVar,
            (boost::python::arg("date"), boost::python::arg("data")),
            "Compute conditional variance at the specified date.\n\n"
            "Parameters:\n"
            "    date (int): The date index\n"
            "    data (cRegArchValue): The data object\n\n"
            "Returns:\n"
            "    double: The computed conditional variance"
        )
        .def("get_n_param", &cArch::GetNParam,
            "Get the number of parameters in the model.\n\n"
            "Returns:\n"
            "    int: Number of parameters"
        )
        .def("get_n_lags", &cArch::GetNLags,
            "Get the number of lags required for gradient computation.\n\n"
            "Returns:\n"
            "    int: Number of lags"
        )
        .def("compute_grad", &cArch::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("residuals") = object()),
            "Compute the gradient of the model at the specified date.\n\n"
            "Parameters:\n"
            "    date (int): The date index\n"
            "    data (cRegArchValue): The data object\n"
            "    grad_data (cRegArchGradient): The gradient object to store results\n"
            "    residuals (cAbstResiduals, optional): The residuals distribution object"
        )
        .def("compute_hess", &cArch::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("residuals") = object()),
            "Compute the Hessian of the model at the specified date.\n\n"
            "Parameters:\n"
            "    date (int): The date index\n"
            "    data (cRegArchValue): The data object\n"
            "    grad_data (cRegArchGradient): The gradient object\n"
            "    hess_data (cRegArchHessien): The Hessian object to store results\n"
            "    residuals (cAbstResiduals, optional): The residuals distribution object"
        )
        .def("compute_grad_and_hess", &cArch::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("residuals") = object()),
            "Compute both gradient and Hessian in one operation.\n\n"
            "Parameters:\n"
            "    date (int): The date index\n"
            "    data (cRegArchValue): The data object\n"
            "    grad_data (cRegArchGradient): The gradient object to store results\n"
            "    hess_data (cRegArchHessien): The Hessian object to store results\n"
            "    residuals (cAbstResiduals, optional): The residuals distribution object"
        )
        .def("reg_arch_param_to_vector", &cArch::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index") = 0),
            "Convert model parameters to a vector representation.\n\n"
            "Parameters:\n"
            "    dest_vect (cDVector): The destination vector\n"
            "    index (int, optional): The starting index. Default is 0."
        )
        .def("vector_to_reg_arch_param", &cArch::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Convert a vector representation back to model parameters.\n\n"
            "Parameters:\n"
            "    src_vect (cDVector): The source vector\n"
            "    index (int, optional): The starting index. Default is 0."
        )

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
            "Get parameter name as a C-style string array.\n\n"
            "Parameters:\n"
            "    index (int): The parameter index\n"
            "Note: This method is primarily for internal use. Use get_param_name_string instead."
        )
        .def("get_param_name_string",
            static_cast<void (cArch::*)(uint, std::string[])>(&cArch::GetParamName),
            boost::python::arg("index"),
            "Get parameter name as a C++ string array.\n\n"
            "Parameters:\n"
            "    index (int): The parameter index"
        )
        .def("GetParamNameChar", static_cast<void (cArch::*)(uint, char**)>(&cArch::GetParamName))
        .def("GetParamNameString", static_cast<void (cArch::*)(uint, std::string[])>(&cArch::GetParamName))
        ;
}