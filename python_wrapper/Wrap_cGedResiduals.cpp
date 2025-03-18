#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

static void GedResiduals_Print(cGedResiduals& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cGedResiduals()
{
    class_<cGedResiduals, bases<cAbstResiduals> >("cGedResiduals",
        "GED residuals with shape parameter Beta",
        init< optional<double, bool> >(
            (boost::python::arg("theBeta") = 2.0, boost::python::arg("theSimulFlag") = false),
            "cGedResiduals(double beta=2.0, bool simulFlag=false)"
        )
    )
        .def("Print", &GedResiduals_Print)
        .def("SetDefaultInitPoint", &cGedResiduals::SetDefaultInitPoint)
        .def("Generate", &cGedResiduals::Generate)
        .def("LogDensity", &cGedResiduals::LogDensity)
        .def("GetNParam", &cGedResiduals::GetNParam)
        .def("DiffLogDensity", &cGedResiduals::DiffLogDensity)
        .def("ComputeGrad", &cGedResiduals::ComputeGrad)
        .def("RegArchParamToVector", &cGedResiduals::RegArchParamToVector)
        .def("VectorToRegArchParam", &cGedResiduals::VectorToRegArchParam)
        .def("ComputeEspAbsEps", &cGedResiduals::ComputeEspAbsEps)
        .def("ComputeGradBetaEspAbsEps", &cGedResiduals::ComputeGradBetaEspAbsEps)
        .def("ComputeHessBetaEspAbsEps", &cGedResiduals::ComputeHessBetaEspAbsEps)
        .def("Diff2LogDensity", &cGedResiduals::Diff2LogDensity)
        .def("GradDiffLogDensity", &cGedResiduals::GradDiffLogDensity)
        .def("ComputeHess", &cGedResiduals::ComputeHess)
        .def("ComputeGradAndHess", &cGedResiduals::ComputeGradAndHess)
        .def("GetParamNameChar",
            static_cast<void (cGedResiduals::*)(uint, char**)>(&cGedResiduals::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cGedResiduals::*)(uint, std::string[])>(&cGedResiduals::GetParamName))
        ;
}
