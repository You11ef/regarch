#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

static void Sqrgarch_Print(cSqrgarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cSqrgarch()
{
    class_<cSqrgarch, bases<cAbstCondVar> >("cSqrgarch",
        "SQR-GARCH(p,q) model",
        init< optional<uint, uint> >(
            (boost::python::arg("theNArch") = 0, boost::python::arg("theNGarch") = 0),
            "cSqrgarch(uint p=0, uint q=0)"
        )
    )
        .def("Delete", &cSqrgarch::Delete)
        .def("Print", &Sqrgarch_Print)

        .def("SetDefaultInitPoint",
            static_cast<void (cSqrgarch::*)(double, double)>(&cSqrgarch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cSqrgarch::*)(cRegArchValue&)>(&cSqrgarch::SetDefaultInitPoint))

        // ReAlloc
        .def("ReAlloc",
            static_cast<void (cSqrgarch::*)(uint, uint)>(&cSqrgarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cSqrgarch::*)(const cDVector&, uint)>(&cSqrgarch::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cSqrgarch::*)(double, uint, uint)>(&cSqrgarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cSqrgarch::*)(const cDVector&, uint)>(&cSqrgarch::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cSqrgarch::*)(uint, uint)>(&cSqrgarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cSqrgarch::*)(uint)>(&cSqrgarch::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get by name
        .def("SetValueName",
            static_cast<void (cSqrgarch::*)(double, std::string, uint)>(&cSqrgarch::Set))
        .def("SetVectorName",
            static_cast<void (cSqrgarch::*)(const cDVector&, std::string)>(&cSqrgarch::Set))
        .def("GetValueName",
            static_cast<double (cSqrgarch::*)(std::string, uint)>(&cSqrgarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cSqrgarch::*)(std::string)>(&cSqrgarch::Get),
            return_value_policy<reference_existing_object>())

        .def("ComputeVar", &cSqrgarch::ComputeVar)
        .def("GetNParam", &cSqrgarch::GetNParam)
        .def("GetNLags", &cSqrgarch::GetNLags)
        .def("ComputeGrad", &cSqrgarch::ComputeGrad)
        .def("RegArchParamToVector", &cSqrgarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cSqrgarch::VectorToRegArchParam)
        .def("ComputeHess", &cSqrgarch::ComputeHess)
        .def("ComputeGradAndHess", &cSqrgarch::ComputeGradAndHess)
        .def("SyncSizeParam", &cSqrgarch::SyncSizeParam)

        .def("GetParamNameChar",
            static_cast<void (cSqrgarch::*)(uint, char**)>(&cSqrgarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cSqrgarch::*)(uint, std::string[])>(&cSqrgarch::GetParamName))
        ;
}
