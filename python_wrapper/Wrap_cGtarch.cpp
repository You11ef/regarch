#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

static void Gtarch_Print(cGtarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cGtarch()
{
    class_<cGtarch, bases<cAbstCondVar> >("cGtarch",
        "GTARCH(p,q) model with asymmetry for positive/negative shocks.",
        init< optional<uint, uint> >(
            (boost::python::arg("theNArch") = 0, boost::python::arg("theNGarch") = 0),
            "cGtarch(uint p=0, uint q=0)"
        )
    )
        .def("Delete", &cGtarch::Delete)
        .def("Print", &Gtarch_Print)
        .def("SetDefaultInitPoint",
            static_cast<void (cGtarch::*)(double, double)>(&cGtarch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cGtarch::*)(cRegArchValue&)>(&cGtarch::SetDefaultInitPoint))
        .def("ReAlloc",
            static_cast<void (cGtarch::*)(uint, uint)>(&cGtarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cGtarch::*)(const cDVector&, uint)>(&cGtarch::ReAlloc))
        .def("SetValueIndex",
            static_cast<void (cGtarch::*)(double, uint, uint)>(&cGtarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cGtarch::*)(const cDVector&, uint)>(&cGtarch::Set))
        .def("GetValueIndex",
            static_cast<double (cGtarch::*)(uint, uint)>(&cGtarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cGtarch::*)(uint)>(&cGtarch::Get),
            return_value_policy<reference_existing_object>())
        .def("SetValueName",
            static_cast<void (cGtarch::*)(double, std::string, uint)>(&cGtarch::Set))
        .def("SetVectorName",
            static_cast<void (cGtarch::*)(const cDVector&, std::string)>(&cGtarch::Set))
        .def("GetValueName",
            static_cast<double (cGtarch::*)(std::string, uint)>(&cGtarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cGtarch::*)(std::string)>(&cGtarch::Get),
            return_value_policy<reference_existing_object>())
        .def("ComputeVar", &cGtarch::ComputeVar)
        .def("GetNParam", &cGtarch::GetNParam)
        .def("GetNLags", &cGtarch::GetNLags)
        .def("ComputeGrad", &cGtarch::ComputeGrad)
        .def("RegArchParamToVector", &cGtarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cGtarch::VectorToRegArchParam)
        .def("ComputeHess", &cGtarch::ComputeHess)
        .def("ComputeGradAndHess", &cGtarch::ComputeGradAndHess)
        .def("SyncSizeParam", &cGtarch::SyncSizeParam)
        .def("GetParamNameChar",
            static_cast<void (cGtarch::*)(uint, char**)>(&cGtarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cGtarch::*)(uint, std::string[])>(&cGtarch::GetParamName))
        ;
}
