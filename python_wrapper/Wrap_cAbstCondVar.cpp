#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

/*!
 * A trampoline class to wrap cAbstCondVar in Boost.Python.
 */
struct cAbstCondVarWrap : public cAbstCondVar, wrapper<cAbstCondVar>
{
    // Constructor matching cAbstCondVar(eCondVarEnum)
    cAbstCondVarWrap(eCondVarEnum theType = eNotKnown)
        : cAbstCondVar(theType)
    {}

    // Override Print(ostream&) to call Python "PrintImpl()" if defined.
    virtual void Print(ostream& theOut) const override
    {
        if (override f = this->get_override("PrintImpl"))
            f(); // ignoring theOut in the bridge
    }
    // Convenience method for Python to call Print(std::cout)
    void PrintPython()
    {
        Print(std::cout);
    }

    virtual void Delete(void) override
    {
        if (override f = this->get_override("Delete"))
            f();
    }

    //--- Overridden pure virtual "Set(...)" methods ---
    virtual void Set(const double theValue, const uint theIndex, const uint theNumParam = 0) override
    {
        if (override f = this->get_override("SetDoubleIndex"))
            f(theValue, theIndex, theNumParam);
    }
    virtual void Set(const cDVector& theVectParam, const uint theNumParam = 0) override
    {
        if (override f = this->get_override("SetVectorIndex"))
            f(boost::cref(theVectParam), theNumParam);
    }
    virtual void Set(const double theValue, std::string theNameParam, const uint theIndex = 0) override
    {
        if (override f = this->get_override("SetDoubleName"))
            f(theValue, theNameParam, theIndex);
    }
    virtual void Set(const cDVector& theVectParam, std::string theNameParam) override
    {
        if (override f = this->get_override("SetVectorName"))
            f(boost::cref(theVectParam), theNameParam);
    }

    //--- Overridden pure virtual "Get(...)" methods ---
    virtual double Get(const uint theIndex, const uint theNumParam) override
    {
        if (override f = this->get_override("GetIndex"))
            return f(theIndex, theNumParam);
        return 0.0;
    }
    virtual cDVector& Get(const uint theNumParam) override
    {
        if (override f = this->get_override("GetVector"))
        {
            object pyObj = f(theNumParam);
            return extract<cDVector&>(pyObj);
        }
        static cDVector dummy;
        return dummy;
    }
    virtual double Get(std::string theNameParam, const uint theIndex) override
    {
        if (override f = this->get_override("GetNameIndex"))
            return f(theNameParam, theIndex);
        return 0.0;
    }
    virtual cDVector& Get(std::string theNameParam) override
    {
        if (override f = this->get_override("GetName"))
        {
            object pyObj = f(theNameParam);
            return extract<cDVector&>(pyObj);
        }
        static cDVector dummy;
        return dummy;
    }

    //--- Overridden pure virtual "ReAlloc(...)" methods ---
    virtual void ReAlloc(const uint theSize, const uint theNumParam = 0) override
    {
        if (override f = this->get_override("ReAllocSize"))
            f(theSize, theNumParam);
    }
    virtual void ReAlloc(const cDVector& theVectParam, const uint theNumParam = 0) override
    {
        if (override f = this->get_override("ReAllocVector"))
            f(boost::cref(theVectParam), theNumParam);
    }
    virtual void ReAllocProxyVarParameters(uint theOldNParam = 0) override
    {
        if (override f = this->get_override("ReAllocProxyVarParameters"))
            f(theOldNParam);
    }
    virtual void SyncSizeParam(const uint theNumParam) override
    {
        if (override f = this->get_override("SyncSizeParam"))
            f(theNumParam);
    }
    virtual void UpdateProxyVarParameters(void) override
    {
        if (override f = this->get_override("UpdateProxyVarParameters"))
            f();
    }

    //--- Other pure virtuals ---
    virtual double ComputeVar(uint theDate, const cRegArchValue& theData) const override
    {
        if (override f = this->get_override("ComputeVar"))
            return f(theDate, boost::cref(theData));
        return 0.0;
    }
    virtual uint GetNParam(void) const override
    {
        if (override f = this->get_override("GetNParam"))
            return f();
        return 0;
    }
    virtual uint GetNLags(void) const override
    {
        if (override f = this->get_override("GetNLags"))
            return f();
        return 0;
    }
    virtual void ComputeGrad(uint theDate, const cRegArchValue& theData, cRegArchGradient& theGradData, cAbstResiduals* theResiduals = NULL) override
    {
        if (override f = this->get_override("ComputeGrad"))
            f(theDate, boost::cref(theData), boost::ref(theGradData), theResiduals);
    }
    virtual void RegArchParamToVector(cDVector& theDestVect, uint theIndex = 0) override
    {
        if (override f = this->get_override("RegArchParamToVector"))
            f(boost::ref(theDestVect), theIndex);
    }
    virtual void VectorToRegArchParam(const cDVector& theSrcVect, uint theIndex = 0) override
    {
        if (override f = this->get_override("VectorToRegArchParam"))
            f(boost::cref(theSrcVect), theIndex);
    }
    virtual void SetDefaultInitPoint(double theMean, double theVar) override
    {
        if (override f = this->get_override("SetDefaultInitPointMeanVar"))
            f(theMean, theVar);
    }
    virtual void SetDefaultInitPoint(cRegArchValue& theValue) override
    {
        if (override f = this->get_override("SetDefaultInitPointValue"))
            f(boost::ref(theValue));
    }
    virtual void ComputeHess(uint theDate, const cRegArchValue& theData, const cRegArchGradient& theGradData,
        cRegArchHessien& theHessData, cAbstResiduals* theResiduals = NULL) override
    {
        if (override f = this->get_override("ComputeHess"))
            f(theDate, boost::cref(theData), boost::cref(theGradData), boost::ref(theHessData), theResiduals);
    }
    virtual void ComputeGradAndHess(uint theDate, const cRegArchValue& theData, cRegArchGradient& theGradData,
        cRegArchHessien& theHessData, cAbstResiduals* theResiduals = NULL) override
    {
        if (override f = this->get_override("ComputeGradAndHess"))
            f(theDate, boost::cref(theData), boost::ref(theGradData), boost::ref(theHessData), theResiduals);
    }
    virtual void GetParamName(uint theIndex, char** theName) override
    {
        if (override f = this->get_override("GetParamNameChar"))
            f(theIndex, theName);
    }
    virtual void GetParamName(uint theIndex, std::string theName[]) override
    {
        if (override f = this->get_override("GetParamNameString"))
            f(theIndex, theName);
    }
};

void export_cAbstCondVar()
{
    class_<cAbstCondVarWrap, boost::noncopyable>("cAbstCondVar",
        "Wrap of abstract cAbstCondVar. Overloaded Set/Get need explicit casts.",
        init<optional<eCondVarEnum> >()
    )
        // Disambiguate the overloaded GetCondVarType by selecting the const version:
        .def("GetCondVarType", static_cast<eCondVarEnum(cAbstCondVar::*)() const>(&cAbstCondVar::GetCondVarType))
        .def("ExistParamName", &cAbstCondVar::ExistParamName)
        .def("GetNumParamFromName", &cAbstCondVar::GetNumParamFromName)
        .def("SetCondVarType", &cAbstCondVar::SetCondVarType)
        .def("Print", &cAbstCondVarWrap::PrintPython)

        // Pure virtual methods with explicit casts and, where needed, return value policies:
        .def("Delete", pure_virtual(&cAbstCondVarWrap::Delete))

        // Overloaded 'Set'
        .def("SetDoubleIndex",
            pure_virtual(static_cast<void (cAbstCondVar::*)(double, uint, uint)>(&cAbstCondVar::Set)),
            "(double theValue, uint theIndex, uint theNumParam=0)"
        )
        .def("SetVectorIndex",
            pure_virtual(static_cast<void (cAbstCondVar::*)(const cDVector&, uint)>(&cAbstCondVar::Set)),
            "(cDVector&, uint theNumParam=0)"
        )
        .def("SetDoubleName",
            pure_virtual(static_cast<void (cAbstCondVar::*)(double, std::string, uint)>(&cAbstCondVar::Set)),
            "(double, string, uint)"
        )
        .def("SetVectorName",
            pure_virtual(static_cast<void (cAbstCondVar::*)(const cDVector&, std::string)>(&cAbstCondVar::Set)),
            "(cDVector&, string)"
        )

        // Overloaded 'Get'
        .def("GetIndex",
            pure_virtual(static_cast<double (cAbstCondVar::*)(uint, uint)>(&cAbstCondVar::Get))
        )
        .def("GetVector",
            pure_virtual(static_cast<cDVector & (cAbstCondVar::*)(uint)>(&cAbstCondVar::Get)),
            return_value_policy<reference_existing_object>()
        )
        .def("GetNameIndex",
            pure_virtual(static_cast<double (cAbstCondVar::*)(std::string, uint)>(&cAbstCondVar::Get))
        )
        .def("GetName",
            pure_virtual(static_cast<cDVector & (cAbstCondVar::*)(std::string)>(&cAbstCondVar::Get)),
            return_value_policy<reference_existing_object>()
        )

        // Overloaded 'ReAlloc'
        .def("ReAllocSize",
            pure_virtual(static_cast<void (cAbstCondVar::*)(uint, uint)>(&cAbstCondVar::ReAlloc))
        )
        .def("ReAllocVector",
            pure_virtual(static_cast<void (cAbstCondVar::*)(const cDVector&, uint)>(&cAbstCondVar::ReAlloc))
        )
        .def("ReAllocProxyVarParameters", pure_virtual(&cAbstCondVar::ReAllocProxyVarParameters))
        .def("SyncSizeParam", pure_virtual(&cAbstCondVar::SyncSizeParam))
        .def("UpdateProxyVarParameters", pure_virtual(&cAbstCondVar::UpdateProxyVarParameters))

        // Other methods:
        .def("ComputeVar", pure_virtual(&cAbstCondVar::ComputeVar))
        .def("GetNParam", pure_virtual(&cAbstCondVar::GetNParam))
        .def("GetNLags", pure_virtual(&cAbstCondVar::GetNLags))
        .def("ComputeGrad", pure_virtual(&cAbstCondVar::ComputeGrad))
        .def("RegArchParamToVector", pure_virtual(&cAbstCondVar::RegArchParamToVector))
        .def("VectorToRegArchParam", pure_virtual(&cAbstCondVar::VectorToRegArchParam))
        .def("SetDefaultInitPointMeanVar",
            pure_virtual(static_cast<void (cAbstCondVar::*)(double, double)>(&cAbstCondVar::SetDefaultInitPoint))
        )
        .def("SetDefaultInitPointValue",
            pure_virtual(static_cast<void (cAbstCondVar::*)(cRegArchValue&)>(&cAbstCondVar::SetDefaultInitPoint))
        )
        .def("ComputeHess", pure_virtual(&cAbstCondVar::ComputeHess))
        .def("ComputeGradAndHess", pure_virtual(&cAbstCondVar::ComputeGradAndHess))

        // Overloaded GetParamName
        .def("GetParamNameChar",
            pure_virtual(static_cast<void (cAbstCondVar::*)(uint, char**)>(&cAbstCondVar::GetParamName))
        )
        .def("GetParamNameString",
            pure_virtual(static_cast<void (cAbstCondVar::*)(uint, std::string[])>(&cAbstCondVar::GetParamName))
        )
        ;
}
