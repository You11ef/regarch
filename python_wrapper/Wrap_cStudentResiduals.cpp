#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>


using namespace boost::python;
using namespace RegArchLib;
using namespace VectorAndMatrixNameSpace;

/*!
 * Factory function that creates a new cStudentResiduals from a Python list or tuple
 * e.g. [5.0], converting it to a cDVector pointer.
 */
cStudentResiduals* CreateStudentResidualsFromVector(object pyParam, bool simulFlag)
{
    // Convert the Python list/tuple to cDVector by value:
    cDVector vec = extract<cDVector>(pyParam);
    // Construct a new cStudentResiduals with a pointer to a copy:
    cDVector* heapVec = new cDVector(vec);
    return new cStudentResiduals(heapVec, simulFlag);
}

static void StudentResiduals_Print(cStudentResiduals& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cStudentResiduals()
{
    // Normal constructor: (double dof, bool simulFlag)
    class_<cStudentResiduals, bases<cAbstResiduals> >("cStudentResiduals",
        "Student residuals with unitary variance.\n"
        "Call cStudentResiduals(dof, bool) directly.\n"
        "If you have a param vector, use CreateStudentResidualsFromVector(...) instead.",
        init< optional<double, bool> >(
            (boost::python::arg("theDof") = 0.0, boost::python::arg("theSimulFlag") = true),
            "cStudentResiduals(double theDof=0.0, bool theSimulFlag=true)"
        )
    )
        .def("Print", &StudentResiduals_Print)
        .def("SetDefaultInitPoint", &cStudentResiduals::SetDefaultInitPoint)
        .def("Generate", &cStudentResiduals::Generate)
        .def("LogDensity", &cStudentResiduals::LogDensity)
        .def("GetNParam", &cStudentResiduals::GetNParam)
        .def("DiffLogDensity", &cStudentResiduals::DiffLogDensity)
        .def("ComputeGrad", &cStudentResiduals::ComputeGrad)
        .def("RegArchParamToVector", &cStudentResiduals::RegArchParamToVector)
        .def("VectorToRegArchParam", &cStudentResiduals::VectorToRegArchParam)
        .def("ComputeEspAbsEps", &cStudentResiduals::ComputeEspAbsEps)
        .def("ComputeGradBetaEspAbsEps", &cStudentResiduals::ComputeGradBetaEspAbsEps)
        .def("ComputeHessBetaEspAbsEps", &cStudentResiduals::ComputeHessBetaEspAbsEps)
        .def("Diff2LogDensity", &cStudentResiduals::Diff2LogDensity)
        .def("GradDiffLogDensity", &cStudentResiduals::GradDiffLogDensity)
        .def("ComputeHess", &cStudentResiduals::ComputeHess)
        .def("ComputeGradAndHess", &cStudentResiduals::ComputeGradAndHess)
        .def("GetParamNameChar",
            static_cast<void (cStudentResiduals::*)(uint, char**)>(&cStudentResiduals::GetParamName))
        .def("GetParamNameString",
            static_cast<void (cStudentResiduals::*)(uint, std::string[])>(&cStudentResiduals::GetParamName))
        ;

    // Free function for param-vector approach:
    def("CreateStudentResidualsFromVector",
        &CreateStudentResidualsFromVector,
        return_value_policy<manage_new_object>(), // IMPORTANT: returning a raw pointer
        (boost::python::arg("pyParam"), boost::python::arg("simulFlag") = false),
        "Create a new cStudentResiduals from a Python list/tuple param, e.g. [5.0].\n"
        "Usage:\n"
        "  stud = regarch_wrapper.CreateStudentResidualsFromVector([5.0], False)\n"
    );
}
