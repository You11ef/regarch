#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

static void Figarch_Print(cFigarch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cFigarch()
{
    class_<cFigarch, bases<cAbstCondVar> >("cFigarch",
        "FIGARCH(p,d,q) conditional variance model.",
        init< optional<uint, uint, double, uint> >(
            (boost::python::arg("theNArch") = 0,
                boost::python::arg("theNGarch") = 0,
                boost::python::arg("theFracD") = 0.0,
                boost::python::arg("theNTruncLag") = 20),
            "cFigarch(uint p=0, uint q=0, double d=0.0, uint nTrunc=20)"
        )
    )
        .def("Delete", &cFigarch::Delete)
        .def("Print", &Figarch_Print)

        .def("SetDefaultInitPoint",
            static_cast<void (cFigarch::*)(double, double)>(&cFigarch::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cFigarch::*)(cRegArchValue&)>(&cFigarch::SetDefaultInitPoint))

        // Overloaded ReAlloc
        .def("ReAlloc",
            static_cast<void (cFigarch::*)(uint, uint)>(&cFigarch::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cFigarch::*)(const cDVector&, uint)>(&cFigarch::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cFigarch::*)(double, uint, uint)>(&cFigarch::Set))
        .def("SetVectorIndex",
            static_cast<void (cFigarch::*)(const cDVector&, uint)>(&cFigarch::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cFigarch::*)(uint, uint)>(&cFigarch::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cFigarch::*)(uint)>(&cFigarch::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get with string
        .def("SetValueName",
            static_cast<void (cFigarch::*)(double, std::string, uint)>(&cFigarch::Set))
        .def("SetVectorName",
            static_cast<void (cFigarch::*)(const cDVector&, std::string)>(&cFigarch::Set))
        .def("GetValueName",
            static_cast<double (cFigarch::*)(std::string, uint)>(&cFigarch::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cFigarch::*)(std::string)>(&cFigarch::Get),
            return_value_policy<reference_existing_object>())

        .def("ComputeVar", &cFigarch::ComputeVar)
        .def("GetNParam", &cFigarch::GetNParam)
        .def("GetNLags", &cFigarch::GetNLags)
        .def("RegArchParamToVector", &cFigarch::RegArchParamToVector)
        .def("VectorToRegArchParam", &cFigarch::VectorToRegArchParam)
        .def("ComputeGrad", &cFigarch::ComputeGrad)
        .def("ComputeHess", &cFigarch::ComputeHess)
        .def("ComputeGradAndHess", &cFigarch::ComputeGradAndHess)

        // Possibly expose ReAllocProxyVarParameters, UpdateProxyVarParameters, etc.
        .def("ReAllocProxyVarParameters", &cFigarch::ReAllocProxyVarParameters)
        .def("UpdateProxyVarParameters", &cFigarch::UpdateProxyVarParameters)

        .def("GetParamNameChar",
            static_cast<void (cFigarch::*)(uint, char**)>(&cFigarch::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cFigarch::*)(uint, std::string[])>(&cFigarch::GetParamName))
        ;
}
