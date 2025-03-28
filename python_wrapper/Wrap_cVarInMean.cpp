#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

static void VarInMean_Print(cVarInMean& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cVarInMean()
{
    class_<cVarInMean, bases<cAbstCondMean> >("cVarInMean",
        "Variance-in-Mean model component",
        init< optional<double> >(
            (boost::python::arg("theVarInMean") = 0.0),
            "cVarInMean(double theVarInMean=0.0)"
        )
    )
        .def("Delete", &cVarInMean::Delete)
        .def("Print", &VarInMean_Print)
        .def("SetDefaultInitPoint",
            static_cast<void (cVarInMean::*)(double, double)>(&cVarInMean::SetDefaultInitPoint),
            (boost::python::arg("theMean"), boost::python::arg("theVar")))
        .def("SetDefaultInitPointValue",
            static_cast<void (cVarInMean::*)(cRegArchValue&)>(&cVarInMean::SetDefaultInitPoint))
        .def("ReAlloc",
            static_cast<void (cVarInMean::*)(uint, uint)>(&cVarInMean::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cVarInMean::*)(const cDVector&, uint)>(&cVarInMean::ReAlloc))
        .def("SetValueIndex",
            static_cast<void (cVarInMean::*)(double, uint, uint)>(&cVarInMean::Set))
        .def("SetVectorIndex",
            static_cast<void (cVarInMean::*)(const cDVector&, uint)>(&cVarInMean::Set))
        .def("GetValueIndex",
            static_cast<double (cVarInMean::*)(uint, uint)>(&cVarInMean::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cVarInMean::*)(uint)>(&cVarInMean::Get),
            return_value_policy<reference_existing_object>())
        .def("SetValueName",
            static_cast<void (cVarInMean::*)(double, std::string, uint)>(&cVarInMean::Set))
        .def("SetVectorName",
            static_cast<void (cVarInMean::*)(const cDVector&, std::string)>(&cVarInMean::Set))
        .def("GetValueName",
            static_cast<double (cVarInMean::*)(std::string, uint)>(&cVarInMean::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cVarInMean::*)(std::string)>(&cVarInMean::Get),
            return_value_policy<reference_existing_object>())
        .def("ComputeMean", &cVarInMean::ComputeMean)
        .def("GetNParam", &cVarInMean::GetNParam)
        .def("GetNLags", &cVarInMean::GetNLags)
        .def("ComputeGrad", &cVarInMean::ComputeGrad)
        .def("ComputeHess", &cVarInMean::ComputeHess)
        .def("ComputeGradAndHess", &cVarInMean::ComputeGradAndHess)
        .def("RegArchParamToVector", &cVarInMean::RegArchParamToVector)
        .def("VectorToRegArchParam", &cVarInMean::VectorToRegArchParam)
        .def("GetParamNameChar",
            static_cast<void (cVarInMean::*)(uint, char**)>(&cVarInMean::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cVarInMean::*)(uint, std::string[])>(&cVarInMean::GetParamName))
        ;
}
