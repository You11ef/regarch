#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// 1) Include your helper header that provides py_list_or_tuple_to_cDVector(...)
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// Printing helper
static void Gtarch_Print(cGtarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// --------------------------------------------------------------------
//  NEW: Helper for a unified "set" that accepts Python lists
// --------------------------------------------------------------------
static void cGtarchSet(cGtarch& self, const object& value_or_vector, const object& index_or_name, const object& num_param = object())
{
    extract<cDVector> vec_extract(value_or_vector);
    extract<double>   val_extract(value_or_vector);

    // Check if second argument is a string or a uint
    extract<std::string> name_extract(index_or_name);
    extract<uint>        idx_extract(index_or_name);

    if (vec_extract.check())
    {
        // The user passed a cDVector
        cDVector vect = vec_extract();

        if (name_extract.check()) {
            // self.Set(cDVector, std::string)
            self.Set(vect, name_extract());
        }
        else if (idx_extract.check()) {
            uint param_num = 0;
            if (!num_param.is_none())
                param_num = extract<uint>(num_param);
            // self.Set(cDVector, uint)
            self.Set(vect, param_num);
        }
        else {
            throw std::runtime_error("Invalid usage for cGtarch.set(...) with a cDVector. Provide param name (string) or param index (uint).");
        }
    }
    else if (val_extract.check())
    {
        // The user passed a scalar (double)
        double val = val_extract();

        if (name_extract.check()) {
            // self.Set(double, std::string, uint)
            uint idx = 0;
            if (!num_param.is_none())
                idx = extract<uint>(num_param);
            self.Set(val, name_extract(), idx);
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
            throw std::runtime_error("Invalid usage for cGtarch.set(...) with a float. Provide param name (string) or param index (uint).");
        }
    }
    else
    {
        // Fallback for Python list/tuple
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
                throw std::runtime_error("Invalid usage for cGtarch.set(...) with a Python list/tuple. Provide param name or index.");
            }
        }
        else {
            throw std::runtime_error("First arg must be float, cDVector, or a Python list/tuple of floats for cGtarch.set(...).");
        }
    }
}

// --------------------------------------------------------------------
//  NEW: Helper for a unified "get" that can retrieve param by name or index
// --------------------------------------------------------------------
static object cGtarchGet(cGtarch& self, const object& index_or_name, const object& num_param = object())
{
    extract<std::string> name_extract(index_or_name);
    extract<uint>        idx_extract(index_or_name);

    if (name_extract.check())
    {
        // The user passed a param name
        std::string pname = name_extract();

        if (num_param.is_none()) {
            // Get(std::string) -> cDVector&
            return object(boost::ref(self.Get(pname)));
        }
        else {
            // Get(std::string, uint) -> double
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
        throw std::runtime_error("First argument must be a param name (string) or a param index (uint) for cGtarch.get(...).");
    }
}

// --------------------------------------------------------------------
//  NEW: Helper for a unified "realloc" that can accept a list or cDVector
// --------------------------------------------------------------------
static void cGtarchReAlloc(cGtarch& self, const object& size_or_vector, const object& num_param = object())
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
            throw std::runtime_error("First argument must be uint, cDVector, or a Python list/tuple of floats for cGtarch.realloc(...).");
        }
    }
}

// --------------------------------------------------------------------
// The export function with minimal changes
// --------------------------------------------------------------------
void export_cGtarch()
{
    class_<cGtarch, bases<cAbstCondVar> >("cGtarch",
        "GTARCH(p,q) model with asymmetry for positive/negative shocks.",
        init< optional<uint, uint> >(
            (boost::python::arg("theNArch") = 0, boost::python::arg("theNGarch") = 0),
            "cGtarch(uint p=0, uint q=0)"
        )
    )
        .def("Delete", &cGtarch::Delete)
        .def("Print", &Gtarch_Print)
        .def("SetDefaultInitPoint",
            static_cast<void (cGtarch::*)(double, double)>(&cGtarch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cGtarch::*)(cRegArchValue&)>(&cGtarch::SetDefaultInitPoint))
        .def("ReAlloc",
            static_cast<void (cGtarch::*)(uint, uint)>(&cGtarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cGtarch::*)(const cDVector&, uint)>(&cGtarch::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cGtarch::*)(double, uint, uint)>(&cGtarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cGtarch::*)(const cDVector&, uint)>(&cGtarch::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cGtarch::*)(uint, uint)>(&cGtarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cGtarch::*)(uint)>(&cGtarch::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get by name
        .def("SetValueName",
            static_cast<void (cGtarch::*)(double, std::string, uint)>(&cGtarch::Set))
        .def("SetVectorName",
            static_cast<void (cGtarch::*)(const cDVector&, std::string)>(&cGtarch::Set))
        .def("GetValueName",
            static_cast<double (cGtarch::*)(std::string, uint)>(&cGtarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cGtarch::*)(std::string)>(&cGtarch::Get),
            return_value_policy<reference_existing_object>())

        // Existing GTARCH methods
        .def("ComputeVar", &cGtarch::ComputeVar)
        .def("GetNParam", &cGtarch::GetNParam)
        .def("GetNLags", &cGtarch::GetNLags)
        .def("ComputeGrad", &cGtarch::ComputeGrad)
        .def("RegArchParamToVector", &cGtarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cGtarch::VectorToRegArchParam)
        .def("ComputeHess", &cGtarch::ComputeHess)
        .def("ComputeGradAndHess", &cGtarch::ComputeGradAndHess)
        .def("SyncSizeParam", &cGtarch::SyncSizeParam)

        .def("GetParamNameChar",
            static_cast<void (cGtarch::*)(uint, char**)>(&cGtarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cGtarch::*)(uint, std::string[])>(&cGtarch::GetParamName))

        // -------------------------------------------------------------
        // NEW: Unified "set", "get", "realloc" that accept Python lists
        // -------------------------------------------------------------
        .def("set", &cGtarchSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to set parameters.\n\n"
            "Examples:\n"
            "  gtarch.set(0.1, 0)          # Single float to param index\n"
            "  gtarch.set([0.1, 0.2], 0)   # Python list of floats\n"
            "  gtarch.set(0.1, 'alpha', 1) # By name\n"
        )
        .def("get", &cGtarchGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to get parameters.\n\n"
            "Examples:\n"
            "  v = gtarch.get(0)          # cDVector\n"
            "  x = gtarch.get(0, 1)       # double\n"
            "  alpha = gtarch.get('alpha')\n"
            "  val = gtarch.get('alpha', 1)\n"
        )
        .def("realloc", &cGtarchReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("num_param") = object()),
            "Unified method to reallocate parameters.\n\n"
            "Examples:\n"
            "  gtarch.realloc(5)                # specify size\n"
            "  gtarch.realloc([0.1, 0.2, 0.3])  # python list\n"
            "  gtarch.realloc(cDVector(...))    # cDVector"
        )
        ;
}
