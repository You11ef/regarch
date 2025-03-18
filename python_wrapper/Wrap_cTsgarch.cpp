#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

static void Tsgarch_Print(cTsgarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cTsgarch()
{
    class_<cTsgarch, bases<cAbstCondVar> >("cTsgarch",
        "TS-GARCH(p,q) model (Taylor-Schwert GARCH model)",
        init< optional<uint, uint> >(
            (boost::python::arg("theNArch") = 0, boost::python::arg("theNGarch") = 0),
            "cTsgarch(uint theNArch=0, uint theNGarch=0)"
        )
    )
        .def("Delete", &cTsgarch::Delete)
        .def("Print", &Tsgarch_Print)
        .def("SetDefaultInitPoint",
            static_cast<void (cTsgarch::*)(double, double)>(&cTsgarch::SetDefaultInitPoint),
            (boost::python::arg("theMean"), boost::python::arg("theVar")))
        .def("SetDefaultInitPointValue",
            static_cast<void (cTsgarch::*)(cRegArchValue&)>(&cTsgarch::SetDefaultInitPoint))
        .def("ReAlloc",
            static_cast<void (cTsgarch::*)(uint, uint)>(&cTsgarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cTsgarch::*)(const cDVector&, uint)>(&cTsgarch::ReAlloc))
        .def("SetValueIndex",
            static_cast<void (cTsgarch::*)(double, uint, uint)>(&cTsgarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cTsgarch::*)(const cDVector&, uint)>(&cTsgarch::Set))
        .def("GetValueIndex",
            static_cast<double (cTsgarch::*)(uint, uint)>(&cTsgarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cTsgarch::*)(uint)>(&cTsgarch::Get),
            return_value_policy<reference_existing_object>())
        .def("SetValueName",
            static_cast<void (cTsgarch::*)(double, std::string, uint)>(&cTsgarch::Set))
        .def("SetVectorName",
            static_cast<void (cTsgarch::*)(const cDVector&, std::string)>(&cTsgarch::Set))
        .def("GetValueName",
            static_cast<double (cTsgarch::*)(std::string, uint)>(&cTsgarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cTsgarch::*)(std::string)>(&cTsgarch::Get),
            return_value_policy<reference_existing_object>())
        .def("ComputeVar", &cTsgarch::ComputeVar)
        .def("GetNParam", &cTsgarch::GetNParam)
        .def("GetNLags", &cTsgarch::GetNLags)
        .def("ComputeGrad", &cTsgarch::ComputeGrad)
        .def("RegArchParamToVector", &cTsgarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cTsgarch::VectorToRegArchParam)
        .def("ComputeHess", &cTsgarch::ComputeHess)
        .def("ComputeGradAndHess", &cTsgarch::ComputeGradAndHess)
        .def("GetParamNameChar",
            static_cast<void (cTsgarch::*)(uint, char**)>(&cTsgarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cTsgarch::*)(uint, std::string[])>(&cTsgarch::GetParamName))
        ;
}
