#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// 1) Include your helper header that provides py_list_or_tuple_to_cDVector(...)
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

// Helper function for Print
static void GedResiduals_Print(cGedResiduals& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "set" that can accept float or list/tuple
// --------------------------------------------------------------------
static void cGedResidualsSet(cGedResiduals& self,
    const object& pyValueOrVector,
    const object& pyIndex = object())
{
    // Typically, cGedResiduals only has 1 param (Beta).
    // But we'll allow a flexible approach that checks if the user wants
    // to set multiple distribution parameters at once.

    extract<cDVector> vectorExtract(pyValueOrVector);
    extract<double>   doubleExtract(pyValueOrVector);

    // If you have multiple parameters, you might interpret pyIndex as an index
    // for which param to set. But cGedResiduals typically only has param[0].
    // We'll show the logic anyway, in case there's more than 1 parameter.

    uint paramIndex = 0;
    if (!pyIndex.is_none())
        paramIndex = extract<uint>(pyIndex);

    if (vectorExtract.check())
    {
        // The user passed a cDVector or a Python list converted to cDVector
        cDVector vect = vectorExtract();
        // If your distribution has only 1 param, vect.size() probably == 1
        // We'll store them via VectorToRegArchParam or direct internal calls.
        // For simplicity, let's do:
        self.VectorToRegArchParam(vect, paramIndex);
    }
    else if (doubleExtract.check())
    {
        // The user passed a single float or int
        double val = doubleExtract();

        // We'll create a cDVector(1) just to store this single param,
        // then call VectorToRegArchParam
        cDVector tmp(1);
        tmp[0] = val;
        self.VectorToRegArchParam(tmp, paramIndex);
    }
    else
    {
        // fallback if it's neither a direct cDVector nor a double:
        // maybe it's a Python list/tuple that didn't auto-convert.
        // we'll check for a sequence manually.
        if (PyList_Check(pyValueOrVector.ptr()) || PyTuple_Check(pyValueOrVector.ptr()))
        {
            cDVector vect = py_list_or_tuple_to_cDVector(pyValueOrVector);
            self.VectorToRegArchParam(vect, paramIndex);
        }
        else {
            throw std::runtime_error(
                "ged.set(...) usage error: first argument must be a float, "
                "a cDVector, or a Python list/tuple of floats.");
        }
    }
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "get" that returns float or cDVector
// --------------------------------------------------------------------
static object cGedResidualsGet(cGedResiduals& self,
    const object& pyIndex = object())
{
    // If cGedResiduals only has 1 param, we might ignore pyIndex
    // and just return that param. But let's do it flexibly anyway.

    uint paramIndex = 0;
    if (!pyIndex.is_none())
        paramIndex = extract<uint>(pyIndex);

    // We'll figure out how many parameters exist
    // Typically cGedResiduals has 1 param => GetNParam() = 1.
    // We'll allocate a cDVector of that size, call RegArchParamToVector, and see if paramIndex matters.
    uint nparam = self.GetNParam();
    if (paramIndex >= nparam)
    {
        throw std::runtime_error(
            "Invalid paramIndex for cGedResiduals.get(...). Out of range.");
    }

    // let's get the entire param vector, then either return the single param
    // or the entire vector if paramIndex wasn't specified
    cDVector allParams(nparam);
    self.RegArchParamToVector(allParams, 0);

    // If the user didn't specify an index, return the entire vector
    if (pyIndex.is_none() && nparam > 1)
    {
        // Return the entire parameter vector
        return object(boost::ref(allParams));
    }
    else if (pyIndex.is_none() && nparam == 1)
    {
        // Just return the single param as a float
        return object(allParams[0]);
    }
    else
    {
        // They specified an index, so return that param as a double
        if (paramIndex >= allParams.GetSize())
            throw std::runtime_error("paramIndex out of range in cGedResiduals.get(...).");
        return object(allParams[paramIndex]);
    }
}

// --------------------------------------------------------------------
// NEW: Helper for a unified "realloc" if you want to resize param array
//     If cGedResiduals doesn't actually implement ReAlloc, omit this.
// --------------------------------------------------------------------
static void cGedResidualsReAlloc(cGedResiduals& /*self*/,
    const object& /*size_or_vector*/)
{
    // If cGedResiduals doesn't have a ReAlloc(...) method,
    // you can either omit this function or implement a stub that throws.
    throw std::runtime_error(
        "cGedResiduals does not support reallocation of parameters");
}

// --------------------------------------------------------------------
// Final export with minimal changes + new set/get + (optionally) realloc
// --------------------------------------------------------------------
void export_cGedResiduals()
{
    class_<cGedResiduals, bases<cAbstResiduals> >("cGedResiduals",
        "GED residuals with shape parameter Beta",
        init< optional<double, bool> >(
            (boost::python::arg("theBeta") = 2.0, boost::python::arg("theSimulFlag") = false),
            "cGedResiduals(double beta=2.0, bool simulFlag=false)"
        )
    )
        .def("Print", &GedResiduals_Print)
        .def("SetDefaultInitPoint", &cGedResiduals::SetDefaultInitPoint)
        .def("Generate", &cGedResiduals::Generate)
        .def("LogDensity", &cGedResiduals::LogDensity)
        .def("GetNParam", &cGedResiduals::GetNParam)
        .def("DiffLogDensity", &cGedResiduals::DiffLogDensity)
        .def("ComputeGrad", &cGedResiduals::ComputeGrad)
        .def("RegArchParamToVector", &cGedResiduals::RegArchParamToVector)
        .def("VectorToRegArchParam", &cGedResiduals::VectorToRegArchParam)
        .def("ComputeEspAbsEps", &cGedResiduals::ComputeEspAbsEps)
        .def("ComputeGradBetaEspAbsEps", &cGedResiduals::ComputeGradBetaEspAbsEps)
        .def("ComputeHessBetaEspAbsEps", &cGedResiduals::ComputeHessBetaEspAbsEps)
        .def("Diff2LogDensity", &cGedResiduals::Diff2LogDensity)
        .def("GradDiffLogDensity", &cGedResiduals::GradDiffLogDensity)
        .def("ComputeHess", &cGedResiduals::ComputeHess)
        .def("ComputeGradAndHess", &cGedResiduals::ComputeGradAndHess)

        .def("GetParamNameChar",
            static_cast<void (cGedResiduals::*)(uint, char**)>(&cGedResiduals::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cGedResiduals::*)(uint, std::string[])>(&cGedResiduals::GetParamName))

        // ----------------------------------------------------------------
        // NEW: unify param setting / getting / reallocation
        // ----------------------------------------------------------------
        .def("set", &cGedResidualsSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("param_index") = object()),
            "Unified method to set GED distribution parameters.\n\n"
            "Examples:\n"
            "    ged.set(2.5)          # sets Beta=2.5 if only one param\n"
            "    ged.set([2.5])        # also sets Beta=2.5\n"
            "    ged.set(2.0, 0)       # sets paramIndex=0 to 2.0\n"
        )
        .def("get", &cGedResidualsGet,
            (boost::python::arg("param_index") = object()),
            "Unified method to get GED distribution parameters.\n\n"
            "Examples:\n"
            "    val = ged.get()       # returns Beta if only 1 param\n"
            "    vec = ged.get()       # if multiple params, returns entire param vector\n"
            "    val2 = ged.get(0)     # param 0 as float\n"
        )
        .def("realloc", &cGedResidualsReAlloc,
            (boost::python::arg("size_or_vector")),
            "Stub for reallocation. GED typically has 1 param, so not used.")
        ;
}
