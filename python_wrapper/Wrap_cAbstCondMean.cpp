/******************************************************************************
 * File: Wrap_cAbstCondMean.cpp
 * ---------------------------------------------------------------------------
 * This file wraps the abstract conditional mean class (cAbstCondMean)
 * for use with Boost.Python, allowing Python classes to inherit from it
 * or letting users create derived C++ instances from Python, set parameters, etc.
 *****************************************************************************/

#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

 // 1) Include our new helper header:
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;


/*!
 * A trampoline class to wrap cAbstCondMean in Boost.Python.
 * This allows Python classes to inherit from cAbstCondMean and override its methods.
 */
struct cAbstCondMeanWrap : public cAbstCondMean, wrapper<cAbstCondMean>
{
    // Constructor matching cAbstCondMean(eCondMeanEnum)
    cAbstCondMeanWrap(eCondMeanEnum theType = eUnknown)
        : cAbstCondMean(theType)
    {}

    // For pure virtual methods, we need a default implementation for when Python
    // doesn't override them. These implementations will throw exceptions.

    // Overriding Print(std::ostream&) 
    virtual void Print(ostream& theOut) const override
    {
        if (override f = this->get_override("print"))
            f(); // ignoring theOut in the bridge
        else
            throw std::runtime_error("Implementation needed: You must override the 'print' method in your Python class");
    }

    // Convenience method for Python to call Print
    void PrintPython()
    {
        std::ostringstream oss;
        Print(oss);
        // Python code can call this method instead
        std::cout << oss.str() << std::endl;
    }

    // Delete is a pure virtual method
    virtual void Delete(void) override
    {
        if (override f = this->get_override("delete"))
            f();
        else
            throw std::runtime_error("Implementation needed: You must override the 'delete' method in your Python class");
    }

    //--- Overridden pure virtual "Set(...)" methods ---
    virtual void Set(const double theValue, const uint theIndex, const uint theNumParam = 0) override
    {
        if (override f = this->get_override("set_value_by_index"))
            f(theValue, theIndex, theNumParam);
        else
            throw std::runtime_error("Implementation needed: You must override the 'set_value_by_index' method in your Python class");
    }

    virtual void Set(const cDVector& theVectParam, const uint theNumParam = 0) override
    {
        if (override f = this->get_override("set_vector_by_index"))
            f(boost::cref(theVectParam), theNumParam);
        else
            throw std::runtime_error("Implementation needed: You must override the 'set_vector_by_index' method in your Python class");
    }

    virtual void Set(const double theValue, std::string theNameParam, const uint theIndex = 0) override
    {
        if (override f = this->get_override("set_value_by_name"))
            f(theValue, theNameParam, theIndex);
        else
            throw std::runtime_error("Implementation needed: You must override the 'set_value_by_name' method in your Python class");
    }

    virtual void Set(const cDVector& theVectParam, std::string theNameParam) override
    {
        if (override f = this->get_override("set_vector_by_name"))
            f(boost::cref(theVectParam), theNameParam);
        else
            throw std::runtime_error("Implementation needed: You must override the 'set_vector_by_name' method in your Python class");
    }

    //--- Overridden pure virtual "Get(...)" methods ---
    virtual double Get(const uint theIndex, const uint theNumParam) override
    {
        if (override f = this->get_override("get_value_by_index"))
            return f(theIndex, theNumParam);
        throw std::runtime_error("Implementation needed: You must override the 'get_value_by_index' method in your Python class");
    }

    virtual cDVector& Get(const uint theNumParam) override
    {
        if (override f = this->get_override("get_vector_by_index"))
        {
            try {
                object pyObj = f(theNumParam);
                return extract<cDVector&>(pyObj);
            }
            catch (error_already_set&) {
                PyErr_Print();
                throw std::runtime_error("Failed to extract cDVector from Python return value");
            }
        }
        throw std::runtime_error("Implementation needed: You must override the 'get_vector_by_index' method in your Python class");
    }

    virtual double Get(std::string theNameParam, const uint theIndex) override
    {
        if (override f = this->get_override("get_value_by_name"))
            return f(theNameParam, theIndex);
        throw std::runtime_error("Implementation needed: You must override the 'get_value_by_name' method in your Python class");
    }

    virtual cDVector& Get(std::string theNameParam) override
    {
        if (override f = this->get_override("get_vector_by_name"))
        {
            try {
                object pyObj = f(theNameParam);
                return extract<cDVector&>(pyObj);
            }
            catch (error_already_set&) {
                PyErr_Print();
                throw std::runtime_error("Failed to extract cDVector from Python return value");
            }
        }
        throw std::runtime_error("Implementation needed: You must override the 'get_vector_by_name' method in your Python class");
    }

    //--- Overridden pure virtual "ReAlloc(...)" methods ---
    virtual void ReAlloc(const uint theSize, const uint theNumParam = 0) override
    {
        if (override f = this->get_override("realloc_by_size"))
            f(theSize, theNumParam);
        else
            throw std::runtime_error("Implementation needed: You must override the 'realloc_by_size' method in your Python class");
    }

    virtual void ReAlloc(const cDVector& theVectParam, const uint theNumParam = 0) override
    {
        if (override f = this->get_override("realloc_by_vector"))
            f(boost::cref(theVectParam), theNumParam);
        else
            throw std::runtime_error("Implementation needed: You must override the 'realloc_by_vector' method in your Python class");
    }

    virtual void ReAllocProxyMeanParameters(uint theOldNParam = 0) override
    {
        if (override f = this->get_override("realloc_proxy_mean_parameters"))
            f(theOldNParam);
        else
            throw std::runtime_error("Implementation needed: You must override the 'realloc_proxy_mean_parameters' method in your Python class");
    }

    virtual void UpdateProxyMeanParameters(void) override
    {
        if (override f = this->get_override("update_proxy_mean_parameters"))
            f();
        else
            throw std::runtime_error("Implementation needed: You must override the 'update_proxy_mean_parameters' method in your Python class");
    }

    //--- Other pure virtuals ---
    virtual double ComputeMean(uint theDate, const cRegArchValue& theData) const override
    {
        if (override f = this->get_override("compute_mean"))
            return f(theDate, boost::cref(theData));
        throw std::runtime_error("Implementation needed: You must override the 'compute_mean' method in your Python class");
    }

    virtual uint GetNParam(void) const override
    {
        if (override f = this->get_override("get_n_param"))
            return f();
        throw std::runtime_error("Implementation needed: You must override the 'get_n_param' method in your Python class");
    }

    virtual uint GetNLags(void) const override
    {
        if (override f = this->get_override("get_n_lags"))
            return f();
        throw std::runtime_error("Implementation needed: You must override the 'get_n_lags' method in your Python class");
    }

    virtual void ComputeGrad(uint theDate, const cRegArchValue& theData, cRegArchGradient& theGradData, uint theBegIndex) override
    {
        if (override f = this->get_override("compute_grad"))
            f(theDate, boost::cref(theData), boost::ref(theGradData), theBegIndex);
        else
            throw std::runtime_error("Implementation needed: You must override the 'compute_grad' method in your Python class");
    }

    virtual void RegArchParamToVector(cDVector& theDestVect, uint theIndex = 0) override
    {
        if (override f = this->get_override("reg_arch_param_to_vector"))
            f(boost::ref(theDestVect), theIndex);
        else
            throw std::runtime_error("Implementation needed: You must override the 'reg_arch_param_to_vector' method in your Python class");
    }

    virtual void VectorToRegArchParam(const cDVector& theSrcVect, uint theIndex = 0) override
    {
        if (override f = this->get_override("vector_to_reg_arch_param"))
            f(boost::cref(theSrcVect), theIndex);
        else
            throw std::runtime_error("Implementation needed: You must override the 'vector_to_reg_arch_param' method in your Python class");
    }

    virtual void SetDefaultInitPoint(double theMean, double theVar) override
    {
        if (override f = this->get_override("set_default_init_point_mean_var"))
            f(theMean, theVar);
        else
            throw std::runtime_error("Implementation needed: You must override the 'set_default_init_point_mean_var' method in your Python class");
    }

    virtual void SetDefaultInitPoint(cRegArchValue& theValue) override
    {
        if (override f = this->get_override("set_default_init_point_value"))
            f(boost::ref(theValue));
        else
            throw std::runtime_error("Implementation needed: You must override the 'set_default_init_point_value' method in your Python class");
    }

    virtual void ComputeHess(uint theDate, const cRegArchValue& theData, const cRegArchGradient& theGradData,
        cRegArchHessien& theHessData, uint theBegIndex) override
    {
        if (override f = this->get_override("compute_hess"))
            f(theDate, boost::cref(theData), boost::cref(theGradData), boost::ref(theHessData), theBegIndex);
        else
            throw std::runtime_error("Implementation needed: You must override the 'compute_hess' method in your Python class");
    }

    virtual void ComputeGradAndHess(uint theDate, const cRegArchValue& theData, cRegArchGradient& theGradData,
        cRegArchHessien& theHessData, uint theBegIndex) override
    {
        if (override f = this->get_override("compute_grad_and_hess"))
            f(theDate, boost::cref(theData), boost::ref(theGradData), boost::ref(theHessData), theBegIndex);
        else
            throw std::runtime_error("Implementation needed: You must override the 'compute_grad_and_hess' method in your Python class");
    }

    virtual void GetParamName(uint theIndex, char** theName) override
    {
        // This is a C++ only implementation - not exposed to Python
        std::string names[10];  // Assuming max 10 names
        GetParamName(theIndex, names);

        // Convert std::string to char*
        for (int i = 0; i < 10; i++) {
            if (!names[i].empty() && theName[i] != nullptr) {
                strncpy(theName[i], names[i].c_str(), names[i].length() + 1);
            }
        }
    }

    virtual void GetParamName(uint theIndex, std::string theName[]) override
    {
        if (override f = this->get_override("get_param_name_string"))
        {
            // Call Python implementation
            f(theIndex, theName);
        }
        else
            throw std::runtime_error("Implementation needed: You must override the 'get_param_name_string' method in your Python class");
    }
};

// Helper function to get parameter names as a Python list
boost::python::list get_param_names_py(cAbstCondMean& self, uint theIndex)
{
    std::string names[10];
    self.GetParamName(theIndex, names);

    boost::python::list result;
    for (int i = 0; i < 10; i++)
    {
        if (!names[i].empty())
            result.append(names[i]);
    }
    return result;
}

// -------------------------------------------------------------------------
// Modified cAbstCondMean_set(...) with fallback for Python list/tuple
// -------------------------------------------------------------------------
void cAbstCondMean_set(cAbstCondMean& self, const object& arg1, const object& arg2, const object& arg3 = object())
{
    extract<cDVector> vector_extract(arg1);
    extract<double>   value_extract(arg1);

    // Check if second argument is a string or a uint
    extract<std::string> name_extract(arg2);
    extract<uint>        index_extract(arg2);

    // First try direct conversions
    if (vector_extract.check())
    {
        // The user gave us a cDVector from Python side
        cDVector vector = vector_extract();

        if (name_extract.check()) {
            std::string name = name_extract();
            self.Set(vector, name);
        }
        else if (index_extract.check()) {
            uint param_num = index_extract();
            self.Set(vector, param_num);
        }
        else {
            throw std::runtime_error("Second argument must be either a parameter name (string) or parameter number (uint)");
        }
    }
    else if (value_extract.check())
    {
        // The user gave us a scalar (double)
        double value = value_extract();

        if (name_extract.check()) {
            std::string name = name_extract();
            uint index = 0;
            if (arg3 != object()) {
                index = extract<uint>(arg3);
            }
            self.Set(value, name, index);
        }
        else if (index_extract.check()) {
            uint index = index_extract();
            uint param_num = 0;
            if (arg3 != object()) {
                param_num = extract<uint>(arg3);
            }
            self.Set(value, index, param_num);
        }
        else {
            throw std::runtime_error("Second argument must be either a parameter name (string) or parameter index (uint)");
        }
    }
    else
    {
        // Try list/tuple fallback
        if (PyList_Check(arg1.ptr()) || PyTuple_Check(arg1.ptr()))
        {
            // Convert the python sequence to cDVector:
            cDVector vector = py_list_or_tuple_to_cDVector(arg1);

            if (name_extract.check()) {
                self.Set(vector, name_extract());
            }
            else if (index_extract.check()) {
                self.Set(vector, index_extract());
            }
            else {
                throw std::runtime_error("Second argument must be either a parameter name (string) or parameter number (uint)");
            }
        }
        else {
            throw std::runtime_error("First argument must be float, cDVector, or a Python list/tuple of floats");
        }
    }
}

// Helper function to create a unified 'get' method with appropriate Python behavior
object cAbstCondMean_get(cAbstCondMean& self, const object& arg1, const object& arg2 = object())
{
    extract<std::string> name_extract(arg1);
    extract<uint>        param_extract(arg1);

    if (name_extract.check())
    {
        // Getting by name
        std::string name = name_extract();
        if (arg2 == object())
        {
            // Return the entire vector
            try {
                return object(boost::ref(self.Get(name)));
            }
            catch (const std::exception& e) {
                PyErr_SetString(PyExc_KeyError,
                    (std::string("Parameter name not found: ") + name).c_str());
                throw_error_already_set();
                return object();
            }
        }
        else
        {
            // Return a specific value by index
            uint index = extract<uint>(arg2);
            try {
                return object(self.Get(name, index));
            }
            catch (const std::exception& e) {
                PyErr_SetString(PyExc_IndexError,
                    (std::string("Parameter or index not found: ") + name +
                        ", index " + std::to_string(index)).c_str());
                throw_error_already_set();
                return object();
            }
        }
    }
    else if (param_extract.check())
    {
        // Getting by param_num
        uint param_num = param_extract();
        if (arg2 == object())
        {
            // Return the entire vector
            try {
                return object(boost::ref(self.Get(param_num)));
            }
            catch (const std::exception& e) {
                PyErr_SetString(PyExc_IndexError,
                    (std::string("Parameter number not found: ") +
                        std::to_string(param_num)).c_str());
                throw_error_already_set();
                return object();
            }
        }
        else
        {
            // Return a specific value by index
            uint index = extract<uint>(arg2);
            try {
                return object(self.Get(index, param_num));
            }
            catch (const std::exception& e) {
                PyErr_SetString(PyExc_IndexError,
                    (std::string("Parameter or index not found: param ") +
                        std::to_string(param_num) + ", index " +
                        std::to_string(index)).c_str());
                throw_error_already_set();
                return object();
            }
        }
    }
    else
    {
        throw std::runtime_error("First argument must be either a parameter name (string) or parameter number (uint)");
    }
}

void export_cAbstCondMean()
{
    class_<cAbstCondMeanWrap, boost::noncopyable>("cAbstCondMean",
        // docstring omitted for brevity...
        init<optional<eCondMeanEnum> >((boost::python::arg("mean_type") = eUnknown))
    )
        // Non-virtual methods
        .def("get_cond_mean_type", &cAbstCondMean::GetCondMeanType)
        .def("set_cond_mean_type", &cAbstCondMean::SetCondMeanType, boost::python::arg("mean_type"))
        .def("exist_param_name", &cAbstCondMean::ExistParamName, boost::python::arg("param_name"))
        .def("get_num_param_from_name", &cAbstCondMean::GetNumParamFromName, boost::python::arg("param_name"))
        .def("print", &cAbstCondMeanWrap::PrintPython)

        // Helper method for getting parameter names
        .def("get_param_names", &get_param_names_py, boost::python::arg("index"))

        // Unified methods for Python-friendly interface
        .def("set", &cAbstCondMean_set,
            (boost::python::arg("arg1"), boost::python::arg("arg2"), boost::python::arg("arg3") = object()))
        .def("get", &cAbstCondMean_get,
            (boost::python::arg("arg1"), boost::python::arg("arg2") = object()))

        // Pure virtual methods with Pythonic names
        .def("delete", pure_virtual(&cAbstCondMean::Delete))

        // ReAlloc methods
        .def("realloc",
            pure_virtual(static_cast<void (cAbstCondMean::*)(uint, uint)>(&cAbstCondMean::ReAlloc)),
            (boost::python::arg("size"), boost::python::arg("param_num") = 0))
        .def("realloc",
            pure_virtual(static_cast<void (cAbstCondMean::*)(const cDVector&, uint)>(&cAbstCondMean::ReAlloc)),
            (boost::python::arg("values"), boost::python::arg("param_num") = 0))
        .def("realloc_proxy_mean_parameters",
            pure_virtual(&cAbstCondMean::ReAllocProxyMeanParameters),
            (boost::python::arg("old_n_param") = 0))
        .def("update_proxy_mean_parameters",
            pure_virtual(&cAbstCondMean::UpdateProxyMeanParameters))

        .def("compute_mean",
            pure_virtual(&cAbstCondMean::ComputeMean),
            (boost::python::arg("date"), boost::python::arg("data")))
        .def("get_n_param",
            pure_virtual(&cAbstCondMean::GetNParam))
        .def("get_n_lags",
            pure_virtual(&cAbstCondMean::GetNLags))
        .def("compute_grad",
            pure_virtual(&cAbstCondMean::ComputeGrad),
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("beg_index")))
        .def("reg_arch_param_to_vector",
            pure_virtual(&cAbstCondMean::RegArchParamToVector),
            (boost::python::arg("dest_vect"), boost::python::arg("index") = 0))
        .def("vector_to_reg_arch_param",
            pure_virtual(&cAbstCondMean::VectorToRegArchParam),
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0))

        // SetDefaultInitPoint methods
        .def("set_default_init_point",
            pure_virtual(static_cast<void (cAbstCondMean::*)(double, double)>(&cAbstCondMean::SetDefaultInitPoint)),
            (boost::python::arg("mean"), boost::python::arg("var")))
        .def("set_default_init_point",
            pure_virtual(static_cast<void (cAbstCondMean::*)(cRegArchValue&)>(&cAbstCondMean::SetDefaultInitPoint)),
            boost::python::arg("value"))

        // Hessian computation methods
        .def("compute_hess",
            pure_virtual(&cAbstCondMean::ComputeHess),
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("beg_index")))
        .def("compute_grad_and_hess",
            pure_virtual(&cAbstCondMean::ComputeGradAndHess),
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("beg_index")))
        ;
}
