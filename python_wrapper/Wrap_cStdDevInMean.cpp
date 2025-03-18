#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

static void StdDevInMean_Print(cStdDevInMean& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cStdDevInMean()
{
    class_<cStdDevInMean, bases<cAbstCondMean> >("cStdDevInMean",
        "Standard-Deviation-in-Mean component",
        init< optional<double> >(
            (boost::python::arg("theStdDevInMean") = 0.0),
            "cStdDevInMean(double theStdDevInMean=0.0)"
        )
    )
        .def("Delete", &cStdDevInMean::Delete)
        .def("Print", &StdDevInMean_Print)

        .def("SetDefaultInitPoint",
            static_cast<void (cStdDevInMean::*)(double, double)>(&cStdDevInMean::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue",
            static_cast<void (cStdDevInMean::*)(cRegArchValue&)>(&cStdDevInMean::SetDefaultInitPoint))

        // Overloaded ReAlloc
        .def("ReAlloc",
            static_cast<void (cStdDevInMean::*)(uint, uint)>(&cStdDevInMean::ReAlloc))
        .def("ReAllocVect",
            static_cast<void (cStdDevInMean::*)(const cDVector&, uint)>(&cStdDevInMean::ReAlloc))

        // Overloaded Set
        .def("SetValueIndex",
            static_cast<void (cStdDevInMean::*)(double, uint, uint)>(&cStdDevInMean::Set))
        .def("SetVectorIndex",
            static_cast<void (cStdDevInMean::*)(const cDVector&, uint)>(&cStdDevInMean::Set))

        // Overloaded Get
        .def("GetValueIndex",
            static_cast<double (cStdDevInMean::*)(uint, uint)>(&cStdDevInMean::Get))
        .def("GetVectorIndex",
            static_cast<cDVector & (cStdDevInMean::*)(uint)>(&cStdDevInMean::Get),
            return_value_policy<reference_existing_object>())

        // Overloaded set/get with string
        .def("SetValueName",
            static_cast<void (cStdDevInMean::*)(double, std::string, uint)>(&cStdDevInMean::Set))
        .def("SetVectorName",
            static_cast<void (cStdDevInMean::*)(const cDVector&, std::string)>(&cStdDevInMean::Set))
        .def("GetValueName",
            static_cast<double (cStdDevInMean::*)(std::string, uint)>(&cStdDevInMean::Get))
        .def("GetVectorName",
            static_cast<cDVector & (cStdDevInMean::*)(std::string)>(&cStdDevInMean::Get),
            return_value_policy<reference_existing_object>())

        .def("ComputeMean", &cStdDevInMean::ComputeMean)
        .def("GetNParam", &cStdDevInMean::GetNParam)
        .def("GetNLags", &cStdDevInMean::GetNLags)
        .def("RegArchParamToVector", &cStdDevInMean::RegArchParamToVector)
        .def("VectorToRegArchParam", &cStdDevInMean::VectorToRegArchParam)
        .def("ComputeGrad", &cStdDevInMean::ComputeGrad)
        .def("ComputeHess", &cStdDevInMean::ComputeHess)
        .def("ComputeGradAndHess", &cStdDevInMean::ComputeGradAndHess)

        .def("GetParamNameChar",
            static_cast<void (cStdDevInMean::*)(uint, char**)>(&cStdDevInMean::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cStdDevInMean::*)(uint, std::string[])>(&cStdDevInMean::GetParamName))
        ;
}
