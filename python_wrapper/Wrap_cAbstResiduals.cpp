#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

/*!
 * A trampoline class to wrap cAbstResiduals in Boost.Python.
 */
struct cAbstResidualsWrap : public cAbstResiduals, wrapper<cAbstResiduals>
{
    // Constructor matching: cAbstResiduals(eDistrTypeEnum, cDVector*, bool)
    cAbstResidualsWrap(eDistrTypeEnum theDistr, cDVector* theDistrParam = 0, bool theSimulFlag = false)
        : cAbstResiduals(theDistr, theDistrParam, theSimulFlag)
    {}

    // Override Print(ostream&) const.
    virtual void Print(ostream& theOut) const override {
        if (override f = this->get_override("PrintImpl"))
            f(); // ignoring theOut for the Python bridge
    }
    // Convenience method for Python users.
    void PrintPython() {
        Print(std::cout);
    }

    //--- Overridden pure virtual methods ---
    virtual void Generate(const uint theNSample, cDVector& theYt) const override {
        if (override f = this->get_override("Generate"))
            f(theNSample, boost::ref(theYt));
    }

    virtual double LogDensity(double theX) const override {
        if (override f = this->get_override("LogDensity"))
            return f(theX);
        return 0.0;
    }

    virtual uint GetNParam(void) const override {
        if (override f = this->get_override("GetNParam"))
            return f();
        return 0;
    }

    virtual double DiffLogDensity(double theX) const override {
        if (override f = this->get_override("DiffLogDensity"))
            return f(theX);
        return 0.0;
    }

    virtual void ComputeGrad(uint theDate, const cRegArchValue& theData, cRegArchGradient& theGradData) const override {
        if (override f = this->get_override("ComputeGrad"))
            f(theDate, boost::cref(theData), boost::ref(theGradData));
    }

    virtual void RegArchParamToVector(cDVector& theDestVect, uint theIndex) const override {
        if (override f = this->get_override("RegArchParamToVector"))
            f(boost::ref(theDestVect), theIndex);
    }

    virtual void VectorToRegArchParam(const cDVector& theSrcVect, uint theIndex = 0) override {
        if (override f = this->get_override("VectorToRegArchParam"))
            f(boost::cref(theSrcVect), theIndex);
    }

    virtual double ComputeEspAbsEps(void) override {
        if (override f = this->get_override("ComputeEspAbsEps"))
            return f();
        return 0.0;
    }

    virtual void ComputeGradBetaEspAbsEps(cDVector& theGrad) override {
        if (override f = this->get_override("ComputeGradBetaEspAbsEps"))
            f(boost::ref(theGrad));
    }

    virtual void ComputeHessBetaEspAbsEps(cDMatrix& theHess) override {
        if (override f = this->get_override("ComputeHessBetaEspAbsEps"))
            f(boost::ref(theHess));
    }

    virtual void SetDefaultInitPoint(void) override {
        if (override f = this->get_override("SetDefaultInitPoint"))
            f();
    }

    virtual double Diff2LogDensity(double theX) const override {
        if (override f = this->get_override("Diff2LogDensity"))
            return f(theX);
        return 0.0;
    }

    virtual void GradDiffLogDensity(double theX, const cDVector& theDistrParam, cDVector& theGrad) override {
        if (override f = this->get_override("GradDiffLogDensity"))
            f(theX, boost::cref(theDistrParam), boost::ref(theGrad));
    }

    virtual void ComputeHess(uint theDate, const cRegArchValue& theData, const cRegArchGradient& theGradData, cRegArchHessien& theHessData) override {
        if (override f = this->get_override("ComputeHess"))
            f(theDate, boost::cref(theData), boost::cref(theGradData), boost::ref(theHessData));
    }

    virtual void ComputeGradAndHess(uint theDate, const cRegArchValue& theData, cRegArchGradient& theGradData, cRegArchHessien& theHessData) override {
        if (override f = this->get_override("ComputeGradAndHess"))
            f(theDate, boost::cref(theData), boost::ref(theGradData), boost::ref(theHessData));
    }

    virtual void GetParamName(uint theIndex, char** theName) override {
        if (override f = this->get_override("GetParamNameChar"))
            f(theIndex, theName);
    }

    virtual void GetParamName(uint theIndex, std::string theName[]) override {
        if (override f = this->get_override("GetParamNameString"))
            f(theIndex, theName);
    }

    virtual void NumericComputeGrad(uint theDate, const cRegArchValue& theData,
        cRegArchGradient& theGradData, uint theBegIndex,
        cNumericDerivative& theNumDeriv) override {
        if (override f = this->get_override("NumericComputeGrad"))
            f(theDate, boost::cref(theData), boost::ref(theGradData), theBegIndex, boost::ref(theNumDeriv));
        else {
            cAbstResiduals::NumericComputeGrad(theDate, theData, theGradData, theBegIndex, theNumDeriv);
        }
    }
};

void export_cAbstResiduals()
{
    class_<cAbstResidualsWrap, boost::noncopyable>("cAbstResiduals",
        "Wrap of abstract cAbstResiduals class.",
        init<eDistrTypeEnum, cDVector*, bool>(
            (boost::python::arg("theDistr"),
                boost::python::arg("theDistrParam") = static_cast<cDVector*>(0),
                boost::python::arg("theSimulFlag") = false)
        )
    )
        // Expose non-virtual functions:
        .def("Delete", &cAbstResiduals::Delete)
        .def("GetDistrType", &cAbstResiduals::GetDistrType)
        .def("SetSimul", &cAbstResiduals::SetSimul)
        .def("IsForSimul", &cAbstResiduals::IsForSimul)
        .def("Get", &cAbstResiduals::Get)
        .def("ReAlloc", &cAbstResiduals::ReAlloc)
        .def("Set", &cAbstResiduals::Set)

        // Expose pure virtual functions via the trampoline:
        .def("Print", pure_virtual(&cAbstResidualsWrap::Print),
            "Print the residual parameters (calls Print(std::cout))")
        .def("Generate", pure_virtual(&cAbstResidualsWrap::Generate))
        .def("LogDensity", pure_virtual(&cAbstResidualsWrap::LogDensity))
        .def("GetNParam", pure_virtual(&cAbstResidualsWrap::GetNParam))
        .def("DiffLogDensity", pure_virtual(&cAbstResidualsWrap::DiffLogDensity))
        .def("ComputeGrad", pure_virtual(&cAbstResidualsWrap::ComputeGrad))
        .def("RegArchParamToVector", pure_virtual(&cAbstResidualsWrap::RegArchParamToVector))
        .def("VectorToRegArchParam", pure_virtual(&cAbstResidualsWrap::VectorToRegArchParam))
        .def("ComputeEspAbsEps", pure_virtual(&cAbstResidualsWrap::ComputeEspAbsEps))
        .def("ComputeGradBetaEspAbsEps", pure_virtual(&cAbstResidualsWrap::ComputeGradBetaEspAbsEps))
        .def("ComputeHessBetaEspAbsEps", pure_virtual(&cAbstResidualsWrap::ComputeHessBetaEspAbsEps))
        .def("SetDefaultInitPoint", pure_virtual(&cAbstResidualsWrap::SetDefaultInitPoint))
        .def("Diff2LogDensity", pure_virtual(&cAbstResidualsWrap::Diff2LogDensity))
        .def("GradDiffLogDensity", pure_virtual(&cAbstResidualsWrap::GradDiffLogDensity))
        .def("ComputeHess", pure_virtual(&cAbstResidualsWrap::ComputeHess))
        .def("ComputeGradAndHess", pure_virtual(&cAbstResidualsWrap::ComputeGradAndHess))
        .def("GetParamNameChar", pure_virtual(static_cast<void (cAbstResiduals::*)(uint, char**)>(&cAbstResiduals::GetParamName)))
        .def("GetParamNameString", pure_virtual(static_cast<void (cAbstResiduals::*)(uint, std::string[])>(&cAbstResiduals::GetParamName)))
        .def("NumericComputeGrad", pure_virtual(&cAbstResidualsWrap::NumericComputeGrad))
        .def("PrintPython", &cAbstResidualsWrap::PrintPython,
            "Convenience method to print residuals to stdout")
        ;

    // Expose factory functions with disambiguation and an explicit return value policy.
    def("CreateRealCondResiduals",
        static_cast<cAbstResiduals * (*)(const eDistrTypeEnum, cDVector*, bool)>(&CreateRealCondResiduals),
        (boost::python::arg("theDistr"),
            boost::python::arg("theDistrParam") = static_cast<cDVector*>(0),
            boost::python::arg("theSimulFlag") = false),
        return_value_policy<manage_new_object>());
    def("CreateRealCondResidualsCopy",
        static_cast<cAbstResiduals * (*)(cAbstResiduals&)>(&CreateRealCondResiduals),
        boost::python::arg("theAbstCondResiduals"),
        return_value_policy<manage_new_object>());

}
