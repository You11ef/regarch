#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// 1) Include your helper header that declares py_list_or_tuple_to_cDVector(...)
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// Helper function for Print
static void LinReg_Print(cLinReg& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// --------------------------------------------------------------------
//  NEW: Helper for a "unified set" method that can accept lists
// --------------------------------------------------------------------
static void cLinRegSet(cLinReg& self, const object& value_or_vector, const object& index_or_name, const object& num_param = object())
{
    extract<cDVector> vec_extract(value_or_vector);
    extract<double>   val_extract(value_or_vector);

    // Check if second argument is a name (string) or an index (uint)
    extract<std::string> name_extract(index_or_name);
    extract<uint>        idx_extract(index_or_name);

    if (vec_extract.check())
    {
        // The user passed a cDVector directly
        cDVector vector = vec_extract();

        if (name_extract.check()) {
            // self.Set(cDVector, std::string)
            std::string param_name = name_extract();
            self.Set(vector, param_name);
        }
        else if (idx_extract.check()) {
            // self.Set(cDVector, uint)
            uint param_num = 0;
            if (!num_param.is_none())
                param_num = extract<uint>(num_param);
            self.Set(vector, param_num);
        }
        else {
            throw std::runtime_error("cLinReg.set(...) usage error: second argument must be string or uint when passing a vector.");
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
            uint pnum = 0;
            if (!num_param.is_none())
                pnum = extract<uint>(num_param);
            self.Set(val, idx, pnum);
        }
        else {
            throw std::runtime_error("cLinReg.set(...) usage error: second argument must be string or uint when passing a float.");
        }
    }
    else
    {
        // Fallback for Python list/tuple
        if (PyList_Check(value_or_vector.ptr()) || PyTuple_Check(value_or_vector.ptr()))
        {
            // Convert python sequence to cDVector
            cDVector vector = py_list_or_tuple_to_cDVector(value_or_vector);

            if (name_extract.check()) {
                // self.Set(cDVector, std::string)
                self.Set(vector, name_extract());
            }
            else if (idx_extract.check()) {
                uint param_num = 0;
                if (!num_param.is_none())
                    param_num = extract<uint>(num_param);
                self.Set(vector, param_num);
            }
            else {
                throw std::runtime_error("cLinReg.set(...) usage error: second argument must be string or uint when passing a Python list/tuple.");
            }
        }
        else {
            throw std::runtime_error("cLinReg.set(...) usage error: first arg must be float, cDVector, or Python list/tuple of floats.");
        }
    }
}

// --------------------------------------------------------------------
//  NEW: Helper for a "unified get" method that can accept name or index
// --------------------------------------------------------------------
static object cLinRegGet(cLinReg& self, const object& index_or_name, const object& num_param = object())
{
    extract<std::string> name_extract(index_or_name);
    extract<uint>        idx_extract(index_or_name);

    if (name_extract.check())
    {
        std::string param_name = name_extract();

        if (num_param.is_none()) {
            // Get(std::string) -> cDVector&
            return object(boost::ref(self.Get(param_name)));
        }
        else {
            // Get(std::string, uint) -> double
            uint idx = extract<uint>(num_param);
            return object(self.Get(param_name, idx));
        }
    }
    else if (idx_extract.check())
    {
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
        throw std::runtime_error("cLinReg.get(...) usage error: first argument must be a string (param name) or uint (param index).");
    }
}

// --------------------------------------------------------------------
//  NEW: Helper for a "unified realloc" method that can accept list or cDVector
// --------------------------------------------------------------------
static void cLinRegReAlloc(cLinReg& self, const object& size_or_vector, const object& num_param = object())
{
    extract<cDVector> vec_extract(size_or_vector);
    extract<uint>     size_extract(size_or_vector);

    uint pnum = 0;
    if (!num_param.is_none())
        pnum = extract<uint>(num_param);

    if (vec_extract.check())
    {
        // The user passed a cDVector
        cDVector vector = vec_extract();
        self.ReAlloc(vector, pnum);
    }
    else if (size_extract.check())
    {
        // The user passed a size (uint)
        uint size = size_extract();
        self.ReAlloc(size, pnum);
    }
    else
    {
        // Fallback for Python list/tuple
        if (PyList_Check(size_or_vector.ptr()) || PyTuple_Check(size_or_vector.ptr()))
        {
            cDVector vector = py_list_or_tuple_to_cDVector(size_or_vector);
            self.ReAlloc(vector, pnum);
        }
        else {
            throw std::runtime_error("cLinReg.realloc(...) usage error: must pass a uint size, cDVector, or Python list/tuple of floats.");
        }
    }
}

// --------------------------------------------------------------------
//  The export function with minimal additions
// --------------------------------------------------------------------
void export_cLinReg()
{
    class_<cLinReg, bases<cAbstCondMean> >("cLinReg",
        "Linear Regression mean component",
        init< optional<int> >(
            (boost::python::arg("theNLinReg") = 0),
            "cLinReg(int theNLinReg=0)"
        )
    )
        .def("Delete", &cLinReg::Delete)
        .def("Print", &LinReg_Print)

        // Overloaded ReAlloc
        .def("ReAllocVect",
            static_cast<void (cLinReg::*)(const cDVector&, uint)>(&cLinReg::ReAlloc))
        .def("ReAllocSize",
            static_cast<void (cLinReg::*)(uint, uint)>(&cLinReg::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cLinReg::*)(double, uint, uint)>(&cLinReg::Set))
        .def("SetVectorIndex",
            static_cast<void (cLinReg::*)(const cDVector&, uint)>(&cLinReg::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cLinReg::*)(uint, uint)>(&cLinReg::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cLinReg::*)(uint)>(&cLinReg::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get by name
        .def("SetValueName",
            static_cast<void (cLinReg::*)(double, std::string, uint)>(&cLinReg::Set))
        .def("SetVectorName",
            static_cast<void (cLinReg::*)(const cDVector&, std::string)>(&cLinReg::Set))
        .def("GetValueName",
            static_cast<double (cLinReg::*)(std::string, uint)>(&cLinReg::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cLinReg::*)(std::string)>(&cLinReg::Get),
            return_value_policy<reference_existing_object>())

        .def("SetDefaultInitPoint",
            static_cast<void (cLinReg::*)(double, double)>(&cLinReg::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cLinReg::*)(cRegArchValue&)>(&cLinReg::SetDefaultInitPoint))

        .def("ComputeMean", &cLinReg::ComputeMean)
        .def("GetNParam", &cLinReg::GetNParam)
        .def("GetNLags", &cLinReg::GetNLags)
        .def("RegArchParamToVector", &cLinReg::RegArchParamToVector)
        .def("VectorToRegArchParam", &cLinReg::VectorToRegArchParam)
        .def("ComputeGrad", &cLinReg::ComputeGrad)
        .def("ComputeHess", &cLinReg::ComputeHess)
        .def("ComputeGradAndHess", &cLinReg::ComputeGradAndHess)

        .def("GetParamNameChar",
            static_cast<void (cLinReg::*)(uint, char**)>(&cLinReg::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cLinReg::*)(uint, std::string[])>(&cLinReg::GetParamName))

        // ----------------------------------------------------------------
        // NEW: Unified "set", "get", "realloc" that can accept Python lists
        // ----------------------------------------------------------------
        .def("set", &cLinRegSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to set parameters.\n\n"
            "Usage:\n"
            "  set(float_value, param_index[, param_num])\n"
            "  set(float_value, param_name[, subindex])\n"
            "  set(list_of_floats, param_index)\n"
            "  set(list_of_floats, param_name)\n"
            "  set(cDVector, param_index)\n"
            "  set(cDVector, param_name)\n\n"
            "Examples:\n"
            "  linreg.set(1.0, 0)              # set a single param by index\n"
            "  linreg.set([1.0, 2.0], 0)       # set multiple params from a Python list\n"
            "  linreg.set(cDVector(...), 0)    # set from a cDVector"
        )
        .def("get", &cLinRegGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to get parameters.\n\n"
            "Usage:\n"
            "  get(param_index) -> cDVector\n"
            "  get(param_index, subindex) -> float\n"
            "  get(param_name) -> cDVector\n"
            "  get(param_name, index) -> float\n"
        )
        .def("realloc", &cLinRegReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("num_param") = object()),
            "Unified method to reallocate parameters.\n\n"
            "Usage:\n"
            "  realloc(size[, param_num])\n"
            "  realloc(list_of_floats[, param_num])\n"
            "  realloc(cDVector[, param_num])\n\n"
            "Examples:\n"
            "  linreg.realloc(5)                # reallocate with size=5\n"
            "  linreg.realloc([1.0, 2.0])       # from a python list\n"
            "  linreg.realloc(cDVector(...))    # from a cDVector"
        )
        ;
}
