#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// 1) Include your helper header for converting Python lists/tuples to cDVector
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// Existing small helper for printing
static void ConstCondVar_Print(cConstCondVar& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "set" that accepts float or Python lists
// --------------------------------------------------------------------
static void cConstCondVarSet(cConstCondVar& self,
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
        // The user passed a cDVector or Python list that auto-converted
        cDVector vect = vec_extract();

        if (name_extract.check()) {
            // Set(cDVector, param_name)
            self.Set(vect, name_extract());
        }
        else if (idx_extract.check()) {
            // Set(cDVector, uint)
            uint param_num = 0;
            if (!num_param.is_none())
                param_num = extract<uint>(num_param);
            self.Set(vect, param_num);
        }
        else {
            throw std::runtime_error(
                "cConstCondVar.set(...) usage: second arg must be string or uint when passing a cDVector.");
        }
    }
    else if (val_extract.check())
    {
        // The user passed a scalar (float/double)
        double val = val_extract();

        if (name_extract.check()) {
            // Set(double, param_name, index)
            std::string param_name = name_extract();
            uint idx = 0;
            if (!num_param.is_none())
                idx = extract<uint>(num_param);
            self.Set(val, param_name, idx);
        }
        else if (idx_extract.check()) {
            // Set(double, uint, uint)
            uint idx = idx_extract();
            uint param_num = 0;
            if (!num_param.is_none())
                param_num = extract<uint>(num_param);
            self.Set(val, idx, param_num);
        }
        else {
            throw std::runtime_error(
                "cConstCondVar.set(...) usage: second arg must be string or uint when passing a float.");
        }
    }
    else
    {
        // Fallback for Python list/tuple that didn't auto-convert
        if (PyList_Check(value_or_vector.ptr()) || PyTuple_Check(value_or_vector.ptr()))
        {
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
                throw std::runtime_error(
                    "cConstCondVar.set(...) usage: second arg must be string or uint when passing a Python list/tuple.");
            }
        }
        else {
            throw std::runtime_error(
                "First arg must be float, cDVector, or a Python list/tuple of floats for cConstCondVar.set(...).");
        }
    }
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "get" that returns float or cDVector
// --------------------------------------------------------------------
static object cConstCondVarGet(cConstCondVar& self,
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
            // get(param_name) -> cDVector&
            return object(boost::ref(self.Get(pname)));
        }
        else {
            // get(param_name, idx) -> double
            uint idx = extract<uint>(num_param);
            return object(self.Get(pname, idx));
        }
    }
    else if (idx_extract.check())
    {
        // The user passed a param index
        uint idx = idx_extract();

        if (num_param.is_none()) {
            // get(uint) -> cDVector&
            return object(boost::ref(self.Get(idx)));
        }
        else {
            // get(uint, subindex) -> double
            uint sub_idx = extract<uint>(num_param);
            return object(self.Get(idx, sub_idx));
        }
    }
    else {
        throw std::runtime_error(
            "First argument must be a param name (string) or a param index (uint) for cConstCondVar.get(...).");
    }
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "realloc" that can accept size or python list
// --------------------------------------------------------------------
static void cConstCondVarReAlloc(cConstCondVar& self,
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
        // ReAlloc(cDVector, uint)
        cDVector vect = vec_extract();
        self.ReAlloc(vect, param);
    }
    else if (size_extract.check())
    {
        // ReAlloc(uint, uint)
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
            throw std::runtime_error(
                "cConstCondVar.realloc(...) usage: must pass a uint size, cDVector, or a Python list/tuple of floats.");
        }
    }
}

// --------------------------------------------------------------------
// Export function with minimal new additions
// --------------------------------------------------------------------
void export_cConstCondVar()
{
    class_<cConstCondVar, bases<cAbstCondVar> >("cConstCondVar",
        "Constant conditional variance model.\n\n"
        "This class implements a constant conditional variance model.\n"
        "It uses a single parameter representing the constant variance value.",
        init<optional<double> >(
            (boost::python::arg("value") = 1.0),
            "cConstCondVar(double value=1.0)\n\n"
            "Creates a constant variance model with the specified value."
        )
    )
        // Copy constructor
        .def(init<const cConstCondVar&>(
            boost::python::arg("other"),
            "cConstCondVar(cConstCondVar other)\n\n"
            "Copy constructor - creates a copy of another constant variance model."
        ))

        // Pythonic naming
        .def("delete", &cConstCondVar::Delete,
            "Releases memory used by the model.")
        .def("print", &ConstCondVar_Print,
            "Prints model parameters to standard output.")
        .def("set_default_init_point",
            static_cast<void (cConstCondVar::*)(double, double)>(&cConstCondVar::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Sets default initial parameter values based on mean and variance.")
        .def("set_default_init_point_value",
            static_cast<void (cConstCondVar::*)(cRegArchValue&)>(&cConstCondVar::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Sets default initial parameter values based on RegArch values.")

        // ReAlloc methods - Pythonic naming
        .def("realloc",
            static_cast<void (cConstCondVar::*)(uint, uint)>(&cConstCondVar::ReAlloc),
            (boost::python::arg("size"), boost::python::arg("param_num") = 0),
            "Reallocates memory for model parameters.")
        .def("realloc_vect",
            static_cast<void (cConstCondVar::*)(const cDVector&, uint)>(&cConstCondVar::ReAlloc),
            (boost::python::arg("vect_param"), boost::python::arg("param_num") = 0),
            "Reallocates memory for model parameters based on vector size.")

        // Set methods - Pythonic naming
        .def("set_value_index",
            static_cast<void (cConstCondVar::*)(double, uint, uint)>(&cConstCondVar::Set),
            (boost::python::arg("value"), boost::python::arg("index"), boost::python::arg("param_num")),
            "Sets a parameter value by numerical index.")
        .def("set_vector_index",
            static_cast<void (cConstCondVar::*)(const cDVector&, uint)>(&cConstCondVar::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_num")),
            "Sets parameters from a vector by numerical index.")

        // Get methods - Pythonic naming
        .def("get_value_index",
            static_cast<double (cConstCondVar::*)(uint, uint)>(&cConstCondVar::Get),
            (boost::python::arg("index"), boost::python::arg("param_num")),
            "Gets a parameter value by numerical index.")
        .def("get_vector_index",
            static_cast<cDVector & (cConstCondVar::*)(uint)>(&cConstCondVar::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_num"),
            "Gets parameters as a vector by numerical index.")

        // Named parameters - Pythonic naming
        .def("set_value_name",
            static_cast<void (cConstCondVar::*)(double, std::string, uint)>(&cConstCondVar::Set),
            (boost::python::arg("value"), boost::python::arg("param_name"), boost::python::arg("index") = 0),
            "Sets a parameter value by name.")
        .def("set_vector_name",
            static_cast<void (cConstCondVar::*)(const cDVector&, std::string)>(&cConstCondVar::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_name")),
            "Sets parameters from a vector by name.")
        .def("get_value_name",
            static_cast<double (cConstCondVar::*)(std::string, uint)>(&cConstCondVar::Get),
            (boost::python::arg("param_name"), boost::python::arg("index")),
            "Gets a parameter value by name.")
        .def("get_vector_name",
            static_cast<cDVector & (cConstCondVar::*)(std::string)>(&cConstCondVar::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Gets parameters as a vector by name.")

        // Proxy parameter methods - Pythonic naming
        .def("realloc_proxy_var_parameters", &cConstCondVar::ReAllocProxyVarParameters,
            (boost::python::arg("old_n_param") = 0),
            "Reallocates proxy variance parameters.")
        .def("update_proxy_var_parameters", &cConstCondVar::UpdateProxyVarParameters,
            "Updates proxy variance parameters.")

        // Synchronize parameter size - Pythonic naming
        .def("sync_size_param", &cConstCondVar::SyncSizeParam,
            boost::python::arg("param_num"),
            "Synchronizes parameter sizes.")

        // Core functionality - Pythonic naming
        .def("compute_var", &cConstCondVar::ComputeVar,
            (boost::python::arg("date"), boost::python::arg("data")),
            "Computes the conditional variance at the specified date given the data.")
        .def("get_n_param", &cConstCondVar::GetNParam,
            "Returns the total number of parameters in the model.")
        .def("get_n_lags", &cConstCondVar::GetNLags,
            "Returns the number of lags required for computation.")
        .def("compute_grad", &cConstCondVar::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("residuals")),
            "Computes the gradient of the model at the specified date.")
        .def("compute_hess", &cConstCondVar::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("residuals")),
            "Computes the Hessian of the model at the specified date.")
        .def("compute_grad_and_hess", &cConstCondVar::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("residuals")),
            "Computes both gradient and Hessian in one operation.")
        .def("reg_arch_param_to_vector", &cConstCondVar::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index")),
            "Converts model parameters to a vector representation.")
        .def("vector_to_reg_arch_param", &cConstCondVar::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Converts a vector representation back to model parameters.")
        .def("get_param_name_char",
            static_cast<void (cConstCondVar::*)(uint, char**)>(&cConstCondVar::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C-style string.")
        .def("get_param_name_string",
            static_cast<void (cConstCondVar::*)(uint, std::string[])>(&cConstCondVar::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C++ string.")

        // Original C++ naming for backwards compatibility
        .def("Delete", &cConstCondVar::Delete)
        .def("Print", &ConstCondVar_Print)
        .def("SetDefaultInitPoint", static_cast<void (cConstCondVar::*)(double, double)>(&cConstCondVar::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue", static_cast<void (cConstCondVar::*)(cRegArchValue&)>(&cConstCondVar::SetDefaultInitPoint))
        .def("ReAlloc", static_cast<void (cConstCondVar::*)(uint, uint)>(&cConstCondVar::ReAlloc))
        .def("ReAllocVect", static_cast<void (cConstCondVar::*)(const cDVector&, uint)>(&cConstCondVar::ReAlloc))
        .def("SetValueIndex", static_cast<void (cConstCondVar::*)(double, uint, uint)>(&cConstCondVar::Set))
        .def("SetVectorIndex", static_cast<void (cConstCondVar::*)(const cDVector&, uint)>(&cConstCondVar::Set))
        .def("GetValueIndex", static_cast<double (cConstCondVar::*)(uint, uint)>(&cConstCondVar::Get))
        .def("GetVectorIndex", static_cast<cDVector & (cConstCondVar::*)(uint)>(&cConstCondVar::Get),
            return_value_policy<reference_existing_object>())
        .def("SetValueName", static_cast<void (cConstCondVar::*)(double, std::string, uint)>(&cConstCondVar::Set))
        .def("SetVectorName", static_cast<void (cConstCondVar::*)(const cDVector&, std::string)>(&cConstCondVar::Set))
        .def("GetValueName", static_cast<double (cConstCondVar::*)(std::string, uint)>(&cConstCondVar::Get))
        .def("GetVectorName", static_cast<cDVector & (cConstCondVar::*)(std::string)>(&cConstCondVar::Get),
            return_value_policy<reference_existing_object>())
        .def("SyncSizeParam", &cConstCondVar::SyncSizeParam)
        .def("ReAllocProxyVarParameters", &cConstCondVar::ReAllocProxyVarParameters)
        .def("UpdateProxyVarParameters", &cConstCondVar::UpdateProxyVarParameters)
        .def("ComputeVar", &cConstCondVar::ComputeVar)
        .def("GetNParam", &cConstCondVar::GetNParam)
        .def("GetNLags", &cConstCondVar::GetNLags)
        .def("ComputeGrad", &cConstCondVar::ComputeGrad)
        .def("RegArchParamToVector", &cConstCondVar::RegArchParamToVector)
        .def("VectorToRegArchParam", &cConstCondVar::VectorToRegArchParam)
        .def("ComputeHess", &cConstCondVar::ComputeHess)
        .def("ComputeGradAndHess", &cConstCondVar::ComputeGradAndHess)
        .def("GetParamNameChar", static_cast<void (cConstCondVar::*)(uint, char**)>(&cConstCondVar::GetParamName))
        .def("GetParamNameString", static_cast<void (cConstCondVar::*)(uint, std::string[])>(&cConstCondVar::GetParamName))

        // Assignment operator
        .def("__assign__", &cConstCondVar::operator=, return_internal_reference<>(),
            "Assignment operator - copies another constant variance model.")

        // ----------------------------------------------------------------
        // NEW: Unified "set", "get", "realloc" that handle Python lists
        // ----------------------------------------------------------------
        .def("set", &cConstCondVarSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to set parameters.\n\n"
            "Examples:\n"
            "  cvar.set(2.0, 0)             # single float to param index\n"
            "  cvar.set([2.0], 0)           # python list with one float\n"
            "  cvar.set(5.0, 'const', 0)    # by name + subindex"
        )
        .def("get", &cConstCondVarGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to get parameters.\n\n"
            "Examples:\n"
            "  val = cvar.get(0)            # gets a cDVector if there's more than 1 param\n"
            "  val2 = cvar.get('const', 0)  # single float\n"
        )
        .def("realloc", &cConstCondVarReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("num_param") = object()),
            "Unified method to reallocate parameters.\n\n"
            "Examples:\n"
            "  cvar.realloc(1)             # size=1\n"
            "  cvar.realloc([5.0])         # from python list\n"
            "  cvar.realloc(cDVector(...)) # from a cDVector"
        )
        ;
}
