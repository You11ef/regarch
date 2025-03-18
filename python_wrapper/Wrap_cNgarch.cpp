#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>


using namespace boost::python;
using namespace RegArchLib;

static void Ngarch_Print(cNgarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cNgarch()
{
    class_<cNgarch, bases<cAbstCondVar> >("cNgarch",
        "NGARCH(p,q) variance model",
        init< optional<uint, uint> >(
            (boost::python::arg("theNArch") = 0, boost::python::arg("theNNgarch") = 0),
            "cNgarch(uint p=0, uint q=0)"
        )
    )
        .def("Delete", &cNgarch::Delete)
        .def("Print", &Ngarch_Print)

        .def("SetDefaultInitPoint",
            static_cast<void (cNgarch::*)(double, double)>(&cNgarch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cNgarch::*)(cRegArchValue&)>(&cNgarch::SetDefaultInitPoint))

        // ReAlloc
        .def("ReAlloc",
            static_cast<void (cNgarch::*)(uint, uint)>(&cNgarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cNgarch::*)(const cDVector&, uint)>(&cNgarch::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cNgarch::*)(double, uint, uint)>(&cNgarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cNgarch::*)(const cDVector&, uint)>(&cNgarch::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cNgarch::*)(uint, uint)>(&cNgarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cNgarch::*)(uint)>(&cNgarch::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get by name
        .def("SetValueName",
            static_cast<void (cNgarch::*)(double, std::string, uint)>(&cNgarch::Set))
        .def("SetVectorName",
            static_cast<void (cNgarch::*)(const cDVector&, std::string)>(&cNgarch::Set))
        .def("GetValueName",
            static_cast<double (cNgarch::*)(std::string, uint)>(&cNgarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cNgarch::*)(std::string)>(&cNgarch::Get),
            return_value_policy<reference_existing_object>())

        .def("ComputeVar", &cNgarch::ComputeVar)
        .def("GetNParam", &cNgarch::GetNParam)
        .def("GetNLags", &cNgarch::GetNLags)
        .def("ComputeGrad", &cNgarch::ComputeGrad)
        .def("RegArchParamToVector", &cNgarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cNgarch::VectorToRegArchParam)
        .def("ComputeHess", &cNgarch::ComputeHess)
        .def("ComputeGradAndHess", &cNgarch::ComputeGradAndHess)

        .def("GetParamNameChar",
            static_cast<void (cNgarch::*)(uint, char**)>(&cNgarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cNgarch::*)(uint, std::string[])>(&cNgarch::GetParamName))
        ;
}
