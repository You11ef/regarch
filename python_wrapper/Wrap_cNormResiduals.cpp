#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

static void NormResiduals_Print(cNormResiduals& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cNormResiduals()
{
    class_<cNormResiduals, bases<cAbstResiduals> >("cNormResiduals",
        "Standard normal residuals (N(0,1))",
        init< optional<cDVector*, bool> >(
            (boost::python::arg("theDistrParameter") = (cDVector*)0,
                boost::python::arg("theSimulFlag") = false),
            "cNormResiduals(cDVector* param=nullptr, bool simulFlag=false)"
        )
    )
        .def("Print", &NormResiduals_Print)
        .def("SetDefaultInitPoint", &cNormResiduals::SetDefaultInitPoint)
        .def("Generate", &cNormResiduals::Generate)
        .def("LogDensity", &cNormResiduals::LogDensity)
        .def("GetNParam", &cNormResiduals::GetNParam)
        .def("DiffLogDensity", &cNormResiduals::DiffLogDensity)
        .def("ComputeGrad", &cNormResiduals::ComputeGrad)
        .def("RegArchParamToVector", &cNormResiduals::RegArchParamToVector)
        .def("VectorToRegArchParam", &cNormResiduals::VectorToRegArchParam)
        .def("ComputeEspAbsEps", &cNormResiduals::ComputeEspAbsEps)
        .def("ComputeGradBetaEspAbsEps", &cNormResiduals::ComputeGradBetaEspAbsEps)
        .def("ComputeHessBetaEspAbsEps", &cNormResiduals::ComputeHessBetaEspAbsEps)
        .def("Diff2LogDensity", &cNormResiduals::Diff2LogDensity)
        .def("GradDiffLogDensity", &cNormResiduals::GradDiffLogDensity)
        .def("ComputeHess", &cNormResiduals::ComputeHess)
        .def("ComputeGradAndHess", &cNormResiduals::ComputeGradAndHess)
        .def("GetParamNameChar",
            static_cast<void (cNormResiduals::*)(uint, char**)>(&cNormResiduals::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cNormResiduals::*)(uint, std::string[])>(&cNormResiduals::GetParamName))
        ;
}
