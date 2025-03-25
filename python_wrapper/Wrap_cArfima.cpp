#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// 1) Include your helper header that provides py_list_or_tuple_to_cDVector(...)
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// ---------------------------------------------------------------------
// NEW HELPER: cArfimaSet for unified Python-style parameter setting
// ---------------------------------------------------------------------
static void cArfimaSet(cArfima& self, const object& arg1, const object& arg2, const object& arg3 = object())
{
    // Attempt direct extraction as cDVector or double
    extract<cDVector> vec_extract(arg1);
    extract<double>   val_extract(arg1);

    // Check second argument for string (param name) or uint (param index)
    extract<std::string> name_extract(arg2);
    extract<uint>        index_extract(arg2);

    if (vec_extract.check())
    {
        // The user passed a cDVector
        cDVector vector = vec_extract();

        if (name_extract.check()) {
            // e.g. set(vector, "ar") or set(vector, "ma"), etc.
            self.Set(vector, name_extract());
        }
        else if (index_extract.check()) {
            // e.g. set(vector, 0)
            uint param_num = 0;
            if (!arg3.is_none())
                param_num = extract<uint>(arg3);
            self.Set(vector, param_num);
        }
        else {
            throw std::runtime_error("Second argument must be string (param name) or uint (index) when first arg is a cDVector.");
        }
    }
    else if (val_extract.check())
    {
        // The user passed a single scalar (double)
        double value = val_extract();

        if (name_extract.check()) {
            // e.g. set(0.5, "ar", 0)
            uint idx = 0;
            if (!arg3.is_none())
                idx = extract<uint>(arg3);
            self.Set(value, name_extract(), idx);
        }
        else if (index_extract.check()) {
            // e.g. set(0.5, 0, 2)
            uint idx = index_extract();
            uint param_num = 0;
            if (!arg3.is_none())
                param_num = extract<uint>(arg3);
            self.Set(value, idx, param_num);
        }
        else {
            throw std::runtime_error("Second argument must be string (param name) or uint (index) when first arg is a double.");
        }
    }
    else
    {
        // Fallback if the user passed a Python list or tuple
        if (PyList_Check(arg1.ptr()) || PyTuple_Check(arg1.ptr()))
        {
            cDVector vector = py_list_or_tuple_to_cDVector(arg1);

            if (name_extract.check()) {
                // e.g. set([0.1, 0.2], "ar")
                self.Set(vector, name_extract());
            }
            else if (index_extract.check()) {
                // e.g. set([0.1, 0.2], 0)
                uint param_num = 0;
                if (!arg3.is_none())
                    param_num = extract<uint>(arg3);
                self.Set(vector, param_num);
            }
            else {
                throw std::runtime_error("Second argument must be string or uint when passing a Python list/tuple.");
            }
        }
        else {
            throw std::runtime_error("First argument must be float, cDVector, or a Python list/tuple of floats.");
        }
    }
}

// ---------------------------------------------------------------------
// NEW HELPER: cArfimaGet for unified Python-style parameter getting
// ---------------------------------------------------------------------
static object cArfimaGet(cArfima& self, const object& arg1, const object& arg2 = object())
{
    // Check if first argument is a string or an index
    extract<std::string> name_extract(arg1);
    extract<uint>        idx_extract(arg1);

    if (name_extract.check())
    {
        // e.g. get("ar") or get("ar", 0)
        std::string param_name = name_extract();

        if (arg2.is_none()) {
            // Return the entire vector => cDVector
            return object(boost::ref(self.Get(param_name)));
        }
        else {
            // Return a single double
            uint sub_idx = extract<uint>(arg2);
            return object(self.Get(param_name, sub_idx));
        }
    }
    else if (idx_extract.check())
    {
        // e.g. get(0) or get(0, 1)
        uint param_num = idx_extract();
        if (arg2.is_none()) {
            // Return a cDVector
            return object(boost::ref(self.Get(param_num)));
        }
        else {
            // Return a double
            uint sub_idx = extract<uint>(arg2);
            return object(self.Get(sub_idx, param_num));
        }
    }
    else {
        throw std::runtime_error("First argument must be a string (param name) or uint (param index).");
    }
}

// ---------------------------------------------------------------------
// NEW HELPER: cArfimaReAlloc for unified Python-style reallocation
// ---------------------------------------------------------------------
static void cArfimaReAlloc(cArfima& self, const object& arg1, const object& arg2 = object())
{
    // Try to extract a cDVector or uint from the first argument
    extract<cDVector> vec_extract(arg1);
    extract<uint>     uint_extract(arg1);

    uint param_num = 0;
    if (!arg2.is_none())
        param_num = extract<uint>(arg2);

    if (vec_extract.check())
    {
        // ReAlloc(cDVector, uint)
        cDVector vect = vec_extract();
        self.ReAlloc(vect, param_num);
    }
    else if (uint_extract.check())
    {
        // ReAlloc(uint, uint)
        uint size = uint_extract();
        self.ReAlloc(size, param_num);
    }
    else
    {
        // Fallback for Python list or tuple
        if (PyList_Check(arg1.ptr()) || PyTuple_Check(arg1.ptr()))
        {
            cDVector vect = py_list_or_tuple_to_cDVector(arg1);
            self.ReAlloc(vect, param_num);
        }
        else {
            throw std::runtime_error("First argument must be a uint, cDVector, or a Python list/tuple of floats.");
        }
    }
}

// ---------------------------------------------------------------------
// Original code from your snippet, unchanged
// ---------------------------------------------------------------------
static void cArfimaPrint(cArfima& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cArfima()
{
    class_<cArfima, bases<cAbstCondMean> >("cArfima",
        "ARFIMA(p,d,q) conditional mean model.\n\n"
        "The ARFIMA model combines autoregressive (AR), fractionally integrated (FI), and moving average (MA) components:\n"
        "  - p: AR order\n"
        "  - d: Fractional differencing parameter (-0.5 < d < 0.5)\n"
        "  - q: MA order\n\n"
        "This model is useful for time series with long memory properties.",
        init< optional<uint, uint, double, uint> >(
            (boost::python::arg("n_ar") = 0, boost::python::arg("n_ma") = 0,
                boost::python::arg("frac_d") = 0.0, boost::python::arg("n_trunc_lag") = 20),
            "cArfima(uint n_ar=0, uint n_ma=0, double frac_d=0.0, uint n_trunc_lag=20)\n\n"
            "Creates an ARFIMA(p,d,q) model with specified orders and fractional parameter."
        )
    )
        // Additional constructors
        .def(init<const cDVector&, const cDVector&, double, optional<uint> >(
            (boost::python::arg("ar_coeffs"), boost::python::arg("ma_coeffs"),
                boost::python::arg("frac_d"), boost::python::arg("n_trunc_lag") = 20),
            "cArfima(cDVector ar_coeffs, cDVector ma_coeffs, double frac_d, uint n_trunc_lag=20)\n\n"
            "Creates an ARFIMA model with directly specified AR and MA coefficients."
        ))
        .def(init<const cArfima&>(
            boost::python::arg("other"),
            "cArfima(cArfima other)\n\n"
            "Copy constructor - creates a copy of another ARFIMA model."
        ))
        // Basic housekeeping
        .def("delete", &cArfima::Delete,
            "Releases memory used by the model.")
        .def("delete_poly", &cArfima::DeletePoly,
            "Releases memory used by polynomial representations.")
        .def("print", &cArfimaPrint,
            "Prints model parameters to standard output.")
        // SetDefaultInitPoint methods - Pythonic naming
        .def("set_default_init_point",
            static_cast<void (cArfima::*)(double, double)>(&cArfima::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Sets default initial parameter values based on mean and variance.")
        .def("set_default_init_point",
            static_cast<void (cArfima::*)(cRegArchValue&)>(&cArfima::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Sets default initial parameter values based on RegArch values.")
        // ReAlloc methods - Pythonic naming
        .def("realloc",
            static_cast<void (cArfima::*)(uint, uint)>(&cArfima::ReAlloc),
            (boost::python::arg("size"), boost::python::arg("param_num") = 0),
            "Reallocates memory for model parameters.")
        .def("realloc_vect",
            static_cast<void (cArfima::*)(const cDVector&, uint)>(&cArfima::ReAlloc),
            (boost::python::arg("vect_param"), boost::python::arg("param_num") = 0),
            "Reallocates memory for model parameters based on vector size.")
        // Set methods - Pythonic naming
        .def("set_value_index",
            static_cast<void (cArfima::*)(double, uint, uint)>(&cArfima::Set),
            (boost::python::arg("value"), boost::python::arg("index"), boost::python::arg("param_num")),
            "Sets a parameter value by numerical index.")
        .def("set_vector_index",
            static_cast<void (cArfima::*)(const cDVector&, uint)>(&cArfima::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_num")),
            "Sets parameters from a vector by numerical index.")
        // Get methods - Pythonic naming
        .def("get_value_index",
            static_cast<double (cArfima::*)(uint, uint)>(&cArfima::Get),
            (boost::python::arg("index"), boost::python::arg("param_num")),
            "Gets a parameter value by numerical index.")
        .def("get_vector_index",
            static_cast<cDVector & (cArfima::*)(uint)>(&cArfima::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_num"),
            "Gets parameters as a vector by numerical index.")
        // Named parameters - Pythonic naming
        .def("set_value_name",
            static_cast<void (cArfima::*)(double, std::string, uint)>(&cArfima::Set),
            (boost::python::arg("value"), boost::python::arg("param_name"), boost::python::arg("index")),
            "Sets a parameter value by name.")
        .def("set_vector_name",
            static_cast<void (cArfima::*)(const cDVector&, std::string)>(&cArfima::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_name")),
            "Sets parameters from a vector by name.")
        .def("get_value_name",
            static_cast<double (cArfima::*)(std::string, uint)>(&cArfima::Get),
            (boost::python::arg("param_name"), boost::python::arg("index")),
            "Gets a parameter value by name.")
        .def("get_vector_name",
            static_cast<cDVector & (cArfima::*)(std::string)>(&cArfima::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Gets parameters as a vector by name.")
        // For backwards compatibility
        .def("Delete", &cArfima::Delete)
        .def("DeletePoly", &cArfima::DeletePoly)
        .def("Print", &cArfimaPrint)
        .def("SetDefaultInitPoint", static_cast<void (cArfima::*)(double, double)>(&cArfima::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue", static_cast<void (cArfima::*)(cRegArchValue&)>(&cArfima::SetDefaultInitPoint))
        .def("ReAlloc", static_cast<void (cArfima::*)(uint, uint)>(&cArfima::ReAlloc))
        .def("ReAllocVect", static_cast<void (cArfima::*)(const cDVector&, uint)>(&cArfima::ReAlloc))
        .def("SetValueIndex", static_cast<void (cArfima::*)(double, uint, uint)>(&cArfima::Set))
        .def("SetVectorIndex", static_cast<void (cArfima::*)(const cDVector&, uint)>(&cArfima::Set))
        .def("GetValueIndex", static_cast<double (cArfima::*)(uint, uint)>(&cArfima::Get))
        .def("GetVectorIndex", static_cast<cDVector & (cArfima::*)(uint)>(&cArfima::Get), return_value_policy<reference_existing_object>())
        .def("SetValueName", static_cast<void (cArfima::*)(double, std::string, uint)>(&cArfima::Set))
        .def("SetVectorName", static_cast<void (cArfima::*)(const cDVector&, std::string)>(&cArfima::Set))
        .def("GetValueName", static_cast<double (cArfima::*)(std::string, uint)>(&cArfima::Get))
        .def("GetVectorName", static_cast<cDVector & (cArfima::*)(std::string)>(&cArfima::Get), return_value_policy<reference_existing_object>())
        // Core functionality - Pythonic naming
        .def("compute_mean", &cArfima::ComputeMean,
            (boost::python::arg("date"), boost::python::arg("data")),
            "Computes the conditional mean at the specified date given the data.")
        .def("get_n_param", &cArfima::GetNParam,
            "Returns the total number of parameters in the model.")
        .def("get_n_lags", &cArfima::GetNLags,
            "Returns the number of lags required for computation.")
        .def("get_n_ar", &cArfima::GetNAr,
            "Returns the number of AR parameters (p).")
        .def("get_n_ma", &cArfima::GetNMa,
            "Returns the number of MA parameters (q).")
        .def("compute_grad", &cArfima::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("beg_index")),
            "Computes the gradient of the model at the specified date.")
        .def("compute_hess", &cArfima::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("beg_index")),
            "Computes the Hessian of the model at the specified date.")
        .def("compute_grad_and_hess", &cArfima::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("beg_index")),
            "Computes both gradient and Hessian in one operation.")
        .def("reg_arch_param_to_vector", &cArfima::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index") = 0),
            "Converts model parameters to a vector representation.")
        .def("vector_to_reg_arch_param", &cArfima::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Converts a vector representation back to model parameters.")
        // Core functionality - Original naming
        .def("ComputeMean", &cArfima::ComputeMean)
        .def("GetNParam", &cArfima::GetNParam)
        .def("GetNLags", &cArfima::GetNLags)
        .def("GetNAr", &cArfima::GetNAr)
        .def("GetNMa", &cArfima::GetNMa)
        .def("ComputeGrad", &cArfima::ComputeGrad)
        .def("ComputeHess", &cArfima::ComputeHess)
        .def("ComputeGradAndHess", &cArfima::ComputeGradAndHess)
        .def("RegArchParamToVector", &cArfima::RegArchParamToVector)
        .def("VectorToRegArchParam", &cArfima::VectorToRegArchParam)
        // Proxy parameters - Pythonic naming
        .def("realloc_proxy_mean_parameters", &cArfima::ReAllocProxyMeanParameters,
            (boost::python::arg("old_n_param") = 0),
            "Reallocates proxy mean parameters.")
        .def("update_proxy_mean_parameters", &cArfima::UpdateProxyMeanParameters,
            "Updates proxy mean parameters.")
        // ReSizePoly - Added Pythonic & original naming
        .def("resize_poly", &cArfima::ReSizePoly,
            boost::python::arg("n_param"),
            "Resizes polynomial representations to accommodate the given number of parameters.")
        .def("ReSizePoly", &cArfima::ReSizePoly)
        // Proxy parameters - Original naming
        .def("ReAllocProxyMeanParameters", &cArfima::ReAllocProxyMeanParameters)
        .def("UpdateProxyMeanParameters", &cArfima::UpdateProxyMeanParameters)
        // Parameter name getters
        .def("get_param_name_char",
            static_cast<void (cArfima::*)(uint, char**)>(&cArfima::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C-style string.")
        .def("get_param_name_string",
            static_cast<void (cArfima::*)(uint, std::string[])>(&cArfima::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C++ string.")
        .def("GetParamNameChar", static_cast<void (cArfima::*)(uint, char**)>(&cArfima::GetParamName))
        .def("GetParamNameString", static_cast<void (cArfima::*)(uint, std::string[])>(&cArfima::GetParamName))
        // Assignment operator
        .def("__assign__", &cArfima::operator=, return_internal_reference<>(),
            "Assignment operator - copies another ARFIMA model.")

        // ----------------------------------------------------------------
        // NEW .def calls for the unified "set", "get", and "realloc"
        // that handle Python lists/tuples automatically
        // ----------------------------------------------------------------
        .def("set", &cArfimaSet,
            (boost::python::arg("arg1"), boost::python::arg("arg2"), boost::python::arg("arg3") = object()),
            "Unified method to set parameters.\n\n"
            "Examples:\n"
            "  set(value, index, param_num)\n"
            "  set(value, name, index)\n"
            "  set(list_of_floats, index)  # Python list -> cDVector\n"
            "  set(cDVector, name)\n")
        .def("get", &cArfimaGet,
            (boost::python::arg("arg1"), boost::python::arg("arg2") = object()),
            "Unified method to get parameters.\n\n"
            "Examples:\n"
            "  get(index)\n"
            "  get(index, sub_idx)\n"
            "  get(name)\n"
            "  get(name, sub_idx)\n\n"
            "Returns cDVector or float, depending on usage.")
        .def("realloc", &cArfimaReAlloc,
            (boost::python::arg("arg1"), boost::python::arg("arg2") = object()),
            "Unified reallocation method.\n\n"
            "Examples:\n"
            "  realloc(5)            # ReAlloc(uint)\n"
            "  realloc([0.1, 0.2])   # ReAlloc(cDVector) from Python list\n"
            "  realloc(cDVector, paramNum)\n"
        )
        ;
}
