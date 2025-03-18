#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

static void LinReg_Print(cLinReg& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cLinReg()
{
    class_<cLinReg, bases<cAbstCondMean> >("cLinReg",
        "Linear Regression mean component",
        init< optional<int> >(
            (boost::python::arg("theNLinReg") = 0),
            "cLinReg(int theNLinReg=0)"
        )
    )
        .def("Delete", &cLinReg::Delete)
        .def("Print", &LinReg_Print)

        // Overloaded ReAlloc
        .def("ReAllocVect",
            static_cast<void (cLinReg::*)(const cDVector&, uint)>(&cLinReg::ReAlloc))
        .def("ReAllocSize",
            static_cast<void (cLinReg::*)(uint, uint)>(&cLinReg::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cLinReg::*)(double, uint, uint)>(&cLinReg::Set))
        .def("SetVectorIndex",
            static_cast<void (cLinReg::*)(const cDVector&, uint)>(&cLinReg::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cLinReg::*)(uint, uint)>(&cLinReg::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cLinReg::*)(uint)>(&cLinReg::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get by name
        .def("SetValueName",
            static_cast<void (cLinReg::*)(double, std::string, uint)>(&cLinReg::Set))
        .def("SetVectorName",
            static_cast<void (cLinReg::*)(const cDVector&, std::string)>(&cLinReg::Set))
        .def("GetValueName",
            static_cast<double (cLinReg::*)(std::string, uint)>(&cLinReg::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cLinReg::*)(std::string)>(&cLinReg::Get),
            return_value_policy<reference_existing_object>())

        .def("SetDefaultInitPoint",
            static_cast<void (cLinReg::*)(double, double)>(&cLinReg::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cLinReg::*)(cRegArchValue&)>(&cLinReg::SetDefaultInitPoint))

        .def("ComputeMean", &cLinReg::ComputeMean)
        .def("GetNParam", &cLinReg::GetNParam)
        .def("GetNLags", &cLinReg::GetNLags)
        .def("RegArchParamToVector", &cLinReg::RegArchParamToVector)
        .def("VectorToRegArchParam", &cLinReg::VectorToRegArchParam)
        .def("ComputeGrad", &cLinReg::ComputeGrad)
        .def("ComputeHess", &cLinReg::ComputeHess)
        .def("ComputeGradAndHess", &cLinReg::ComputeGradAndHess)

        .def("GetParamNameChar",
            static_cast<void (cLinReg::*)(uint, char**)>(&cLinReg::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cLinReg::*)(uint, std::string[])>(&cLinReg::GetParamName))
        ;
}
