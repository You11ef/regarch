#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// If you have a helper for converting Python lists/tuples to cDVector:
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// Existing helper for printing
static void Ngarch_Print(cNgarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "set" that accepts float or Python lists
// --------------------------------------------------------------------
static void cNgarchSet(cNgarch& self,
    const object& value_or_vector,
    const object& index_or_name,
    const object& num_param = object())
{
    extract<cDVector> vec_extract(value_or_vector);
    extract<double>   val_extract(value_or_vector);

    // Check whether second argument is a string or a uint
    extract<std::string> name_extract(index_or_name);
    extract<uint>        idx_extract(index_or_name);

    if (vec_extract.check())
    {
        // The user gave a cDVector or Python list that auto-converted
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
                "cNgarch.set(...) usage: second argument must be string or uint when passing a vector.");
        }
    }
    else if (val_extract.check())
    {
        // The user gave a single float/double
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
                "cNgarch.set(...) usage: second argument must be string or uint when passing a float.");
        }
    }
    else
    {
        // Possibly a Python list/tuple that didn't auto-convert
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
                    "cNgarch.set(...) usage: second arg must be string or uint when passing a Python list/tuple.");
            }
        }
        else
        {
            throw std::runtime_error(
                "First argument must be float, cDVector, or Python list/tuple of floats for cNgarch.set(...).");
        }
    }
}

// --------------------------------------------------------------------
// NEW: Helper for "get" that returns float or cDVector
// --------------------------------------------------------------------
static object cNgarchGet(cNgarch& self,
    const object& index_or_name,
    const object& num_param = object())
{
    extract<std::string> name_extract(index_or_name);
    extract<uint>        idx_extract(index_or_name);

    if (name_extract.check())
    {
        // param name
        std::string pname = name_extract();

        if (num_param.is_none())
        {
            // cDVector& => self.Get(std::string)
            return object(boost::ref(self.Get(pname)));
        }
        else
        {
            // double => self.Get(std::string, uint)
            uint idx = extract<uint>(num_param);
            return object(self.Get(pname, idx));
        }
    }
    else if (idx_extract.check())
    {
        // param index
        uint i = idx_extract();

        if (num_param.is_none())
        {
            // cDVector& => self.Get(uint)
            return object(boost::ref(self.Get(i)));
        }
        else
        {
            // double => self.Get(uint, uint)
            uint sub_i = extract<uint>(num_param);
            return object(self.Get(i, sub_i));
        }
    }
    else
    {
        throw std::runtime_error(
            "cNgarch.get(...) usage: first arg must be string (param name) or uint (param index).");
    }
}

// --------------------------------------------------------------------
// NEW: Helper for "realloc" that can accept a python list
// --------------------------------------------------------------------
static void cNgarchReAlloc(cNgarch& self,
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
        // self.ReAlloc(const cDVector&, uint)
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
        // Possibly a Python list/tuple
        if (PyList_Check(size_or_vector.ptr()) || PyTuple_Check(size_or_vector.ptr()))
        {
            cDVector vect = py_list_or_tuple_to_cDVector(size_or_vector);
            self.ReAlloc(vect, param);
        }
        else
        {
            throw std::runtime_error(
                "cNgarch.realloc(...) usage: pass a uint size, cDVector, or a Python list/tuple of floats.");
        }
    }
}

// --------------------------------------------------------------------
// The export function with minimal additions
// --------------------------------------------------------------------
void export_cNgarch()
{
    class_<cNgarch, bases<cAbstCondVar> >("cNgarch",
        "NGARCH(p,q) variance model",
        init< optional<uint, uint> >(
            (boost::python::arg("theNArch") = 0, boost::python::arg("theNNgarch") = 0),
            "cNgarch(uint p=0, uint q=0)\n\n"
            "Construct an NGARCH(p,q) model with p arch terms, q garch terms."
        )
    )
        .def("Delete", &cNgarch::Delete)
        .def("Print", &Ngarch_Print)

        .def("SetDefaultInitPoint",
            static_cast<void (cNgarch::*)(double, double)>(&cNgarch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cNgarch::*)(cRegArchValue&)>(&cNgarch::SetDefaultInitPoint))

        // ReAlloc
        .def("ReAlloc",
            static_cast<void (cNgarch::*)(uint, uint)>(&cNgarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cNgarch::*)(const cDVector&, uint)>(&cNgarch::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cNgarch::*)(double, uint, uint)>(&cNgarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cNgarch::*)(const cDVector&, uint)>(&cNgarch::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cNgarch::*)(uint, uint)>(&cNgarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cNgarch::*)(uint)>(&cNgarch::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get by name
        .def("SetValueName",
            static_cast<void (cNgarch::*)(double, std::string, uint)>(&cNgarch::Set))
        .def("SetVectorName",
            static_cast<void (cNgarch::*)(const cDVector&, std::string)>(&cNgarch::Set))
        .def("GetValueName",
            static_cast<double (cNgarch::*)(std::string, uint)>(&cNgarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cNgarch::*)(std::string)>(&cNgarch::Get),
            return_value_policy<reference_existing_object>())

        .def("ComputeVar", &cNgarch::ComputeVar)
        .def("GetNParam", &cNgarch::GetNParam)
        .def("GetNLags", &cNgarch::GetNLags)
        .def("ComputeGrad", &cNgarch::ComputeGrad)
        .def("RegArchParamToVector", &cNgarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cNgarch::VectorToRegArchParam)
        .def("ComputeHess", &cNgarch::ComputeHess)
        .def("ComputeGradAndHess", &cNgarch::ComputeGradAndHess)

        .def("GetParamNameChar",
            static_cast<void (cNgarch::*)(uint, char**)>(&cNgarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cNgarch::*)(uint, std::string[])>(&cNgarch::GetParamName))

        // -------------------------------------------------------------
        // NEW: unified set(...), get(...), and realloc(...) that accept
        // Python floats or Python lists/tuples for cDVector
        // -------------------------------------------------------------
        .def("set", &cNgarchSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to set NGARCH parameters.\n\n"
            "Examples:\n"
            "  ngarch.set(0.1, 0)               # single float param by numeric index\n"
            "  ngarch.set([0.2,0.3], 0)         # multiple params from a Python list\n"
            "  ngarch.set(0.15, 'arch', 1)      # param name 'arch', subindex=1 => float"
        )
        .def("get", &cNgarchGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to get NGARCH parameters.\n\n"
            "Examples:\n"
            "  val = ngarch.get(0, 1)           # single float param\n"
            "  vect = ngarch.get(0)             # cDVector of params\n"
            "  val2 = ngarch.get('arch', 1)     # param name + subindex => float"
        )
        .def("realloc", &cNgarchReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("num_param") = object()),
            "Unified method to reallocate NGARCH parameters.\n\n"
            "Examples:\n"
            "  ngarch.realloc(3)                   # reallocate size=3\n"
            "  ngarch.realloc([0.1, 0.2, 0.3])     # from a python list\n"
            "  ngarch.realloc(cDVector(...), 0)    # from a cDVector"
        )
        ;
}
