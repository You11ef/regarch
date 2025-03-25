#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// 1) Include your helper header that declares py_list_or_tuple_to_cDVector(...)
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// Helper function for printing
static void cAparchPrint(cAparch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// Helper function to allow Python-style overloaded methods
static void cAparchSet(cAparch& self, const object& value_or_vector, const object& index_or_name, const object& num_param = object())
{
    // Check if first argument is a vector or a scalar
    extract<cDVector> vector_extract(value_or_vector);
    extract<double>   value_extract(value_or_vector);

    // Check if second argument is a string or an index
    extract<std::string> name_extract(index_or_name);
    extract<uint>        index_extract(index_or_name);

    if (vector_extract.check())
    {
        // The user passed a cDVector directly
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
    else if (value_extract.check())
    {
        // The user passed a scalar (double)
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
        // 2) Fallback for Python list or tuple
        if (PyList_Check(value_or_vector.ptr()) || PyTuple_Check(value_or_vector.ptr()))
        {
            // Convert it to cDVector via your helper
            cDVector vector = py_list_or_tuple_to_cDVector(value_or_vector);

            if (name_extract.check()) {
                std::string name = name_extract();
                self.Set(vector, name);
            }
            else if (index_extract.check()) {
                uint num = extract<uint>(num_param);
                self.Set(vector, num);
            }
            else {
                throw std::runtime_error("Invalid arguments for set method (second arg must be str or uint)");
            }
        }
        else {
            // 3) If none of the above checks worked, error out
            throw std::runtime_error("First argument must be a float, cDVector, or a Python list/tuple of floats");
        }
    }
}

// Helper function for get
static object cAparchGet(cAparch& self, const object& index_or_name, const object& num_param = object())
{
    // Check if first argument is a string or an index
    extract<std::string> name_extract(index_or_name);
    extract<uint>        index_extract(index_or_name);

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
static void cAparchReAlloc(cAparch& self, const object& size_or_vector, const object& num_param = object())
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
        // Fallback for Python list/tuple
        if (PyList_Check(size_or_vector.ptr()) || PyTuple_Check(size_or_vector.ptr()))
        {
            cDVector vector = py_list_or_tuple_to_cDVector(size_or_vector);
            self.ReAlloc(vector, param);
        }
        else {
            throw std::runtime_error("First argument must be a uint size, cDVector, or a Python list/tuple of floats");
        }
    }
}

// Helper function to get parameter names as a Python list
boost::python::list getAparchParamNames(cAparch& self, uint theIndex)
{
    std::string names[10];
    self.GetParamName(theIndex, names);

    boost::python::list result;
    for (int i = 0; i < 10; i++)
    {
        if (!names[i].empty())
            result.append(names[i]);
    }
    return result;
}

void export_cAparch()
{
    class_<cAparch, bases<cAbstCondVar> >("cAparch",
        "APARCH (Asymmetric Power ARCH) conditional variance model.\n\n"
        "The APARCH model captures asymmetric effects in volatility using power transformations.\n"
        "Parameters\n"
        "----------\n"
        "n_arch : int, optional\n"
        "    ARCH order (p) - number of lagged innovations (default 0)\n"
        "n_garch : int, optional\n"
        "    GARCH order (q) - number of lagged conditional variances (default 0)",
        init<optional<int, int> >(
            (boost::python::arg("n_arch") = 0, boost::python::arg("n_garch") = 0),
            "Create an APARCH(p,q) model with specified orders.\n\n"
            "Parameters\n"
            "----------\n"
            "n_arch : int, optional\n"
            "    ARCH order (p) - number of lagged innovations (default 0)\n"
            "n_garch : int, optional\n"
            "    GARCH order (q) - number of lagged conditional variances (default 0)"
        )
    )
        // Copy constructor
        .def(init<const cAparch&>(
            boost::python::arg("other"),
            "Create a copy of an existing APARCH model.\n\n"
            "Parameters\n"
            "----------\n"
            "other : cAparch\n"
            "    The APARCH model to copy"
        ))

        // Assignment operator
        .def("__assign__", &cAparch::operator=,
            return_value_policy<reference_existing_object>(),
            "Assigns another APARCH model to this one.\n\n"
            "Parameters\n"
            "----------\n"
            "other : cAparch\n"
            "    The APARCH model to assign from\n\n"
            "Returns\n"
            "-------\n"
            "cAparch\n"
            "    Reference to this model after assignment")

        // Basic housekeeping
        .def("delete", &cAparch::Delete,
            "Releases memory used by the model.\n\n"
            "This method frees all resources allocated by this model instance.")

        // Printing
        .def("print", &cAparchPrint,
            "Prints model parameters to standard output.\n\n"
            "This method displays all parameters of the model in a readable format.")

        // Parameter names
        .def("get_param_names", &getAparchParamNames,
            boost::python::arg("index"),
            "Returns a list of parameter names for the given index.\n\n"
            "Parameters\n"
            "----------\n"
            "index : int\n"
            "    The parameter group index\n\n"
            "Returns\n"
            "-------\n"
            "list\n"
            "    List of parameter names for the specified index")

        // SetDefaultInitPoint methods - Pythonic naming
        .def("set_default_init_point",
            static_cast<void (cAparch::*)(double, double)>(&cAparch::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Sets default initial parameter values based on mean and variance.\n\n"
            "Parameters\n"
            "----------\n"
            "mean : float\n"
            "    Sample mean of the time series\n"
            "var : float\n"
            "    Sample variance of the time series")
        .def("set_default_init_point_value",
            static_cast<void (cAparch::*)(cRegArchValue&)>(&cAparch::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Sets default initial parameter values based on RegArch values.\n\n"
            "Parameters\n"
            "----------\n"
            "value : cRegArchValue\n"
            "    Container with the time series data and other values")

        // Unified Python interface methods
        .def("set", &cAparchSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Sets model parameters in a flexible way.\n\n"
            "This method supports multiple calling patterns:\n"
            "  - set(value, index, [param_num]): Set a single parameter value by numerical index\n"
            "  - set(value, name, [index]): Set a single parameter value by name\n"
            "  - set(vector, param_num): Set multiple parameter values by numerical index\n"
            "  - set(vector, name): Set multiple parameter values by name\n\n"
            "Parameters\n"
            "----------\n"
            "value_or_vector : float or cDVector\n"
            "    Either a single parameter value (float) or vector of values (cDVector)\n"
            "index_or_name : int or str\n"
            "    Either an index (int) or parameter name (str)\n"
            "num_param : int, optional\n"
            "    Additional index or parameter number, depending on the call pattern")

        .def("get", &cAparchGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Gets model parameters in a flexible way.\n\n"
            "This method supports multiple calling patterns:\n"
            "  - get(param_num): Get all parameters for the given parameter number as a vector\n"
            "  - get(index, param_num): Get a specific parameter value by numerical index\n"
            "  - get(name): Get all parameters for the given parameter name as a vector\n"
            "  - get(name, index): Get a specific parameter value by name and index\n\n"
            "Parameters\n"
            "----------\n"
            "index_or_name : int or str\n"
            "    Either a parameter number (int) or parameter name (str)\n"
            "num_param : int, optional\n"
            "    Index for retrieving a specific value\n\n"
            "Returns\n"
            "-------\n"
            "float or cDVector\n"
            "    Either a single parameter value or a vector of values")

        .def("realloc", &cAparchReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("param_num") = object()),
            "Reallocates memory for model parameters.\n\n"
            "This method supports two calling patterns:\n"
            "  - realloc(size, [param_num]): Reallocate with a specified size\n"
            "  - realloc(vector, [param_num]): Reallocate based on the vector's size\n\n"
            "Parameters\n"
            "----------\n"
            "size_or_vector : int or cDVector\n"
            "    Either the size to allocate (int) or a vector of values (cDVector)\n"
            "param_num : int, optional\n"
            "    The parameter number to reallocate (default 0)")

        // For backwards compatibility and access to specific methods
        // Original overloaded 'Set' methods
        .def("set_value_index",
            static_cast<void (cAparch::*)(double, uint, uint)>(&cAparch::Set),
            (boost::python::arg("value"), boost::python::arg("index"), boost::python::arg("param_num")),
            "Sets a parameter value by numerical index.\n\n"
            "Parameters\n"
            "----------\n"
            "value : float\n"
            "    The parameter value to set\n"
            "index : int\n"
            "    The index of the parameter\n"
            "param_num : int\n"
            "    The parameter number")
        .def("set_vector_index",
            static_cast<void (cAparch::*)(const cDVector&, uint)>(&cAparch::Set),
            (boost::python::arg("values"), boost::python::arg("param_num")),
            "Sets parameters from a vector by numerical index.\n\n"
            "Parameters\n"
            "----------\n"
            "values : cDVector\n"
            "    Vector of parameter values\n"
            "param_num : int\n"
            "    The parameter number")
        .def("set_value_name",
            static_cast<void (cAparch::*)(double, std::string, uint)>(&cAparch::Set),
            (boost::python::arg("value"), boost::python::arg("param_name"), boost::python::arg("index") = 0),
            "Sets a parameter value by name.\n\n"
            "Parameters\n"
            "----------\n"
            "value : float\n"
            "    The parameter value to set\n"
            "param_name : str\n"
            "    The name of the parameter\n"
            "index : int, optional\n"
            "    The index of the parameter (default 0)")
        .def("set_vector_name",
            static_cast<void (cAparch::*)(const cDVector&, std::string)>(&cAparch::Set),
            (boost::python::arg("values"), boost::python::arg("param_name")),
            "Sets parameters from a vector by name.\n\n"
            "Parameters\n"
            "----------\n"
            "values : cDVector\n"
            "    Vector of parameter values\n"
            "param_name : str\n"
            "    The name of the parameter")

        // Original overloaded 'Get' methods
        .def("get_value_index",
            static_cast<double (cAparch::*)(uint, uint)>(&cAparch::Get),
            (boost::python::arg("index"), boost::python::arg("param_num")),
            "Gets a parameter value by numerical index.\n\n"
            "Parameters\n"
            "----------\n"
            "index : int\n"
            "    The index of the parameter\n"
            "param_num : int\n"
            "    The parameter number\n\n"
            "Returns\n"
            "-------\n"
            "float\n"
            "    The parameter value")
        .def("get_vector_index",
            static_cast<cDVector & (cAparch::*)(uint)>(&cAparch::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_num"),
            "Gets parameters as a vector by numerical index.\n\n"
            "Parameters\n"
            "----------\n"
            "param_num : int\n"
            "    The parameter number\n\n"
            "Returns\n"
            "-------\n"
            "cDVector\n"
            "    Vector of parameter values")
        .def("get_value_name",
            static_cast<double (cAparch::*)(std::string, uint)>(&cAparch::Get),
            (boost::python::arg("param_name"), boost::python::arg("index")),
            "Gets a parameter value by name.\n\n"
            "Parameters\n"
            "----------\n"
            "param_name : str\n"
            "    The name of the parameter\n"
            "index : int\n"
            "    The index of the parameter\n\n"
            "Returns\n"
            "-------\n"
            "float\n"
            "    The parameter value")
        .def("get_vector_name",
            static_cast<cDVector & (cAparch::*)(std::string)>(&cAparch::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Gets parameters as a vector by name.\n\n"
            "Parameters\n"
            "----------\n"
            "param_name : str\n"
            "    The name of the parameter\n\n"
            "Returns\n"
            "-------\n"
            "cDVector\n"
            "    Vector of parameter values")

        // Added SyncSizeParam method
        .def("sync_size_param", &cAparch::SyncSizeParam,
            boost::python::arg("num_param"),
            "Synchronizes parameter sizes.\n\n"
            "Parameters\n"
            "----------\n"
            "num_param : int\n"
            "    The parameter number to synchronize")

        // Added proxy parameter methods
        .def("realloc_proxy_var_parameters", &cAparch::ReAllocProxyVarParameters,
            (boost::python::arg("old_n_param") = 0),
            "Reallocates proxy variance parameters.\n\n"
            "Parameters\n"
            "----------\n"
            "old_n_param : int, optional\n"
            "    The previous number of parameters (default 0)")
        .def("update_proxy_var_parameters", &cAparch::UpdateProxyVarParameters,
            "Updates proxy variance parameters.\n\n"
            "This method updates internal calculations after parameters change.")

        // Key APARCH methods
        .def("compute_var", &cAparch::ComputeVar,
            (boost::python::arg("date"), boost::python::arg("value")),
            "Computes conditional variance at the specified date given the data.\n\n"
            "Parameters\n"
            "----------\n"
            "date : int\n"
            "    The time index for which to compute the variance\n"
            "value : cRegArchValue\n"
            "    Container with the time series data and other values\n\n"
            "Returns\n"
            "-------\n"
            "float\n"
            "    The computed conditional variance value")
        .def("get_n_param", &cAparch::GetNParam,
            "Returns the number of parameters in the model.\n\n"
            "Returns\n"
            "-------\n"
            "int\n"
            "    Total number of parameters in this model")
        .def("get_n_lags", &cAparch::GetNLags,
            "Returns the number of lags required for gradient computation.\n\n"
            "Returns\n"
            "-------\n"
            "int\n"
            "    Maximum number of lags needed by this model")
        .def("reg_arch_param_to_vector", &cAparch::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index") = 0),
            "Converts model parameters to a vector representation.\n\n"
            "Parameters\n"
            "----------\n"
            "dest_vect : cDVector\n"
            "    Destination vector where parameters will be stored\n"
            "index : int, optional\n"
            "    Starting index in the destination vector (default 0)")
        .def("vector_to_reg_arch_param", &cAparch::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Converts a vector representation back to model parameters.\n\n"
            "Parameters\n"
            "----------\n"
            "src_vect : cDVector\n"
            "    Source vector containing the parameter values\n"
            "index : int, optional\n"
            "    Starting index in the source vector (default 0)")
        .def("compute_grad", &cAparch::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("residuals") = object()),
            "Computes the gradient of the model at the specified date.\n\n"
            "Parameters\n"
            "----------\n"
            "date : int\n"
            "    The time index for which to compute the gradient\n"
            "data : cRegArchValue\n"
            "    Container with the time series data and other values\n"
            "grad_data : cRegArchGradient\n"
            "    Gradient container where results will be stored\n"
            "residuals : cAbstResiduals, optional\n"
            "    Residuals object for certain models (default None)")
        .def("compute_hess", &cAparch::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("residuals") = object()),
            "Computes the Hessian of the model at the specified date.\n\n"
            "Parameters\n"
            "----------\n"
            "date : int\n"
            "    The time index for which to compute the Hessian\n"
            "data : cRegArchValue\n"
            "    Container with the time series data and other values\n"
            "grad_data : cRegArchGradient\n"
            "    Gradient container with pre-computed gradient values\n"
            "hess_data : cRegArchHessien\n"
            "    Hessian container where results will be stored\n"
            "residuals : cAbstResiduals, optional\n"
            "    Residuals object for certain models (default None)")
        .def("compute_grad_and_hess", &cAparch::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("residuals") = object()),
            "Computes both gradient and Hessian in one operation.\n\n"
            "This method may be more efficient than computing them separately.\n\n"
            "Parameters\n"
            "----------\n"
            "date : int\n"
            "    The time index for which to compute the derivatives\n"
            "data : cRegArchValue\n"
            "    Container with the time series data and other values\n"
            "grad_data : cRegArchGradient\n"
            "    Gradient container where gradient results will be stored\n"
            "hess_data : cRegArchHessien\n"
            "    Hessian container where Hessian results will be stored\n"
            "residuals : cAbstResiduals, optional\n"
            "    Residuals object for certain models (default None)")

        // Parameter name getters
        .def("get_param_name_char",
            static_cast<void (cAparch::*)(uint, char**)>(&cAparch::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C-style string.\n\n"
            "Parameters\n"
            "----------\n"
            "index : int\n"
            "    The parameter index\n\n"
            "Returns\n"
            "-------\n"
            "char**\n"
            "    Parameter names as C-style strings")
        .def("get_param_name_string",
            static_cast<void (cAparch::*)(uint, std::string[])>(&cAparch::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C++ string.\n\n"
            "Parameters\n"
            "----------\n"
            "index : int\n"
            "    The parameter index\n\n"
            "Returns\n"
            "-------\n"
            "string[]\n"
            "    Parameter names as C++ strings")
        ;
}
