#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>


using namespace boost::python;
using namespace RegArchLib;
using namespace VectorAndMatrixNameSpace;

static void Ugarch_Print(cUgarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// Factory function for the second constructor that takes (bool, double, cDVector&, cDVector&, cDVector&)
cUgarch* createUgarch_from_vectors(bool theExistConst, double theConst, object betaS_obj, object arch_obj, object garch_obj)
{
    // Extract cDVector from the Python objects
    cDVector betaS = extract<cDVector>(betaS_obj);
    cDVector arch = extract<cDVector>(arch_obj);
    cDVector garch = extract<cDVector>(garch_obj);
    // Call the constructor by passing copies (so they can bind to non-const references)
    return new cUgarch(theExistConst, theConst, betaS, arch, garch);
}

void export_cUgarch()
{
    class_<cUgarch, bases<cAbstCondVar> >("cUgarch",
        "UGARCH(p,q) model",
        no_init
    )
        // First constructor: (bool, uint, uint, uint)
        .def(init< optional<bool, uint, uint, uint> >(
            (boost::python::arg("theExistConst") = true,
                boost::python::arg("theNBetas") = 0,
                boost::python::arg("theNArch") = 0,
                boost::python::arg("theNGarch") = 0),
            "cUgarch(bool theExistConst=true, uint theNBetas=0, uint theNArch=0, uint theNGarch=0)"
        ))
        // Second constructor via our factory: (bool, double, cDVector&, cDVector&, cDVector&)
        .def("__init__", make_constructor(&createUgarch_from_vectors,
            default_call_policies(),
            (boost::python::arg("theExistConst"),
                boost::python::arg("theConst"),
                boost::python::arg("theBetaS"),
                boost::python::arg("theArch"),
                boost::python::arg("theGarch"))
        ))
        .def("Delete", &cUgarch::Delete)
        .def("Print", &Ugarch_Print)
        .def("SetDefaultInitPoint",
            static_cast<void (cUgarch::*)(double, double)>(&cUgarch::SetDefaultInitPoint),
            (boost::python::arg("theMean"), boost::python::arg("theVar")))
        .def("SetDefaultInitPointValue",
            static_cast<void (cUgarch::*)(cRegArchValue&)>(&cUgarch::SetDefaultInitPoint))
        .def("ReAlloc",
            static_cast<void (cUgarch::*)(uint, uint)>(&cUgarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cUgarch::*)(const cDVector&, uint)>(&cUgarch::ReAlloc))
        .def("SetValueIndex",
            static_cast<void (cUgarch::*)(double, uint, uint)>(&cUgarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cUgarch::*)(const cDVector&, uint)>(&cUgarch::Set))
        .def("GetValueIndex",
            static_cast<double (cUgarch::*)(uint, uint)>(&cUgarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cUgarch::*)(uint)>(&cUgarch::Get),
            return_value_policy<reference_existing_object>())
        .def("SetValueName",
            static_cast<void (cUgarch::*)(double, std::string, uint)>(&cUgarch::Set))
        .def("SetVectorName",
            static_cast<void (cUgarch::*)(const cDVector&, std::string)>(&cUgarch::Set))
        .def("GetValueName",
            static_cast<double (cUgarch::*)(std::string, uint)>(&cUgarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cUgarch::*)(std::string)>(&cUgarch::Get),
            return_value_policy<reference_existing_object>())
        .def("ComputeVar", &cUgarch::ComputeVar)
        .def("GetNParam", &cUgarch::GetNParam)
        .def("GetNLags", &cUgarch::GetNLags)
        .def("ComputeGrad", &cUgarch::ComputeGrad)
        .def("RegArchParamToVector", &cUgarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cUgarch::VectorToRegArchParam)
        .def("ComputeHess", &cUgarch::ComputeHess)
        .def("ComputeGradAndHess", &cUgarch::ComputeGradAndHess)
        .def("GetParamNameChar",
            static_cast<void (cUgarch::*)(uint, char**)>(&cUgarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cUgarch::*)(uint, std::string[])>(&cUgarch::GetParamName))
        ;
}
