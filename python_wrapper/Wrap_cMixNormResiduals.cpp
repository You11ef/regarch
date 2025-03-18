#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;
using namespace VectorAndMatrixNameSpace; // For cGSLVector / cDVector

// Helper: Create a new cDVector from a Python list/tuple
// (Assumes that your previously registered converter for cDVector works on objects,
//  but here we need to return a pointer.)
cDVector* create_cDVector_from_object(object obj)
{
    // Extract a cDVector by value using the registered converter.
    // This requires that Python lists convert to cDVector.
    cDVector vec = extract<cDVector>(obj);
    // Allocate a new cDVector on the heap (assumes cMixNormResiduals takes ownership or copies it)
    return new cDVector(vec);
}

// Factory function for the list‐based constructor.
// This overload accepts a Python object (list or tuple) and a bool flag.
cMixNormResiduals* createMixNormResiduals_from_object(object paramObj, bool simulFlag)
{
    cDVector* pVec = create_cDVector_from_object(paramObj);
    return new cMixNormResiduals(pVec, simulFlag);
}

// Helper for Print – calls Print on std::cout.
static void MixNormResiduals_Print(cMixNormResiduals& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// Export function
void export_cMixNormResiduals()
{
    class_<cMixNormResiduals, bases<cAbstResiduals> >("cMixNormResiduals",
        "Mixture-of-Normals residuals class.\n"
        "You can construct this object either by explicitly specifying the parameters "
        "(p, var1, var2, simulFlag) or by providing a Python list (or tuple) of parameters [p, var1, var2].",
        no_init
    )
        // Bind the double-based constructor:
        .def(init<double, double, double, optional<bool> >(
            (boost::python::arg("thep"),
                boost::python::arg("theVar1"),
                boost::python::arg("theVar2"),
                boost::python::arg("theSimulFlag") = true),
            "cMixNormResiduals(double p, double var1, double var2, bool theSimulFlag=true)"
        ))
        // Bind the pointer-based constructor via our factory that accepts a Python list:
        .def("__init__", make_constructor(&createMixNormResiduals_from_object,
            default_call_policies(), (boost::python::arg("theDistrParameter"), boost::python::arg("theSimulFlag") = false)
        ))

        // Expose member functions:
        .def("Print", &MixNormResiduals_Print, "Print the parameters.")
        .def("SetDefaultInitPoint", &cMixNormResiduals::SetDefaultInitPoint)
        .def("Generate", &cMixNormResiduals::Generate,
            "(uint theNSample, cDVector& theYt) -> None")
        .def("LogDensity", &cMixNormResiduals::LogDensity,
            "(double theX) -> double\n"
            "Return log density at the given X. Ensure distribution parameters are set.")
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
        ;
}