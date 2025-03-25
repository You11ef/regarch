#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// Include your helper that converts Python lists/tuples to cDVector
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;
using namespace VectorAndMatrixNameSpace; // If your cDVector is in this namespace

// A small helper for printing
static void NormResiduals_Print(cNormResiduals& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// --------------------------------------------------------------------
// If you want a user-friendly constructor that accepts a Python object
// (list/tuple or None) for the distribution parameters, plus a bool
// --------------------------------------------------------------------
static cNormResiduals* createNormResiduals_from_object(object paramObj, bool simulFlag)
{
    // If user passed None or 0-len list => treat as param = nullptr
    if (paramObj.is_none()) {
        // Equivalent to cNormResiduals(nullptr, simulFlag)
        return new cNormResiduals(nullptr, simulFlag);
    }
    // Otherwise, try converting to cDVector
    cDVector paramVec = py_list_or_tuple_to_cDVector(paramObj);
    // cNormResiduals constructor expects cDVector* if your signature is cNormResiduals(cDVector*, bool).
    // We'll allocate a new cDVector on the heap, but typically cNormResiduals copies or uses it:
    cDVector* paramPtr = new cDVector(paramVec);
    return new cNormResiduals(paramPtr, simulFlag);
}

// --------------------------------------------------------------------
// Optional: Let users do: residuals.set_params([...]) or param = residuals.get_params()
// --------------------------------------------------------------------
static void cNormResSetParams(cNormResiduals& self, object paramObj)
{
    // If cNormResiduals truly has 0 param, this is effectively a no-op.
    cDVector newParams = py_list_or_tuple_to_cDVector(paramObj);
    self.VectorToRegArchParam(newParams, 0);
}

static object cNormResGetParams(cNormResiduals& self)
{
    // If it has 0 param => result is a zero-length cDVector
    cDVector paramVec(self.GetNParam());
    self.RegArchParamToVector(paramVec, 0);
    return object(paramVec); // if cDVector->Python converter is registered, it returns list or cDVector proxy
}

// --------------------------------------------------------------------
// The export function
// --------------------------------------------------------------------
void export_cNormResiduals()
{
    class_<cNormResiduals, bases<cAbstResiduals> >("cNormResiduals",
        "Standard normal residuals (N(0,1))\n\n"
        "Constructors:\n"
        "  - cNormResiduals() with no parameters\n"
        "  - cNormResiduals(cDVector* param, bool simulFlag)\n"
        "  - cNormResiduals([list_of_params], bool simulFlag)\n"
        "But typically, standard normal has 0 parameters, so param is unused.",
        no_init // We'll manually specify constructors
    )
        // 1) Bind the existing constructor that takes (cDVector*, bool) with default param
        .def(init<optional<cDVector*, bool> >(
            (boost::python::arg("theDistrParameter") = (cDVector*)nullptr, boost::python::arg("theSimulFlag") = false),
            "cNormResiduals(cDVector* param=nullptr, bool simulFlag=false)\n\n"
            "Construct with an optional pointer to cDVector of parameters (usually empty), plus a bool for simulation."
        ))

        // 2) For Python lists/tuples: define a factory function
        .def("__init__", make_constructor(
            &createNormResiduals_from_object,
            default_call_policies(),
            (boost::python::arg("theDistrParameter") = object(), // user can pass None or list
                boost::python::arg("theSimulFlag") = false)
        ))

        // Basic housekeeping
        .def("Print", &NormResiduals_Print,
            "Print the parameters (usually none for standard normal).")
        .def("SetDefaultInitPoint", &cNormResiduals::SetDefaultInitPoint,
            "Set default initial parameter(s) if any.")
        .def("Generate", &cNormResiduals::Generate,
            "(uint theNSample, cDVector& theYt) -> generate random draws from N(0,1).")
        .def("LogDensity", &cNormResiduals::LogDensity,
            "(double x) -> log-density for N(0,1).")
        .def("GetNParam", &cNormResiduals::GetNParam)
        .def("DiffLogDensity", &cNormResiduals::DiffLogDensity)
        .def("ComputeGrad", &cNormResiduals::ComputeGrad)
        .def("RegArchParamToVector", &cNormResiduals::RegArchParamToVector)
        .def("VectorToRegArchParam", &cNormResiduals::VectorToRegArchParam)
        .def("ComputeEspAbsEps", &cNormResiduals::ComputeEspAbsEps)
        .def("ComputeGradBetaEspAbsEps", &cNormResiduals::ComputeGradBetaEspAbsEps)
        .def("ComputeHessBetaEspAbsEps", &cNormResiduals::ComputeHessBetaEspAbsEps)
        .def("Diff2LogDensity", &cNormResiduals::Diff2LogDensity)
        .def("GradDiffLogDensity", &cNormResiduals::GradDiffLogDensity)
        .def("ComputeHess", &cNormResiduals::ComputeHess)
        .def("ComputeGradAndHess", &cNormResiduals::ComputeGradAndHess)
        .def("GetParamNameChar",
            static_cast<void (cNormResiduals::*)(uint, char**)>(&cNormResiduals::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cNormResiduals::*)(uint, std::string[])>(&cNormResiduals::GetParamName))

        // OPTIONAL: user-friendly Python methods to set/get parameters
        .def("set_params", &cNormResSetParams,
            boost::python::arg("paramList"),
            "Set distribution parameters from a Python list.\n"
            "If NParam=0, this does nothing. Otherwise, it calls VectorToRegArchParam.")
        .def("get_params", &cNormResGetParams,
            "Return the distribution parameters as a cDVector (or Python list).")
        ;
}
