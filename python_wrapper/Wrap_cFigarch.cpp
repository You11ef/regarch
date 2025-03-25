#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// 1) Include your helper header for py_list_or_tuple_to_cDVector(...)
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// Existing helper for printing
static void Figarch_Print(cFigarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "set" that accepts float or Python lists
// --------------------------------------------------------------------
static void cFigarchSet(cFigarch& self,
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
        // The user passed a cDVector or a Python list that can auto-convert to cDVector
        cDVector vect = vec_extract();

        if (name_extract.check()) {
            // Set(cDVector, string)
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
            throw std::runtime_error("Invalid usage for cFigarch.set(...): second argument must be string or uint when passing a cDVector.");
        }
    }
    else if (val_extract.check())
    {
        // The user passed a single float
        double val = val_extract();

        if (name_extract.check()) {
            // Set(double, string, uint)
            uint idx = 0;
            if (!num_param.is_none())
                idx = extract<uint>(num_param);
            self.Set(val, name_extract(), idx);
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
            throw std::runtime_error("Invalid usage for cFigarch.set(...): second argument must be string or uint when passing a float.");
        }
    }
    else
    {
        // Fallback for a Python list/tuple that didn't auto-convert
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
                throw std::runtime_error("Invalid usage for cFigarch.set(...): second arg must be string or uint when passing a Python list/tuple.");
            }
        }
        else {
            throw std::runtime_error("First argument must be float, cDVector, or a Python list/tuple of floats for cFigarch.set(...).");
        }
    }
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "get" that returns float or cDVector
// --------------------------------------------------------------------
static object cFigarchGet(cFigarch& self,
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
        throw std::runtime_error("First argument must be a param name (string) or a param index (uint) for cFigarch.get(...).");
    }
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "realloc" that can accept size or python list
// --------------------------------------------------------------------
static void cFigarchReAlloc(cFigarch& self,
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
        cDVector vect = vec_extract();
        // ReAlloc(cDVector, uint)
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
            throw std::runtime_error("First argument must be uint, cDVector, or a Python list/tuple of floats for cFigarch.realloc(...).");
        }
    }
}

// --------------------------------------------------------------------
// The export function with minimal new additions
// --------------------------------------------------------------------
void export_cFigarch()
{
    class_<cFigarch, bases<cAbstCondVar> >("cFigarch",
        "FIGARCH(p,d,q) conditional variance model.",
        init< optional<uint, uint, double, uint> >(
            (boost::python::arg("theNArch") = 0,
                boost::python::arg("theNGarch") = 0,
                boost::python::arg("theFracD") = 0.0,
                boost::python::arg("theNTruncLag") = 20),
            "cFigarch(uint p=0, uint q=0, double d=0.0, uint nTrunc=20)"
        )
    )
        .def("Delete", &cFigarch::Delete)
        .def("Print", &Figarch_Print)

        .def("SetDefaultInitPoint",
            static_cast<void (cFigarch::*)(double, double)>(&cFigarch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cFigarch::*)(cRegArchValue&)>(&cFigarch::SetDefaultInitPoint))

        // Overloaded ReAlloc
        .def("ReAlloc",
            static_cast<void (cFigarch::*)(uint, uint)>(&cFigarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cFigarch::*)(const cDVector&, uint)>(&cFigarch::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cFigarch::*)(double, uint, uint)>(&cFigarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cFigarch::*)(const cDVector&, uint)>(&cFigarch::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cFigarch::*)(uint, uint)>(&cFigarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cFigarch::*)(uint)>(&cFigarch::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get with string
        .def("SetValueName",
            static_cast<void (cFigarch::*)(double, std::string, uint)>(&cFigarch::Set))
        .def("SetVectorName",
            static_cast<void (cFigarch::*)(const cDVector&, std::string)>(&cFigarch::Set))
        .def("GetValueName",
            static_cast<double (cFigarch::*)(std::string, uint)>(&cFigarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cFigarch::*)(std::string)>(&cFigarch::Get),
            return_value_policy<reference_existing_object>())

        .def("ComputeVar", &cFigarch::ComputeVar)
        .def("GetNParam", &cFigarch::GetNParam)
        .def("GetNLags", &cFigarch::GetNLags)
        .def("RegArchParamToVector", &cFigarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cFigarch::VectorToRegArchParam)
        .def("ComputeGrad", &cFigarch::ComputeGrad)
        .def("ComputeHess", &cFigarch::ComputeHess)
        .def("ComputeGradAndHess", &cFigarch::ComputeGradAndHess)

        // ReAllocProxyVarParameters, UpdateProxyVarParameters
        .def("ReAllocProxyVarParameters", &cFigarch::ReAllocProxyVarParameters)
        .def("UpdateProxyVarParameters", &cFigarch::UpdateProxyVarParameters)

        .def("GetParamNameChar",
            static_cast<void (cFigarch::*)(uint, char**)>(&cFigarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cFigarch::*)(uint, std::string[])>(&cFigarch::GetParamName))

        // ----------------------------------------------------------------
        // NEW: Unified "set", "get", "realloc" that handle python lists
        // ----------------------------------------------------------------
        .def("set", &cFigarchSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to set FIGARCH parameters.\n\n"
            "Examples:\n"
            "  fig.set(0.1, 0)            # single float to param index\n"
            "  fig.set([0.1,0.2], 0)      # python list for multiple parameters\n"
            "  fig.set(0.2, 'phi', 1)     # by name\n"
        )
        .def("get", &cFigarchGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to get FIGARCH parameters.\n\n"
            "Examples:\n"
            "  val = fig.get(0, 1)        # single param as float\n"
            "  vect = fig.get(0)          # entire param vector\n"
            "  val2 = fig.get('phi', 1)   # param by name\n"
        )
        .def("realloc", &cFigarchReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("num_param") = object()),
            "Unified method to reallocate FIGARCH parameters.\n\n"
            "Examples:\n"
            "  fig.realloc(3)                 # reallocate with size=3\n"
            "  fig.realloc([0.1, 0.2, 0.3])   # from a python list\n"
            "  fig.realloc(cDVector(...))     # from a cDVector"
        )
        ;
}
