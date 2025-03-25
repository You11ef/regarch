#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

#include "PythonConversion.h"  // For py_list_or_tuple_to_cDVector

using namespace boost::python;
using namespace RegArchLib;
using namespace VectorAndMatrixNameSpace; // cDVector, cGSLVector, etc.

//------------------------------------------------------------------------------
// A small helper for printing
//------------------------------------------------------------------------------
static void MixNormResiduals_Print(cMixNormResiduals& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

//------------------------------------------------------------------------------
// If you want to let users do: MixNormResiduals([p, var1, var2], simulFlag)
// we define a factory function that creates cDVector from the Python object
// and calls the cMixNormResiduals constructor that takes cDVector + bool
//------------------------------------------------------------------------------
static cMixNormResiduals* createMixNormResiduals_from_object(object paramObj, bool simulFlag)
{
    // The user presumably passes [p, var1, var2] (3 parameters).
    // We'll convert that to cDVector.
    cDVector paramVec = py_list_or_tuple_to_cDVector(paramObj);
    // Now, cMixNormResiduals typically has a constructor that takes cDVector* or cDVector&.
    // We assume there's one that does something like cMixNormResiduals(const cDVector* theParam, bool theSimulFlag).
    // But in your code you might have cMixNormResiduals(cDVector*, bool).
    // So we do new cMixNormResiduals(...) properly:

    // If your actual constructor is e.g. cMixNormResiduals(const cDVector& theParam, bool theSimulFlag),
    // then we can do:
    return new cMixNormResiduals(&paramVec, simulFlag);
    // If your constructor is different, adapt accordingly.
}

//------------------------------------------------------------------------------
// OPTIONAL: If you want a Python-friendly method to set parameters from a Python
// list/tuple, or get them as a Python object, do so here:
//------------------------------------------------------------------------------
static void cMixNormResSetParams(cMixNormResiduals& self, object paramObj)
{
    cDVector newParams = py_list_or_tuple_to_cDVector(paramObj);
    self.VectorToRegArchParam(newParams, 0);
}

static object cMixNormResGetParams(cMixNormResiduals& self)
{
    cDVector paramVec(self.GetNParam());
    self.RegArchParamToVector(paramVec, 0);
    // Return as a python object referencing paramVec. If you have a direct
    // cDVector->list conversion, it might do so automatically. Otherwise, you can
    // do manual conversion to a Python list, e.g.:
    return object(paramVec); // (assuming cDVector to Python converter is registered)
}

//------------------------------------------------------------------------------
// Export function
//------------------------------------------------------------------------------
void export_cMixNormResiduals()
{
    class_<cMixNormResiduals, bases<cAbstResiduals> >("cMixNormResiduals",
        "Mixture-of-Normals residuals class.\n"
        "You can construct this object either by explicitly specifying the parameters "
        "(p, var1, var2, simulFlag) or by providing a Python list [p, var1, var2].",
        no_init
    )
        // 1) Bind the double-based constructor:
        .def(init<double, double, double, optional<bool> >(
            (boost::python::arg("p"),
                boost::python::arg("var1"),
                boost::python::arg("var2"),
                boost::python::arg("simulFlag") = true),
            "cMixNormResiduals(double p, double var1, double var2, bool simulFlag=true)\n\n"
            "Construct with separate parameters p, var1, var2, plus a bool for simulation."
        ))

        // 2) Bind the pointer-based constructor that accepts a Python list:
        //    i.e. cMixNormResiduals(*cDVector, bool).
        //    We do it with make_constructor pointing at createMixNormResiduals_from_object:
        .def("__init__", make_constructor(
            &createMixNormResiduals_from_object,
            default_call_policies(),
            (boost::python::arg("theDistrParameter"), boost::python::arg("theSimulFlag") = false)
        ))

        // Basic housekeeping
        .def("Print", &MixNormResiduals_Print, "Print the parameters.")
        .def("SetDefaultInitPoint", &cMixNormResiduals::SetDefaultInitPoint,
            "Initialize default parameters based on typical or guessed stats.")
        .def("Generate", &cMixNormResiduals::Generate,
            "(uint theNSample, cDVector& theYt) -> None\n"
            "Generate a sample of residuals into Yt.")
        .def("LogDensity", &cMixNormResiduals::LogDensity,
            "(double x) -> double\n"
            "Return log density at x.")
        .def("GetNParam", &cMixNormResiduals::GetNParam)
        .def("DiffLogDensity", &cMixNormResiduals::DiffLogDensity)
        .def("ComputeGrad", &cMixNormResiduals::ComputeGrad)
        .def("RegArchParamToVector", &cMixNormResiduals::RegArchParamToVector)
        .def("VectorToRegArchParam", &cMixNormResiduals::VectorToRegArchParam)
        .def("ComputeEspAbsEps", &cMixNormResiduals::ComputeEspAbsEps)
        .def("ComputeGradBetaEspAbsEps", &cMixNormResiduals::ComputeGradBetaEspAbsEps)
        .def("ComputeHessBetaEspAbsEps", &cMixNormResiduals::ComputeHessBetaEspAbsEps)
        .def("Diff2LogDensity", &cMixNormResiduals::Diff2LogDensity)
        .def("GradDiffLogDensity", &cMixNormResiduals::GradDiffLogDensity)
        .def("ComputeHess", &cMixNormResiduals::ComputeHess)
        .def("ComputeGradAndHess", &cMixNormResiduals::ComputeGradAndHess)
        .def("GetParamNameChar",
            static_cast<void (cMixNormResiduals::*)(uint, char**)>(&cMixNormResiduals::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cMixNormResiduals::*)(uint, std::string[])>(&cMixNormResiduals::GetParamName))

        // OPTIONAL: If you want Python methods to set/get parameters from a Python list easily
        .def("set_params", &cMixNormResSetParams,
            boost::python::arg("paramList"),
            "Set distribution parameters (p, var1, var2, ...) from a Python list.\n"
            "Equivalent to self.VectorToRegArchParam.")
        .def("get_params", &cMixNormResGetParams,
            "Return the distribution parameters as a cDVector (or list if converter is registered).")
        ;
}
