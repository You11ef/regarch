#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

static void Garch_Print(cGarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cGarch()
{
    class_<cGarch, bases<cAbstCondVar> >("cGarch",
        "GARCH(p,q) variance model.",
        init< optional<uint, uint> >(
            (boost::python::arg("theNArch") = 0, boost::python::arg("theNGarch") = 0),
            "cGarch(uint p=0, uint q=0)"
        )
    )
        .def("Delete", &cGarch::Delete)
        .def("Print", &Garch_Print)

        .def("SetDefaultInitPoint",
            static_cast<void (cGarch::*)(double, double)>(&cGarch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cGarch::*)(cRegArchValue&)>(&cGarch::SetDefaultInitPoint))

        // ReAlloc
        .def("ReAlloc",
            static_cast<void (cGarch::*)(uint, uint)>(&cGarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cGarch::*)(const cDVector&, uint)>(&cGarch::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cGarch::*)(double, uint, uint)>(&cGarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cGarch::*)(const cDVector&, uint)>(&cGarch::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cGarch::*)(uint, uint)>(&cGarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cGarch::*)(uint)>(&cGarch::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get by name
        .def("SetValueName",
            static_cast<void (cGarch::*)(double, std::string, uint)>(&cGarch::Set))
        .def("SetVectorName",
            static_cast<void (cGarch::*)(const cDVector&, std::string)>(&cGarch::Set))
        .def("GetValueName",
            static_cast<double (cGarch::*)(std::string, uint)>(&cGarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cGarch::*)(std::string)>(&cGarch::Get),
            return_value_policy<reference_existing_object>())

        .def("ComputeVar", &cGarch::ComputeVar)
        .def("GetNParam", &cGarch::GetNParam)
        .def("GetNLags", &cGarch::GetNLags)
        .def("ComputeGrad", &cGarch::ComputeGrad)
        .def("RegArchParamToVector", &cGarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cGarch::VectorToRegArchParam)
        .def("ComputeHess", &cGarch::ComputeHess)
        .def("ComputeGradAndHess", &cGarch::ComputeGradAndHess)

        .def("GetParamNameChar",
            static_cast<void (cGarch::*)(uint, char**)>(&cGarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cGarch::*)(uint, std::string[])>(&cGarch::GetParamName))
        ;
}
