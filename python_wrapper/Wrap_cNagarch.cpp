#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

static void Nagarch_Print(cNagarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cNagarch()
{
    class_<cNagarch, bases<cAbstCondVar> >("cNagarch",
        "NAGARCH(p,q) variance model",
        init< optional<uint, uint> >(
            (boost::python::arg("theNArch") = 0, boost::python::arg("theNGarch") = 0),
            "cNagarch(uint p=0, uint q=0)"
        )
    )
        .def("Delete", &cNagarch::Delete)
        .def("Print", &Nagarch_Print)

        .def("SetDefaultInitPoint",
            static_cast<void (cNagarch::*)(double, double)>(&cNagarch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cNagarch::*)(cRegArchValue&)>(&cNagarch::SetDefaultInitPoint))

        // ReAlloc
        .def("ReAlloc",
            static_cast<void (cNagarch::*)(uint, uint)>(&cNagarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cNagarch::*)(const cDVector&, uint)>(&cNagarch::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cNagarch::*)(double, uint, uint)>(&cNagarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cNagarch::*)(const cDVector&, uint)>(&cNagarch::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cNagarch::*)(uint, uint)>(&cNagarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cNagarch::*)(uint)>(&cNagarch::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get by name
        .def("SetValueName",
            static_cast<void (cNagarch::*)(double, std::string, uint)>(&cNagarch::Set))
        .def("SetVectorName",
            static_cast<void (cNagarch::*)(const cDVector&, std::string)>(&cNagarch::Set))
        .def("GetValueName",
            static_cast<double (cNagarch::*)(std::string, uint)>(&cNagarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cNagarch::*)(std::string)>(&cNagarch::Get),
            return_value_policy<reference_existing_object>())

        .def("ComputeVar", &cNagarch::ComputeVar)
        .def("GetNParam", &cNagarch::GetNParam)
        .def("GetNLags", &cNagarch::GetNLags)
        .def("ComputeGrad", &cNagarch::ComputeGrad)
        .def("RegArchParamToVector", &cNagarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cNagarch::VectorToRegArchParam)
        .def("ComputeHess", &cNagarch::ComputeHess)
        .def("ComputeGradAndHess", &cNagarch::ComputeGradAndHess)
        .def("SyncSizeParam", &cNagarch::SyncSizeParam)

        .def("GetParamNameChar",
            static_cast<void (cNagarch::*)(uint, char**)>(&cNagarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cNagarch::*)(uint, std::string[])>(&cNagarch::GetParamName))
        ;
}
