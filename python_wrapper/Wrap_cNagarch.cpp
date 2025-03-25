#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// If you have a function that converts Python lists/tuples to cDVector:
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// Helper for printing
static void Nagarch_Print(cNagarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "set" that accepts float or Python lists
// --------------------------------------------------------------------
static void cNagarchSet(cNagarch& self,
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
        // cDVector or Python list that auto-converted
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
            throw std::runtime_error(
                "cNagarch.set(...) usage: second argument must be string or uint when passing a vector.");
        }
    }
    else if (val_extract.check())
    {
        // A single float/double
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
            uint i = idx_extract();
            uint param_num = 0;
            if (!num_param.is_none())
                param_num = extract<uint>(num_param);
            self.Set(val, i, param_num);
        }
        else {
            throw std::runtime_error(
                "cNagarch.set(...) usage: second argument must be string or uint when passing a float.");
        }
    }
    else
    {
        // Possibly a Python list/tuple not auto-converted
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
                    "cNagarch.set(...) usage: second arg must be string or uint when passing a Python list/tuple.");
            }
        }
        else {
            throw std::runtime_error(
                "First argument must be float, cDVector, or Python list/tuple of floats for cNagarch.set(...).");
        }
    }
}

// --------------------------------------------------------------------
// NEW: Helper for "get" that returns float or cDVector
// --------------------------------------------------------------------
static object cNagarchGet(cNagarch& self,
    const object& index_or_name,
    const object& num_param = object())
{
    extract<std::string> name_extract(index_or_name);
    extract<uint>        idx_extract(index_or_name);

    if (name_extract.check())
    {
        // param name
        std::string pname = name_extract();

        if (num_param.is_none()) {
            // cDVector& version => self.Get(std::string)
            return object(boost::ref(self.Get(pname)));
        }
        else {
            // double version => self.Get(std::string, uint)
            uint idx = extract<uint>(num_param);
            return object(self.Get(pname, idx));
        }
    }
    else if (idx_extract.check())
    {
        // param index
        uint i = idx_extract();

        if (num_param.is_none()) {
            // cDVector& => self.Get(uint)
            return object(boost::ref(self.Get(i)));
        }
        else {
            // double => self.Get(uint, uint)
            uint sub_i = extract<uint>(num_param);
            return object(self.Get(i, sub_i));
        }
    }
    else {
        throw std::runtime_error(
            "cNagarch.get(...) usage: first arg must be a string (param name) or a uint (param index).");
    }
}

// --------------------------------------------------------------------
// NEW: Helper for "realloc" that can accept a python list
// --------------------------------------------------------------------
static void cNagarchReAlloc(cNagarch& self,
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
        if (PyList_Check(size_or_vector.ptr()) || PyTuple_Check(size_or_vector.ptr()))
        {
            cDVector vect = py_list_or_tuple_to_cDVector(size_or_vector);
            self.ReAlloc(vect, param);
        }
        else {
            throw std::runtime_error(
                "cNagarch.realloc(...) usage: pass a uint size, cDVector, or Python list/tuple of floats.");
        }
    }
}

// --------------------------------------------------------------------
// The export function with minimal new additions
// --------------------------------------------------------------------
void export_cNagarch()
{
    class_<cNagarch, bases<cAbstCondVar> >("cNagarch",
        "NAGARCH(p,q) variance model",
        init< optional<uint, uint> >(
            (boost::python::arg("theNArch") = 0, boost::python::arg("theNGarch") = 0),
            "cNagarch(uint p=0, uint q=0)\n\n"
            "Construct a NAGARCH(p,q) model with p arch terms and q garch terms."
        )
    )
        .def("Delete", &cNagarch::Delete)
        .def("Print", &Nagarch_Print)

        .def("SetDefaultInitPoint",
            static_cast<void (cNagarch::*)(double, double)>(&cNagarch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cNagarch::*)(cRegArchValue&)>(&cNagarch::SetDefaultInitPoint))

        // ReAlloc
        .def("ReAlloc",
            static_cast<void (cNagarch::*)(uint, uint)>(&cNagarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cNagarch::*)(const cDVector&, uint)>(&cNagarch::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cNagarch::*)(double, uint, uint)>(&cNagarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cNagarch::*)(const cDVector&, uint)>(&cNagarch::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cNagarch::*)(uint, uint)>(&cNagarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cNagarch::*)(uint)>(&cNagarch::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get by name
        .def("SetValueName",
            static_cast<void (cNagarch::*)(double, std::string, uint)>(&cNagarch::Set))
        .def("SetVectorName",
            static_cast<void (cNagarch::*)(const cDVector&, std::string)>(&cNagarch::Set))
        .def("GetValueName",
            static_cast<double (cNagarch::*)(std::string, uint)>(&cNagarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cNagarch::*)(std::string)>(&cNagarch::Get),
            return_value_policy<reference_existing_object>())

        .def("ComputeVar", &cNagarch::ComputeVar)
        .def("GetNParam", &cNagarch::GetNParam)
        .def("GetNLags", &cNagarch::GetNLags)
        .def("ComputeGrad", &cNagarch::ComputeGrad)
        .def("RegArchParamToVector", &cNagarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cNagarch::VectorToRegArchParam)
        .def("ComputeHess", &cNagarch::ComputeHess)
        .def("ComputeGradAndHess", &cNagarch::ComputeGradAndHess)
        .def("SyncSizeParam", &cNagarch::SyncSizeParam)

        .def("GetParamNameChar",
            static_cast<void (cNagarch::*)(uint, char**)>(&cNagarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cNagarch::*)(uint, std::string[])>(&cNagarch::GetParamName))

        // -------------------------------------------------------------
        // NEW: unified set(...), get(...), and realloc(...) that accept
        // Python floats or Python lists/tuples for cDVector
        // -------------------------------------------------------------
        .def("set", &cNagarchSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to set NAGARCH parameters.\n\n"
            "Examples:\n"
            "  nagarch.set(0.1, 0)              # single float param by numeric index\n"
            "  nagarch.set([0.2, 0.3], 0)       # multiple params from a Python list\n"
            "  nagarch.set(0.15, 'arch', 1)     # param name 'arch', subindex=1 => float"
        )
        .def("get", &cNagarchGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to get NAGARCH parameters.\n\n"
            "Examples:\n"
            "  val = nagarch.get(0, 1)          # single float param\n"
            "  vect = nagarch.get(0)            # cDVector of params\n"
            "  val2 = nagarch.get('arch', 1)    # param name + subindex => float"
        )
        .def("realloc", &cNagarchReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("num_param") = object()),
            "Unified method to reallocate NAGARCH parameters.\n\n"
            "Examples:\n"
            "  nagarch.realloc(3)                   # reallocate size=3\n"
            "  nagarch.realloc([0.1, 0.2, 0.3])     # from a python list\n"
            "  nagarch.realloc(cDVector(...), 0)    # from a cDVector, param_num=0"
        )
        ;
}
