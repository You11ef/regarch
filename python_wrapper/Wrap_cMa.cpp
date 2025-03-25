#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// 1) Include your helper header that defines py_list_or_tuple_to_cDVector(...)
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// Helper function for Print
static void Ma_Print(cMa& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// --------------------------------------------------------------------
//  New: Helper for unified "set" 
// --------------------------------------------------------------------
static void cMaSet(cMa& self, const object& value_or_vector, const object& index_or_name, const object& num_param = object())
{
    // Attempt direct extraction of cDVector or double
    extract<cDVector> vector_extract(value_or_vector);
    extract<double>   value_extract(value_or_vector);

    // Check if second argument is a string or an index
    extract<std::string> name_extract(index_or_name);
    extract<uint>        idx_extract(index_or_name);

    if (vector_extract.check())
    {
        // The user gave a cDVector directly
        cDVector vect = vector_extract();

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
            throw std::runtime_error("Invalid usage of cMa.set(...) with a vector. Provide param name (str) or param index (uint).");
        }
    }
    else if (value_extract.check())
    {
        // The user gave a scalar (double)
        double val = value_extract();

        if (name_extract.check()) {
            // Set(double, string, uint)
            uint i = 0;
            if (!num_param.is_none())
                i = extract<uint>(num_param);
            self.Set(val, name_extract(), i);
        }
        else if (idx_extract.check()) {
            // Set(double, uint, uint)
            uint i = idx_extract();
            uint pnum = 0;
            if (!num_param.is_none())
                pnum = extract<uint>(num_param);
            self.Set(val, i, pnum);
        }
        else {
            throw std::runtime_error("Invalid usage of cMa.set(...) with a double. Provide param name (str) or param index (uint).");
        }
    }
    else
    {
        // Fallback for Python list or tuple
        if (PyList_Check(value_or_vector.ptr()) || PyTuple_Check(value_or_vector.ptr()))
        {
            // Convert it to cDVector
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
                throw std::runtime_error("Invalid usage of cMa.set(...) with a Python list/tuple. Provide param name (str) or param index (uint).");
            }
        }
        else {
            throw std::runtime_error("First argument must be a float, cDVector, or a Python list/tuple of floats for cMa.set(...)");
        }
    }
}

// --------------------------------------------------------------------
//  New: Helper for unified "get"
// --------------------------------------------------------------------
static object cMaGet(cMa& self, const object& index_or_name, const object& num_param = object())
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
            uint i = extract<uint>(num_param);
            return object(self.Get(pname, i));
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
            uint pnum = extract<uint>(num_param);
            return object(self.Get(idx, pnum));
        }
    }
    else {
        throw std::runtime_error("First argument must be a string (param name) or uint (param index) for cMa.get(...)");
    }
}

// --------------------------------------------------------------------
//  New: Helper for unified "realloc"
// --------------------------------------------------------------------
static void cMaReAlloc(cMa& self, const object& size_or_vector, const object& param_num = object())
{
    extract<cDVector> vec_extract(size_or_vector);
    extract<uint>     size_extract(size_or_vector);

    uint pnum = 0;
    if (!param_num.is_none())
        pnum = extract<uint>(param_num);

    if (vec_extract.check())
    {
        // The user passed a cDVector
        cDVector vect = vec_extract();
        self.ReAlloc(vect, pnum);
    }
    else if (size_extract.check())
    {
        // The user passed an integer size
        uint sz = size_extract();
        self.ReAlloc(sz, pnum);
    }
    else
    {
        // Fallback for Python list/tuple
        if (PyList_Check(size_or_vector.ptr()) || PyTuple_Check(size_or_vector.ptr()))
        {
            cDVector vect = py_list_or_tuple_to_cDVector(size_or_vector);
            self.ReAlloc(vect, pnum);
        }
        else {
            throw std::runtime_error("First argument must be a uint, cDVector, or a Python list/tuple of floats for cMa.realloc(...)");
        }
    }
}

// --------------------------------------------------------------------
//  The export function with minimal changes
// --------------------------------------------------------------------
void export_cMa()
{
    class_<cMa, bases<cAbstCondMean> >("cMa",
        "Moving Average (MA) mean component",
        init< optional<uint> >(
            (boost::python::arg("theNMa") = 0),
            "cMa(uint theNMa=0)"
        )
    )
        // Existing methods
        .def("Delete", &cMa::Delete)
        .def("Print", &Ma_Print)

        .def("SetDefaultInitPoint",
            static_cast<void (cMa::*)(double, double)>(&cMa::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cMa::*)(cRegArchValue&)>(&cMa::SetDefaultInitPoint))

        // Existing ReAlloc
        .def("ReAlloc",
            static_cast<void (cMa::*)(uint, uint)>(&cMa::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cMa::*)(const cDVector&, uint)>(&cMa::ReAlloc))

        // Existing Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cMa::*)(double, uint, uint)>(&cMa::Set))
        .def("SetVectorIndex",
            static_cast<void (cMa::*)(const cDVector&, uint)>(&cMa::Set))

        // Existing Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cMa::*)(uint, uint)>(&cMa::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cMa::*)(uint)>(&cMa::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get with string
        .def("SetValueName",
            static_cast<void (cMa::*)(double, std::string, uint)>(&cMa::Set))
        .def("SetVectorName",
            static_cast<void (cMa::*)(const cDVector&, std::string)>(&cMa::Set))
        .def("GetValueName",
            static_cast<double (cMa::*)(std::string, uint)>(&cMa::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cMa::*)(std::string)>(&cMa::Get),
            return_value_policy<reference_existing_object>())

        .def("ComputeMean", &cMa::ComputeMean)
        .def("GetNParam", &cMa::GetNParam)
        .def("GetNLags", &cMa::GetNLags)
        .def("ComputeGrad", &cMa::ComputeGrad)
        .def("ComputeHess", &cMa::ComputeHess)
        .def("ComputeGradAndHess", &cMa::ComputeGradAndHess)
        .def("RegArchParamToVector", &cMa::RegArchParamToVector)
        .def("VectorToRegArchParam", &cMa::VectorToRegArchParam)

        .def("GetParamNameChar",
            static_cast<void (cMa::*)(uint, char**)>(&cMa::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cMa::*)(uint, std::string[])>(&cMa::GetParamName))

        // ----------------------------------------------------------------
        // New: Unified "set", "get", and "realloc" that can accept lists
        // ----------------------------------------------------------------
        .def("set", &cMaSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to set parameters.\n\n"
            "Usage:\n"
            "  - set(float_value, param_index[, param_num])\n"
            "  - set(float_value, param_name[, index])\n"
            "  - set(list_of_floats, param_index)\n"
            "  - set(list_of_floats, param_name)\n"
            "  - set(cDVector, param_index)\n"
            "  - set(cDVector, param_name)\n\n"
            "Examples:\n"
            "  ma.set(0.5, 0)            # sets a single value\n"
            "  ma.set([0.5, 0.2], 0)     # sets multiple values from a list\n"
            "  ma.set(cDVector(...), 0)  # sets from a cDVector"
        )
        .def("get", &cMaGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Unified method to get parameters.\n\n"
            "Usage:\n"
            "  - get(param_index) -> cDVector\n"
            "  - get(param_index, subindex) -> float\n"
            "  - get(param_name) -> cDVector\n"
            "  - get(param_name, index) -> float\n\n"
            "Examples:\n"
            "  coeffs = ma.get(0)        # cDVector\n"
            "  val = ma.get(0, 1)        # float\n"
            "  coeffs_by_name = ma.get('ma')\n"
            "  val_by_name = ma.get('ma', 1)\n"
        )
        .def("realloc", &cMaReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("param_num") = object()),
            "Unified method to reallocate parameters.\n\n"
            "Usage:\n"
            "  - realloc(size[, param_num])\n"
            "  - realloc(list_of_floats[, param_num])\n"
            "  - realloc(cDVector[, param_num])\n\n"
            "Examples:\n"
            "  ma.realloc(2)               # reallocate with size=2\n"
            "  ma.realloc([0.1, 0.2])      # reallocate from a python list\n"
            "  ma.realloc(cDVector(...))   # reallocate from a cDVector"
        )
        ;
}
