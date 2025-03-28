#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// Include the helper for Python list conversions
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// Helper for printing
static void StdDevInMean_Print(cStdDevInMean& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "set" that accepts float or Python lists
// --------------------------------------------------------------------
static void cStdDevInMeanSet(cStdDevInMean& self,
    const object& value_or_vector,
    const object& index_or_name,
    const object& num_param = object())
{
    extract<cDVector> vec_extract(value_or_vector);
    extract<double>   val_extract(value_or_vector);

    // Check if second argument is a string or a uint
    extract<std::string> name_extract(index_or_name);
    extract<uint>        idx_extract(index_or_name);

    if (vec_extract.check())
    {
        // The user passed a cDVector directly
        cDVector vect = vec_extract();

        if (name_extract.check()) {
            // self.Set(cDVector, std::string)
            self.Set(vect, name_extract());
        }
        else if (idx_extract.check()) {
            // self.Set(cDVector, uint)
            uint param_num = 0;
            if (!num_param.is_none())
                param_num = extract<uint>(num_param);
            self.Set(vect, param_num);
        }
        else {
            throw std::runtime_error("cStdDevInMean.set(...) usage: second argument must be string or uint when passing a cDVector.");
        }
    }
    else if (val_extract.check())
    {
        // The user passed a single float/double
        double val = val_extract();

        if (name_extract.check()) {
            // self.Set(double, std::string, uint)
            std::string param_name = name_extract();
            uint idx = 0;
            if (!num_param.is_none())
                idx = extract<uint>(num_param);
            self.Set(val, param_name, idx);
        }
        else if (idx_extract.check()) {
            // self.Set(double, uint, uint)
            uint idx = idx_extract();
            uint param_num = 0;
            if (!num_param.is_none())
                param_num = extract<uint>(num_param);
            self.Set(val, idx, param_num);
        }
        else {
            throw std::runtime_error("cStdDevInMean.set(...) usage: second argument must be string or uint when passing a float.");
        }
    }
    else
    {
        // Fallback for Python list/tuple
        if (PyList_Check(value_or_vector.ptr()) || PyTuple_Check(value_or_vector.ptr()))
        {
            // Convert Python list/tuple to cDVector using helper
            cDVector vect = py_list_or_tuple_to_cDVector(value_or_vector);

            if (name_extract.check()) {
                self.Set(vect, name_extract());
            }
            else if (idx_extract.check()) {
                uint param_num = 0;
                if (!num_param.is_none())
                    param_num = extract<uint>(num_param);
                self.Set(vect, param_num);
            }
            else {
                throw std::runtime_error("cStdDevInMean.set(...) usage: second argument must be string or uint when passing a Python list/tuple.");
            }
        }
        else {
            throw std::runtime_error("First argument must be float, cDVector, or a Python list/tuple of floats for cStdDevInMean.set(...).");
        }
    }
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "get" that returns float or cDVector
// --------------------------------------------------------------------
static object cStdDevInMeanGet(cStdDevInMean& self,
    const object& index_or_name,
    const object& num_param = object())
{
    extract<std::string> name_extract(index_or_name);
    extract<uint>        idx_extract(index_or_name);

    if (name_extract.check())
    {
        // The user passed a param name
        std::string pname = name_extract();

        if (num_param.is_none()) {
            // Get(string) -> cDVector&
            return object(boost::ref(self.Get(pname)));
        }
        else {
            // Get(string, uint) -> double
            uint idx = extract<uint>(num_param);
            return object(self.Get(pname, idx));
        }
    }
    else if (idx_extract.check())
    {
        // The user passed a param index
        uint idx = idx_extract();

        if (num_param.is_none()) {
            // Get(uint) -> cDVector&
            return object(boost::ref(self.Get(idx)));
        }
        else {
            // Get(uint, uint) -> double
            uint sub_idx = extract<uint>(num_param);
            return object(self.Get(idx, sub_idx));
        }
    }
    else {
        throw std::runtime_error("First argument must be a param name (string) or a param index (uint) for cStdDevInMean.get(...).");
    }
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "realloc" that can accept a Python list
// --------------------------------------------------------------------
static void cStdDevInMeanReAlloc(cStdDevInMean& self,
    const object& size_or_vector,
    const object& num_param = object())
{
    extract<cDVector> vec_extract(size_or_vector);
    extract<uint>     size_extract(size_or_vector);

    uint param = 0;
    if (!num_param.is_none())
        param = extract<uint>(num_param);

    if (vec_extract.check())
    {
        // self.ReAlloc(cDVector, uint)
        cDVector vect = vec_extract();
        self.ReAlloc(vect, param);
    }
    else if (size_extract.check())
    {
        // self.ReAlloc(uint, uint)
        uint sz = size_extract();
        self.ReAlloc(sz, param);
    }
    else
    {
        // Fallback for Python list/tuple
        if (PyList_Check(size_or_vector.ptr()) || PyTuple_Check(size_or_vector.ptr()))
        {
            cDVector vect = py_list_or_tuple_to_cDVector(size_or_vector);
            self.ReAlloc(vect, param);
        }
        else {
            throw std::runtime_error("First argument must be uint, cDVector, or a Python list/tuple of floats for cStdDevInMean.realloc(...).");
        }
    }
}

void export_cStdDevInMean()
{
    class_<cStdDevInMean, bases<cAbstCondMean> >("cStdDevInMean",
        "Standard-Deviation-in-Mean component.\n\n"
        "This component adds the standard deviation of the conditional variance\n"
        "multiplied by a coefficient into the conditional mean equation.",
        init< optional<double> >(
            (boost::python::arg("theStdDevInMean") = 0.0),
            "cStdDevInMean(double theStdDevInMean=0.0)\n\n"
            "Create a Standard-Deviation-in-Mean model with an optional coefficient."
        )
    )
        .def("delete", &cStdDevInMean::Delete,
            "Free memory associated with this model.")
        .def("print", &StdDevInMean_Print,
            "Print model information to standard output.")

        // SetDefaultInitPoint methods
        .def("set_default_init_point",
            static_cast<void (cStdDevInMean::*)(double, double)>(&cStdDevInMean::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Set default initial parameter values based on mean and variance.")
        .def("set_default_init_point_value",
            static_cast<void (cStdDevInMean::*)(cRegArchValue&)>(&cStdDevInMean::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Set default initial parameter values based on RegArch values.")

        // Overloaded ReAlloc - original methods
        .def("realloc",
            static_cast<void (cStdDevInMean::*)(uint, uint)>(&cStdDevInMean::ReAlloc),
            (boost::python::arg("size"), boost::python::arg("param_num") = 0),
            "Reallocate memory for model parameters.")
        .def("realloc_vect",
            static_cast<void (cStdDevInMean::*)(const cDVector&, uint)>(&cStdDevInMean::ReAlloc),
            (boost::python::arg("vect_param"), boost::python::arg("param_num") = 0),
            "Reallocate memory for model parameters based on vector size.")

        // Overloaded Set - original methods
        .def("set_value_index",
            static_cast<void (cStdDevInMean::*)(double, uint, uint)>(&cStdDevInMean::Set),
            (boost::python::arg("value"), boost::python::arg("index"), boost::python::arg("param_num")),
            "Set a parameter value by numerical index.")
        .def("set_vector_index",
            static_cast<void (cStdDevInMean::*)(const cDVector&, uint)>(&cStdDevInMean::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_num")),
            "Set parameters from a vector by numerical index.")

        // Overloaded Get - original methods
        .def("get_value_index",
            static_cast<double (cStdDevInMean::*)(uint, uint)>(&cStdDevInMean::Get),
            (boost::python::arg("index"), boost::python::arg("param_num")),
            "Get a parameter value by numerical index.")
        .def("get_vector_index",
            static_cast<cDVector & (cStdDevInMean::*)(uint)>(&cStdDevInMean::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_num"),
            "Get parameters as a vector by numerical index.")

        // Overloaded set/get with string - original methods
        .def("set_value_name",
            static_cast<void (cStdDevInMean::*)(double, std::string, uint)>(&cStdDevInMean::Set),
            (boost::python::arg("value"), boost::python::arg("param_name"), boost::python::arg("index") = 0),
            "Set a parameter value by name.")
        .def("set_vector_name",
            static_cast<void (cStdDevInMean::*)(const cDVector&, std::string)>(&cStdDevInMean::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_name")),
            "Set parameters from a vector by name.")
        .def("get_value_name",
            static_cast<double (cStdDevInMean::*)(std::string, uint)>(&cStdDevInMean::Get),
            (boost::python::arg("param_name"), boost::python::arg("index")),
            "Get a parameter value by name.")
        .def("get_vector_name",
            static_cast<cDVector & (cStdDevInMean::*)(std::string)>(&cStdDevInMean::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Get parameters as a vector by name.")

        // Core methods
        .def("compute_mean", &cStdDevInMean::ComputeMean,
            (boost::python::arg("date"), boost::python::arg("data")),
            "Compute the conditional mean at the specified date.")
        .def("get_n_param", &cStdDevInMean::GetNParam,
            "Get the number of parameters in the model.")
        .def("get_n_lags", &cStdDevInMean::GetNLags,
            "Get the number of lags required by the model.")
        .def("reg_arch_param_to_vector", &cStdDevInMean::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index")),
            "Convert model parameters to a vector representation.")
        .def("vector_to_reg_arch_param", &cStdDevInMean::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Convert a vector representation back to model parameters.")
        .def("compute_grad", &cStdDevInMean::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("beg_index")),
            "Compute the gradient of the model at the specified date.")
        .def("compute_hess", &cStdDevInMean::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"),
                boost::python::arg("hess_data"), boost::python::arg("beg_index")),
            "Compute the Hessian of the model at the specified date.")
        .def("compute_grad_and_hess", &cStdDevInMean::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"),
                boost::python::arg("hess_data"), boost::python::arg("beg_index")),
            "Compute both the gradient and Hessian of the model at the specified date.")

        // Parameter name getters
        .def("get_param_name_char",
            static_cast<void (cStdDevInMean::*)(uint, char**)>(&cStdDevInMean::GetParamName),
            boost::python::arg("index"),
            "Get parameter names as C-style strings.")
        .def("get_param_name_string",
            static_cast<void (cStdDevInMean::*)(uint, std::string[])>(&cStdDevInMean::GetParamName),
            boost::python::arg("index"),
            "Get parameter names as C++ strings.")

        // For backwards compatibility - original C++ naming
        .def("Delete", &cStdDevInMean::Delete)
        .def("Print", &StdDevInMean_Print)
        .def("SetDefaultInitPoint", static_cast<void (cStdDevInMean::*)(double, double)>(&cStdDevInMean::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue", static_cast<void (cStdDevInMean::*)(cRegArchValue&)>(&cStdDevInMean::SetDefaultInitPoint))
        .def("ReAlloc", static_cast<void (cStdDevInMean::*)(uint, uint)>(&cStdDevInMean::ReAlloc))
        .def("ReAllocVect", static_cast<void (cStdDevInMean::*)(const cDVector&, uint)>(&cStdDevInMean::ReAlloc))
        .def("SetValueIndex", static_cast<void (cStdDevInMean::*)(double, uint, uint)>(&cStdDevInMean::Set))
        .def("SetVectorIndex", static_cast<void (cStdDevInMean::*)(const cDVector&, uint)>(&cStdDevInMean::Set))
        .def("GetValueIndex", static_cast<double (cStdDevInMean::*)(uint, uint)>(&cStdDevInMean::Get))
        .def("GetVectorIndex", static_cast<cDVector & (cStdDevInMean::*)(uint)>(&cStdDevInMean::Get), return_value_policy<reference_existing_object>())
        .def("SetValueName", static_cast<void (cStdDevInMean::*)(double, std::string, uint)>(&cStdDevInMean::Set))
        .def("SetVectorName", static_cast<void (cStdDevInMean::*)(const cDVector&, std::string)>(&cStdDevInMean::Set))
        .def("GetValueName", static_cast<double (cStdDevInMean::*)(std::string, uint)>(&cStdDevInMean::Get))
        .def("GetVectorName", static_cast<cDVector & (cStdDevInMean::*)(std::string)>(&cStdDevInMean::Get), return_value_policy<reference_existing_object>())
        .def("ComputeMean", &cStdDevInMean::ComputeMean)
        .def("GetNParam", &cStdDevInMean::GetNParam)
        .def("GetNLags", &cStdDevInMean::GetNLags)
        .def("RegArchParamToVector", &cStdDevInMean::RegArchParamToVector)
        .def("VectorToRegArchParam", &cStdDevInMean::VectorToRegArchParam)
        .def("ComputeGrad", &cStdDevInMean::ComputeGrad)
        .def("ComputeHess", &cStdDevInMean::ComputeHess)
        .def("ComputeGradAndHess", &cStdDevInMean::ComputeGradAndHess)
        .def("GetParamNameChar", static_cast<void (cStdDevInMean::*)(uint, char**)>(&cStdDevInMean::GetParamName))
        .def("GetParamNameString", static_cast<void (cStdDevInMean::*)(uint, std::string[])>(&cStdDevInMean::GetParamName))

        // -------------------------------------------------------------
        // NEW: unified set(...), get(...), and realloc(...) that accept
        // Python lists/tuples for cDVector
        // -------------------------------------------------------------
        .def("set", &cStdDevInMeanSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to set parameters.\n\n"
            "Examples:\n"
            "  stddevinmean.set(0.5, 0)                # set coefficient to 0.5\n"
            "  stddevinmean.set([0.5], 0)              # set from a Python list\n"
            "  stddevinmean.set(0.7, 'stddevinmean')   # set by parameter name"
        )
        .def("get", &cStdDevInMeanGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to get parameters.\n\n"
            "Examples:\n"
            "  coef = stddevinmean.get(0, 0)           # get coefficient value\n"
            "  vec = stddevinmean.get(0)               # get as vector\n"
            "  coef = stddevinmean.get('stddevinmean') # get by parameter name"
        )
        .def("realloc", &cStdDevInMeanReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("num_param") = object()),
            "Unified method to reallocate parameters.\n\n"
            "Examples:\n"
            "  stddevinmean.realloc(1)                 # reallocate size=1\n"
            "  stddevinmean.realloc([0.5])             # from Python list\n"
            "  stddevinmean.realloc(cDVector(...))     # from cDVector"
        )
        ;
}