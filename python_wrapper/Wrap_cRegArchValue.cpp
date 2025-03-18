#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>


using namespace boost::python;
using namespace RegArchLib;

// We'll wrap ComputeMeanAndVar(...) and ComputeVar(...) to return doubles 
// or a Python tuple, because the real method uses references.
static boost::python::tuple cRegArchValue_ComputeMeanAndVar(cRegArchValue& self)
{
    double m = 0.0, v = 0.0;
    self.ComputeMeanAndVar(m, v);
    return boost::python::make_tuple(m, v);
}

static double cRegArchValue_ComputeVar(cRegArchValue& self)
{
    double v = 0.0;
    self.ComputeVar(v);
    return v;
}

// Wrap PrintValue to return a string - more pythonic approach
static std::string cRegArchValue_PrintValue(cRegArchValue& self, bool theHeader = true, const char* theSep = "\t")
{
    std::ostringstream oss;
    self.PrintValue(oss, theHeader, theSep);
    return oss.str();
}

// Add a string representation method for python
static std::string cRegArchValue_str(cRegArchValue& self)
{
    std::ostringstream oss;
    self.PrintValue(oss, true, "\t");
    return oss.str();
}

// Function to provide info about the object
static std::string cRegArchValue_repr(cRegArchValue& self)
{
    std::ostringstream oss;
    oss << "cRegArchValue with " << self.mYt.GetSize() << " observations";
    return oss.str();
}

void export_cRegArchValue()
{
    class_<cRegArchValue>("cRegArchValue",
        "Container for GARCH data, residuals, etc.",
        // Constructors - using NULL instead of nullptr
        init< optional<uint, cDMatrix*, cDMatrix*> >(
            (boost::python::arg("theSize") = 0, boost::python::arg("theXt") = (cDMatrix*)NULL, boost::python::arg("theXvt") = (cDMatrix*)NULL),
            "cRegArchValue(uint theSize=0, cDMatrix* theXt=nullptr, cDMatrix* theXvt=nullptr)"
        )
    )
        .def(init< optional<cDVector*, cDMatrix*, cDMatrix*> >(
            (boost::python::arg("theYt") = (cDVector*)NULL, boost::python::arg("theXt") = (cDMatrix*)NULL, boost::python::arg("theXvt") = (cDMatrix*)NULL),
            "cRegArchValue(cDVector* theYt=nullptr, cDMatrix* theXt=nullptr, cDMatrix* theXvt=nullptr)"
        ))

        // Public data
        .def_readwrite("mYt", &cRegArchValue::mYt, "Vector of data")
        .def_readwrite("mXt", &cRegArchValue::mXt, "Matrix of regressors if any")
        .def_readwrite("mXvt", &cRegArchValue::mXvt, "Matrix of regressors in the variance model if any")
        .def_readwrite("mMt", &cRegArchValue::mMt, "Vector of conditional mean")
        .def_readwrite("mHt", &cRegArchValue::mHt, "Vector of conditional variance")
        .def_readwrite("mUt", &cRegArchValue::mUt, "Vector of residuals")
        .def_readwrite("mEpst", &cRegArchValue::mEpst, "Vector of standardized residuals")

        // Methods
        .def("Delete", &cRegArchValue::Delete, "Delete all data and free memory")

        // ReAlloc methods with clear names
        .def("ReAlloc", (void (cRegArchValue::*)(uint)) & cRegArchValue::ReAlloc,
            (boost::python::arg("theSize")),
            "ReAlloc with a new size for Yt, Mt, etc.")

        .def("ReAlloc", (void (cRegArchValue::*)(cDVector&)) & cRegArchValue::ReAlloc,
            (boost::python::arg("theYt")),
            "ReAlloc with a cDVector for Yt.")

        .def("ReAllocXt", (void (cRegArchValue::*)(uint, uint)) & cRegArchValue::ReAllocXt,
            (boost::python::arg("theNRow"), boost::python::arg("theNCol")),
            "Reallocate the regressor matrix Xt with the specified dimensions")

        .def("ReAllocXt", (void (cRegArchValue::*)(cDMatrix&)) & cRegArchValue::ReAllocXt,
            (boost::python::arg("theXt")),
            "Reallocate the regressor matrix Xt using the provided matrix")

        .def("ReAllocXvt", (void (cRegArchValue::*)(uint, uint)) & cRegArchValue::ReAllocXvt,
            (boost::python::arg("theNRow"), boost::python::arg("theNCol")),
            "Reallocate the variance regressor matrix Xvt with the specified dimensions")

        .def("ReAllocXvt", (void (cRegArchValue::*)(cDMatrix&)) & cRegArchValue::ReAllocXvt,
            (boost::python::arg("theXvt")),
            "Reallocate the variance regressor matrix Xvt using the provided matrix")

        // Computation methods
        .def("ComputeMeanAndVar", &cRegArchValue_ComputeMeanAndVar,
            "Compute mean and variance, returns as (mean, variance) tuple")

        .def("ComputeVar", &cRegArchValue_ComputeVar,
            "Compute variance, returns as double")

        // PrintValue with better Python integration
        .def("PrintValue", &cRegArchValue_PrintValue,
            (boost::python::arg("theHeader") = true, boost::python::arg("theSep") = "\t"),
            "Return formatted data as a string")

        // String representation for Python
        .def("__str__", &cRegArchValue_str)
        .def("__repr__", &cRegArchValue_repr)
        ;
}