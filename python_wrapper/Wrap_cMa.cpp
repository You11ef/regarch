#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>


using namespace boost::python;
using namespace RegArchLib;

static void Ma_Print(cMa& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cMa()
{
    class_<cMa, bases<cAbstCondMean> >("cMa",
        "Moving Average (MA) mean component",
        init< optional<uint> >(
            (boost::python::arg("theNMa") = 0),
            "cMa(uint theNMa=0)"
        )
    )
        .def("Delete", &cMa::Delete)
        .def("Print", &Ma_Print)

        .def("SetDefaultInitPoint",
            static_cast<void (cMa::*)(double, double)>(&cMa::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cMa::*)(cRegArchValue&)>(&cMa::SetDefaultInitPoint))

        // ReAlloc
        .def("ReAlloc",
            static_cast<void (cMa::*)(uint, uint)>(&cMa::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cMa::*)(const cDVector&, uint)>(&cMa::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cMa::*)(double, uint, uint)>(&cMa::Set))
        .def("SetVectorIndex",
            static_cast<void (cMa::*)(const cDVector&, uint)>(&cMa::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cMa::*)(uint, uint)>(&cMa::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cMa::*)(uint)>(&cMa::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get with string
        .def("SetValueName",
            static_cast<void (cMa::*)(double, std::string, uint)>(&cMa::Set))
        .def("SetVectorName",
            static_cast<void (cMa::*)(const cDVector&, std::string)>(&cMa::Set))
        .def("GetValueName",
            static_cast<double (cMa::*)(std::string, uint)>(&cMa::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cMa::*)(std::string)>(&cMa::Get),
            return_value_policy<reference_existing_object>())

        .def("ComputeMean", &cMa::ComputeMean)
        .def("GetNParam", &cMa::GetNParam)
        .def("GetNLags", &cMa::GetNLags)
        .def("ComputeGrad", &cMa::ComputeGrad)
        .def("ComputeHess", &cMa::ComputeHess)
        .def("ComputeGradAndHess", &cMa::ComputeGradAndHess)
        .def("RegArchParamToVector", &cMa::RegArchParamToVector)
        .def("VectorToRegArchParam", &cMa::VectorToRegArchParam)

        .def("GetParamNameChar",
            static_cast<void (cMa::*)(uint, char**)>(&cMa::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cMa::*)(uint, std::string[])>(&cMa::GetParamName))
        ;
}
