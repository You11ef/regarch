#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// 1) Include your helper header that provides py_list_or_tuple_to_cDVector(...)
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// Existing helper for printing
static void Garch_Print(cGarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "set" that accepts float or python lists
// --------------------------------------------------------------------
static void cGarchSet(cGarch& self,
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
        // The user passed a cDVector (or Python list automatically convertible to cDVector)
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
            throw std::runtime_error("Invalid usage: second argument must be string or uint when passing a cDVector.");
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
            throw std::runtime_error("Invalid usage: second argument must be string or uint when passing a float.");
        }
    }
    else
    {
        // Fallback for a Python list/tuple
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
                throw std::runtime_error("Invalid usage: second argument must be string or uint when passing a Python list/tuple.");
            }
        }
        else {
            throw std::runtime_error("First argument must be a float, cDVector, or Python list/tuple of floats for cGarch.set(...).");
        }
    }
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "get" that returns float or cDVector
// --------------------------------------------------------------------
static object cGarchGet(cGarch& self,
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
            uint param_num = extract<uint>(num_param);
            return object(self.Get(idx, param_num));
        }
    }
    else {
        throw std::runtime_error("First argument must be a param name (string) or a param index (uint) for cGarch.get(...).");
    }
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "realloc" that can accept a python list
// --------------------------------------------------------------------
static void cGarchReAlloc(cGarch& self,
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
            throw std::runtime_error("First argument must be uint, cDVector, or a list/tuple of floats for cGarch.realloc(...).");
        }
    }
}

// --------------------------------------------------------------------
// The export function with minimal new additions
// --------------------------------------------------------------------
void export_cGarch()
{
    class_<cGarch, bases<cAbstCondVar> >("cGarch",
        "GARCH(p,q) variance model.",
        init< optional<uint, uint> >(
            (boost::python::arg("theNArch") = 0, boost::python::arg("theNGarch") = 0),
            "cGarch(uint p=0, uint q=0)"
        )
    )
        .def("Delete", &cGarch::Delete)
        .def("Print", &Garch_Print)

        .def("SetDefaultInitPoint",
            static_cast<void (cGarch::*)(double, double)>(&cGarch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cGarch::*)(cRegArchValue&)>(&cGarch::SetDefaultInitPoint))

        // ReAlloc
        .def("ReAlloc",
            static_cast<void (cGarch::*)(uint, uint)>(&cGarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cGarch::*)(const cDVector&, uint)>(&cGarch::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cGarch::*)(double, uint, uint)>(&cGarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cGarch::*)(const cDVector&, uint)>(&cGarch::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cGarch::*)(uint, uint)>(&cGarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cGarch::*)(uint)>(&cGarch::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get by name
        .def("SetValueName",
            static_cast<void (cGarch::*)(double, std::string, uint)>(&cGarch::Set))
        .def("SetVectorName",
            static_cast<void (cGarch::*)(const cDVector&, std::string)>(&cGarch::Set))
        .def("GetValueName",
            static_cast<double (cGarch::*)(std::string, uint)>(&cGarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cGarch::*)(std::string)>(&cGarch::Get),
            return_value_policy<reference_existing_object>())

        .def("ComputeVar", &cGarch::ComputeVar)
        .def("GetNParam", &cGarch::GetNParam)
        .def("GetNLags", &cGarch::GetNLags)
        .def("ComputeGrad", &cGarch::ComputeGrad)
        .def("RegArchParamToVector", &cGarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cGarch::VectorToRegArchParam)
        .def("ComputeHess", &cGarch::ComputeHess)
        .def("ComputeGradAndHess", &cGarch::ComputeGradAndHess)

        .def("GetParamNameChar",
            static_cast<void (cGarch::*)(uint, char**)>(&cGarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cGarch::*)(uint, std::string[])>(&cGarch::GetParamName))

        // ----------------------------------------------------------------
        // NEW: Unified "set", "get", "realloc" that handle python lists
        // ----------------------------------------------------------------
        .def("set", &cGarchSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to set GARCH parameters.\n\n"
            "Examples:\n"
            "  garch.set(0.1, 0)           # float to param index\n"
            "  garch.set([0.1, 0.2], 0)    # python list for multiple parameters\n"
            "  garch.set(0.15, 'alpha', 1) # by name\n"
        )
        .def("get", &cGarchGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to get GARCH parameters.\n\n"
            "Examples:\n"
            "  val = garch.get(0, 1)        # single param as float\n"
            "  vect = garch.get(0)          # entire param vector\n"
            "  val2 = garch.get('alpha', 1) # param by name\n"
        )
        .def("realloc", &cGarchReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("num_param") = object()),
            "Unified method to reallocate GARCH parameters.\n\n"
            "Examples:\n"
            "  garch.realloc(3)                # reallocate with size=3\n"
            "  garch.realloc([0.1, 0.2, 0.3])  # from a python list\n"
            "  garch.realloc(cDVector(...))    # from a cDVector"
        )
        ;
}
