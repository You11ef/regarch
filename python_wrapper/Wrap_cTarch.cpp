#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

static void Tarch_Print(cTarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cTarch()
{
    class_<cTarch, bases<cAbstCondVar> >("cTarch",
        "TARCH(p) conditional variance model",
        init< optional<int> >(
            (boost::python::arg("theNTarch") = 0),
            "cTarch(int theNTarch=0)"
        )
    )
        .def("Delete", &cTarch::Delete)
        .def("Print", &Tarch_Print)
        .def("SetDefaultInitPoint",
            static_cast<void (cTarch::*)(double, double)>(&cTarch::SetDefaultInitPoint),
            (boost::python::arg("theMean"), boost::python::arg("theVar")))
        .def("SetDefaultInitPointValue",
            static_cast<void (cTarch::*)(cRegArchValue&)>(&cTarch::SetDefaultInitPoint))
        .def("ReAlloc",
            static_cast<void (cTarch::*)(uint, uint)>(&cTarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cTarch::*)(const cDVector&, uint)>(&cTarch::ReAlloc))
        .def("SetValueIndex",
            static_cast<void (cTarch::*)(double, uint, uint)>(&cTarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cTarch::*)(const cDVector&, uint)>(&cTarch::Set))
        .def("GetValueIndex",
            static_cast<double (cTarch::*)(uint, uint)>(&cTarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cTarch::*)(uint)>(&cTarch::Get),
            return_value_policy<reference_existing_object>())
        .def("SetValueName",
            static_cast<void (cTarch::*)(double, std::string, uint)>(&cTarch::Set))
        .def("SetVectorName",
            static_cast<void (cTarch::*)(const cDVector&, std::string)>(&cTarch::Set))
        .def("GetValueName",
            static_cast<double (cTarch::*)(std::string, uint)>(&cTarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cTarch::*)(std::string)>(&cTarch::Get),
            return_value_policy<reference_existing_object>())
        .def("ComputeVar", &cTarch::ComputeVar)
        .def("GetNParam", &cTarch::GetNParam)
        .def("GetNLags", &cTarch::GetNLags)
        .def("ComputeGrad", &cTarch::ComputeGrad)
        .def("ComputeHess", &cTarch::ComputeHess)
        .def("ComputeGradAndHess", &cTarch::ComputeGradAndHess)
        .def("RegArchParamToVector", &cTarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cTarch::VectorToRegArchParam)
        .def("GetParamNameChar",
            static_cast<void (cTarch::*)(uint, char**)>(&cTarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cTarch::*)(uint, std::string[])>(&cTarch::GetParamName))
        ;
}
