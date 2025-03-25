#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// Include your helper that converts Python lists/tuples to cDVector
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// Helper for printing
static void cConstPrint(cConst& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "set" that accepts float or Python lists
// --------------------------------------------------------------------
static void cConstSet(cConst& self,
    const object& value_or_vector,
    const object& index_or_name,
    const object& num_param = object())
{
    // Attempt direct extraction as cDVector or double
    extract<cDVector> vec_extract(value_or_vector);
    extract<double>   val_extract(value_or_vector);

    // Check second argument
    extract<std::string> name_extract(index_or_name);
    extract<uint>        idx_extract(index_or_name);

    if (vec_extract.check())
    {
        // The user gave us a cDVector or a list that auto-converted
        cDVector vect = vec_extract();

        if (name_extract.check())
        {
            // self.Set(const cDVector&, std::string)
            self.Set(vect, name_extract());
        }
        else if (idx_extract.check())
        {
            // self.Set(const cDVector&, uint)
            uint param_num = 0;
            if (!num_param.is_none())
                param_num = extract<uint>(num_param);
            self.Set(vect, param_num);
        }
        else
        {
            throw std::runtime_error(
                "cConst.set(...) usage: second arg must be string or uint when passing a vector.");
        }
    }
    else if (val_extract.check())
    {
        // The user gave us a single float/double
        double val = val_extract();

        if (name_extract.check())
        {
            // self.Set(double, std::string, uint)
            std::string param_name = name_extract();
            uint idx = 0;
            if (!num_param.is_none())
                idx = extract<uint>(num_param);
            self.Set(val, param_name, idx);
        }
        else if (idx_extract.check())
        {
            // self.Set(double, uint, uint)
            uint i = idx_extract();
            uint param_num = 0;
            if (!num_param.is_none())
                param_num = extract<uint>(num_param);
            self.Set(val, i, param_num);
        }
        else
        {
            throw std::runtime_error(
                "cConst.set(...) usage: second arg must be string or uint when passing a float.");
        }
    }
    else
    {
        // Fallback if it's a Python list/tuple not auto-converted
        if (PyList_Check(value_or_vector.ptr()) || PyTuple_Check(value_or_vector.ptr()))
        {
            cDVector vect = py_list_or_tuple_to_cDVector(value_or_vector);

            if (name_extract.check())
            {
                self.Set(vect, name_extract());
            }
            else if (idx_extract.check())
            {
                uint param_num = 0;
                if (!num_param.is_none())
                    param_num = extract<uint>(num_param);
                self.Set(vect, param_num);
            }
            else
            {
                throw std::runtime_error(
                    "cConst.set(...) usage: second arg must be string or uint when passing a Python list/tuple.");
            }
        }
        else
        {
            throw std::runtime_error(
                "First argument must be float, cDVector, or Python list/tuple for cConst.set(...).");
        }
    }
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "get" that returns float or cDVector
// --------------------------------------------------------------------
static object cConstGet(cConst& self,
    const object& index_or_name,
    const object& num_param = object())
{
    extract<std::string> name_extract(index_or_name);
    extract<uint>        idx_extract(index_or_name);

    if (name_extract.check())
    {
        // The user passed a param name
        std::string pname = name_extract();

        // If user gave no second argument => cDVector& version
        if (num_param.is_none())
        {
            // Potentially ambiguous if you have both:
            //  - cDVector& Get(std::string)
            //  - double Get(std::string, uint=0)
            // If ambiguous, see pointer-to-member approach below
            return object(boost::ref(self.Get(pname)));
        }
        else
        {
            // user gave an index => double version
            uint idx = extract<uint>(num_param);
            return object(self.Get(pname, idx));
        }
    }
    else if (idx_extract.check())
    {
        // The user passed a param index
        uint i = idx_extract();

        if (num_param.is_none())
        {
            // cDVector& version => Get(uint)
            return object(boost::ref(self.Get(i)));
        }
        else
        {
            // double version => Get(uint, uint)
            uint sub_idx = extract<uint>(num_param);
            return object(self.Get(i, sub_idx));
        }
    }
    else
    {
        throw std::runtime_error(
            "cConst.get(...) usage: first argument must be a string or a uint.");
    }
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "realloc" that can accept size or python list
// --------------------------------------------------------------------
static void cConstReAlloc(cConst& self,
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
        else
        {
            throw std::runtime_error(
                "cConst.realloc(...) usage: pass a uint size, cDVector, or list/tuple of floats.");
        }
    }
}

void export_cConst()
{
    class_<cConst, bases<cAbstCondMean> >("cConst",
        "Constant mean component.\n\n"
        "This class implements a constant conditional mean model.\n"
        "It uses a single parameter representing the constant value.",
        init<optional<double> >(
            (boost::python::arg("value") = 0.0),
            "cConst(double value=0.0)\n\n"
            "Creates a constant mean model with the specified value."
        )
    )
        // Copy constructor
        .def(init<const cConst&>(
            boost::python::arg("other"),
            "cConst(cConst other)\n\n"
            "Copy constructor - creates a copy of another constant mean model."
        ))

        // Pythonic naming
        .def("delete", &cConst::Delete,
            "Releases memory used by the model.")
        .def("print", &cConstPrint,
            "Prints model parameters to standard output.")
        .def("set_default_init_point",
            static_cast<void (cConst::*)(double, double)>(&cConst::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Sets default initial parameter values based on mean and variance.")
        .def("set_default_init_point_value",
            static_cast<void (cConst::*)(cRegArchValue&)>(&cConst::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Sets default initial parameter values based on RegArch values.")

        // ReAlloc methods - Pythonic naming
        .def("realloc",
            static_cast<void (cConst::*)(uint, uint)>(&cConst::ReAlloc),
            (boost::python::arg("size"), boost::python::arg("param_num") = 0),
            "Reallocates memory for model parameters.")
        .def("realloc_vect",
            static_cast<void (cConst::*)(const cDVector&, uint)>(&cConst::ReAlloc),
            (boost::python::arg("vect_param"), boost::python::arg("param_num") = 0),
            "Reallocates memory for model parameters based on vector size.")

        // Set methods - Pythonic naming
        .def("set_value_index",
            static_cast<void (cConst::*)(double, uint, uint)>(&cConst::Set),
            (boost::python::arg("value"), boost::python::arg("index"), boost::python::arg("param_num")),
            "Sets a parameter value by numerical index.")
        .def("set_vector_index",
            static_cast<void (cConst::*)(const cDVector&, uint)>(&cConst::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_num")),
            "Sets parameters from a vector by numerical index.")

        // Get methods - Pythonic naming
        .def("get_value_index",
            static_cast<double (cConst::*)(uint, uint)>(&cConst::Get),
            (boost::python::arg("index"), boost::python::arg("param_num")),
            "Gets a parameter value by numerical index.")
        .def("get_vector_index",
            static_cast<cDVector & (cConst::*)(uint)>(&cConst::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_num"),
            "Gets parameters as a vector by numerical index.")

        // Named parameters - Pythonic naming
        .def("set_value_name",
            static_cast<void (cConst::*)(double, std::string, uint)>(&cConst::Set),
            (boost::python::arg("value"), boost::python::arg("param_name"), boost::python::arg("index") = 0),
            "Sets a parameter value by name.")
        .def("set_vector_name",
            static_cast<void (cConst::*)(const cDVector&, std::string)>(&cConst::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_name")),
            "Sets parameters from a vector by name.")
        .def("get_value_name",
            static_cast<double (cConst::*)(std::string, uint)>(&cConst::Get),
            (boost::python::arg("param_name"), boost::python::arg("index")),
            "Gets a parameter value by name.")
        .def("get_vector_name",
            static_cast<cDVector & (cConst::*)(std::string)>(&cConst::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Gets parameters as a vector by name.")

        // Core functionality - Pythonic naming
        .def("compute_mean", &cConst::ComputeMean,
            (boost::python::arg("date"), boost::python::arg("data")),
            "Computes the conditional mean at the specified date given the data.")
        .def("get_n_param", &cConst::GetNParam,
            "Returns the total number of parameters in the model.")
        .def("get_n_lags", &cConst::GetNLags,
            "Returns the number of lags required for computation.")
        .def("compute_grad", &cConst::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("beg_index")),
            "Computes the gradient of the model at the specified date.")
        .def("compute_hess", &cConst::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("beg_index")),
            "Computes the Hessian of the model at the specified date.")
        .def("compute_grad_and_hess", &cConst::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("beg_index")),
            "Computes both gradient and Hessian in one operation.")
        .def("reg_arch_param_to_vector", &cConst::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index")),
            "Converts model parameters to a vector representation.")
        .def("vector_to_reg_arch_param", &cConst::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Converts a vector representation back to model parameters.")
        .def("get_param_name_char",
            static_cast<void (cConst::*)(uint, char**)>(&cConst::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C-style string.")
        .def("get_param_name_string",
            static_cast<void (cConst::*)(uint, std::string[])>(&cConst::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C++ string.")

        // For backwards compatibility - Original C++ naming
        .def("Delete", &cConst::Delete)
        .def("Print", &cConstPrint)
        .def("SetDefaultInitPoint", static_cast<void (cConst::*)(double, double)>(&cConst::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue", static_cast<void (cConst::*)(cRegArchValue&)>(&cConst::SetDefaultInitPoint))
        .def("ReAlloc", static_cast<void (cConst::*)(uint, uint)>(&cConst::ReAlloc))
        .def("ReAllocVect", static_cast<void (cConst::*)(const cDVector&, uint)>(&cConst::ReAlloc))
        .def("SetValueIndex", static_cast<void (cConst::*)(double, uint, uint)>(&cConst::Set))
        .def("SetVectorIndex", static_cast<void (cConst::*)(const cDVector&, uint)>(&cConst::Set))
        .def("GetValueIndex", static_cast<double (cConst::*)(uint, uint)>(&cConst::Get))
        .def("GetVectorIndex", static_cast<cDVector & (cConst::*)(uint)>(&cConst::Get), return_value_policy<reference_existing_object>())
        .def("SetValueName", static_cast<void (cConst::*)(double, std::string, uint)>(&cConst::Set))
        .def("SetVectorName", static_cast<void (cConst::*)(const cDVector&, std::string)>(&cConst::Set))
        .def("GetValueName", static_cast<double (cConst::*)(std::string, uint)>(&cConst::Get))
        .def("GetVectorName", static_cast<cDVector & (cConst::*)(std::string)>(&cConst::Get), return_value_policy<reference_existing_object>())
        .def("ComputeMean", &cConst::ComputeMean)
        .def("GetNParam", &cConst::GetNParam)
        .def("GetNLags", &cConst::GetNLags)
        .def("ComputeGrad", &cConst::ComputeGrad)
        .def("ComputeHess", &cConst::ComputeHess)
        .def("ComputeGradAndHess", &cConst::ComputeGradAndHess)
        .def("RegArchParamToVector", &cConst::RegArchParamToVector)
        .def("VectorToRegArchParam", &cConst::VectorToRegArchParam)
        .def("GetParamNameChar", static_cast<void (cConst::*)(uint, char**)>(&cConst::GetParamName))
        .def("GetParamNameString", static_cast<void (cConst::*)(uint, std::string[])>(&cConst::GetParamName))

        // Assignment operator
        .def("__assign__", &cConst::operator=, return_internal_reference<>(),
            "Assignment operator - copies another constant mean model.")

        // ----------------------------------------------------------------
        // NEW: Unified "set", "get", "realloc" that handle python lists
        // ----------------------------------------------------------------
        .def("set", &cConstSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to set parameters (float or list) by name or index.\n\n"
            "Examples:\n"
            "  cvar.set(2.0, 0)             # single float param by index\n"
            "  cvar.set([2.0], 0)           # from python list\n"
            "  cvar.set(5.0, 'const', 0)    # by name"
        )
        .def("get", &cConstGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to get parameters as float or cDVector.\n\n"
            "Examples:\n"
            "  val = cvar.get(0)            # cDVector if multiple params\n"
            "  val2 = cvar.get(0, 1)        # single float\n"
            "  val3 = cvar.get('const')     # cDVector or float if only one param\n"
        )
        .def("realloc", &cConstReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("num_param") = object()),
            "Unified method to reallocate parameters (size or python list).\n\n"
            "Examples:\n"
            "  cvar.realloc(1)             # reallocate size=1\n"
            "  cvar.realloc([5.0])         # from python list\n"
        )
        ;
}
