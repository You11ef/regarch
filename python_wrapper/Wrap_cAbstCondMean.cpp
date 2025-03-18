#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

/*!
 * A trampoline class to wrap cAbstCondMean in Boost.Python.
 */
struct cAbstCondMeanWrap : public cAbstCondMean, wrapper<cAbstCondMean>
{
    // Constructor matching cAbstCondMean(eCondMeanEnum)
    cAbstCondMeanWrap(eCondMeanEnum theType = eUnknown)
        : cAbstCondMean(theType)
    {}

    // Overriding Print(std::ostream&) by calling Python "print" method if defined.
    virtual void Print(ostream& theOut) const override
    {
        if (override f = this->get_override("print"))
            f(); // ignoring theOut in the bridge
        else
            throw std::runtime_error("Python class must implement 'print' method");
    }

    // Default implementation for Print
    void default_Print(ostream& theOut) const
    {
        throw std::runtime_error("Python class must implement 'print' method");
    }

    // Convenience method for Python to call Print(std::cout)
    void PrintPython()
    {
        Print(std::cout);
    }

    virtual void Delete(void) override
    {
        if (override f = this->get_override("delete"))
            f();
        else
            throw std::runtime_error("Python class must implement 'delete' method");
    }

    void default_Delete()
    {
        throw std::runtime_error("Python class must implement 'delete' method");
    }

    //--- Overridden pure virtual "Set(...)" methods ---
    virtual void Set(const double theValue, const uint theIndex, const uint theNumParam = 0) override
    {
        if (override f = this->get_override("set_value_by_index"))
            f(theValue, theIndex, theNumParam);
        else
            throw std::runtime_error("Python class must implement 'set_value_by_index' method");
    }

    virtual void Set(const cDVector& theVectParam, const uint theNumParam = 0) override
    {
        if (override f = this->get_override("set_vector_by_index"))
            f(boost::cref(theVectParam), theNumParam);
        else
            throw std::runtime_error("Python class must implement 'set_vector_by_index' method");
    }

    virtual void Set(const double theValue, std::string theNameParam, const uint theIndex = 0) override
    {
        if (override f = this->get_override("set_value_by_name"))
            f(theValue, theNameParam, theIndex);
        else
            throw std::runtime_error("Python class must implement 'set_value_by_name' method");
    }

    virtual void Set(const cDVector& theVectParam, std::string theNameParam) override
    {
        if (override f = this->get_override("set_vector_by_name"))
            f(boost::cref(theVectParam), theNameParam);
        else
            throw std::runtime_error("Python class must implement 'set_vector_by_name' method");
    }

    //--- Overridden pure virtual "Get(...)" methods ---
    virtual double Get(const uint theIndex, const uint theNumParam) override
    {
        if (override f = this->get_override("get_value_by_index"))
            return f(theIndex, theNumParam);
        throw std::runtime_error("Python class must implement 'get_value_by_index' method");
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
        throw std::runtime_error("Python class must implement 'get_vector_by_index' method");
    }

    virtual double Get(std::string theNameParam, const uint theIndex) override
    {
        if (override f = this->get_override("get_value_by_name"))
            return f(theNameParam, theIndex);
        throw std::runtime_error("Python class must implement 'get_value_by_name' method");
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
        throw std::runtime_error("Python class must implement 'get_vector_by_name' method");
    }

    //--- Overridden pure virtual "ReAlloc(...)" methods ---
    virtual void ReAlloc(const uint theSize, const uint theNumParam = 0) override
    {
        if (override f = this->get_override("realloc_by_size"))
            f(theSize, theNumParam);
        else
            throw std::runtime_error("Python class must implement 'realloc_by_size' method");
    }

    virtual void ReAlloc(const cDVector& theVectParam, const uint theNumParam = 0) override
    {
        if (override f = this->get_override("realloc_by_vector"))
            f(boost::cref(theVectParam), theNumParam);
        else
            throw std::runtime_error("Python class must implement 'realloc_by_vector' method");
    }

    virtual void ReAllocProxyMeanParameters(uint theOldNParam = 0) override
    {
        if (override f = this->get_override("realloc_proxy_mean_parameters"))
            f(theOldNParam);
        else
            throw std::runtime_error("Python class must implement 'realloc_proxy_mean_parameters' method");
    }

    virtual void UpdateProxyMeanParameters(void) override
    {
        if (override f = this->get_override("update_proxy_mean_parameters"))
            f();
        else
            throw std::runtime_error("Python class must implement 'update_proxy_mean_parameters' method");
    }

    //--- Other pure virtuals ---
    virtual double ComputeMean(uint theDate, const cRegArchValue& theData) const override
    {
        if (override f = this->get_override("compute_mean"))
            return f(theDate, boost::cref(theData));
        throw std::runtime_error("Python class must implement 'compute_mean' method");
    }

    virtual uint GetNParam(void) const override
    {
        if (override f = this->get_override("get_n_param"))
            return f();
        throw std::runtime_error("Python class must implement 'get_n_param' method");
    }

    virtual uint GetNLags(void) const override
    {
        if (override f = this->get_override("get_n_lags"))
            return f();
        throw std::runtime_error("Python class must implement 'get_n_lags' method");
    }

    virtual void ComputeGrad(uint theDate, const cRegArchValue& theData, cRegArchGradient& theGradData, uint theBegIndex) override
    {
        if (override f = this->get_override("compute_grad"))
            f(theDate, boost::cref(theData), boost::ref(theGradData), theBegIndex);
        else
            throw std::runtime_error("Python class must implement 'compute_grad' method");
    }

    virtual void RegArchParamToVector(cDVector& theDestVect, uint theIndex = 0) override
    {
        if (override f = this->get_override("reg_arch_param_to_vector"))
            f(boost::ref(theDestVect), theIndex);
        else
            throw std::runtime_error("Python class must implement 'reg_arch_param_to_vector' method");
    }

    virtual void VectorToRegArchParam(const cDVector& theSrcVect, uint theIndex = 0) override
    {
        if (override f = this->get_override("vector_to_reg_arch_param"))
            f(boost::cref(theSrcVect), theIndex);
        else
            throw std::runtime_error("Python class must implement 'vector_to_reg_arch_param' method");
    }

    virtual void SetDefaultInitPoint(double theMean, double theVar) override
    {
        if (override f = this->get_override("set_default_init_point_mean_var"))
            f(theMean, theVar);
        else
            throw std::runtime_error("Python class must implement 'set_default_init_point_mean_var' method");
    }

    virtual void SetDefaultInitPoint(cRegArchValue& theValue) override
    {
        if (override f = this->get_override("set_default_init_point_value"))
            f(boost::ref(theValue));
        else
            throw std::runtime_error("Python class must implement 'set_default_init_point_value' method");
    }

    virtual void ComputeHess(uint theDate, const cRegArchValue& theData, const cRegArchGradient& theGradData, cRegArchHessien& theHessData, uint theBegIndex) override
    {
        if (override f = this->get_override("compute_hess"))
            f(theDate, boost::cref(theData), boost::cref(theGradData), boost::ref(theHessData), theBegIndex);
        else
            throw std::runtime_error("Python class must implement 'compute_hess' method");
    }

    virtual void ComputeGradAndHess(uint theDate, const cRegArchValue& theData, cRegArchGradient& theGradData, cRegArchHessien& theHessData, uint theBegIndex) override
    {
        if (override f = this->get_override("compute_grad_and_hess"))
            f(theDate, boost::cref(theData), boost::ref(theGradData), boost::ref(theHessData), theBegIndex);
        else
            throw std::runtime_error("Python class must implement 'compute_grad_and_hess' method");
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
            throw std::runtime_error("Python class must implement 'get_param_name_string' method");
    }
};

// Helper function to get parameter names as a Python list
boost::python::list get_param_names_py(cAbstCondMean& self, uint theIndex)
{
    // Let's assume the max number of parameter names is 10 (adjust as needed) 
    std::string names[10];
    self.GetParamName(theIndex, names);

    boost::python::list result;
    // We don't know how many names were actually filled, so we'll add non-empty ones
    for (int i = 0; i < 10; i++)
    {
        if (!names[i].empty())
            result.append(names[i]);
    }
    return result;
}

void export_cAbstCondMean()
{
    // Enum for conditional mean types
    enum_<eCondMeanEnum>("CondMeanEnum")
        .value("eUnknown", eUnknown)
        .value("eConst", eConst)
        .value("eAr", eAr)
        // Add other enum values as needed -- to be done
        .export_values()
        ;

    class_<cAbstCondMeanWrap, boost::noncopyable>("cAbstCondMean",
        "Abstract base class for conditional mean models in RegArch.\n"
        "This class defines the interface for all conditional mean components.",
        init<optional<eCondMeanEnum> >(
            (boost::python::arg("mean_type") = eUnknown),
            "Initialize a conditional mean model with the specified type."
        )
    )
        // Non-virtual methods
        .def("get_cond_mean_type", &cAbstCondMean::GetCondMeanType,
            "Returns the enumeration value representing the conditional mean type.")
        .def("set_cond_mean_type", &cAbstCondMean::SetCondMeanType,
            boost::python::arg("mean_type"),
            "Sets the conditional mean type enumeration value.")
        .def("exist_param_name", &cAbstCondMean::ExistParamName,
            boost::python::arg("param_name"),
            "Checks if a parameter with the given name exists.")
        .def("get_num_param_from_name", &cAbstCondMean::GetNumParamFromName,
            boost::python::arg("param_name"),
            "Gets the numerical parameter index from its name.")
        .def("print", &cAbstCondMeanWrap::PrintPython,
            "Prints the model parameters to standard output.")

        // Helper method for getting parameter names
        .def("get_param_names", &get_param_names_py,
            boost::python::arg("index"),
            "Returns a list of parameter names for the given index.")

        // Pure virtual methods with Pythonic names
        .def("delete", pure_virtual(&cAbstCondMean::Delete),
            "Deletes the model and frees associated memory.")

        // Set methods
        .def("set",
            pure_virtual(static_cast<void (cAbstCondMean::*)(double, uint, uint)>(&cAbstCondMean::Set)),
            (boost::python::arg("value"), boost::python::arg("index"), boost::python::arg("param_num") = 0),
            "Sets a parameter value by its numerical index.")
        .def("set",
            pure_virtual(static_cast<void (cAbstCondMean::*)(const cDVector&, uint)>(&cAbstCondMean::Set)),
            (boost::python::arg("values"), boost::python::arg("param_num") = 0),
            "Sets multiple parameter values from a vector by numerical index.")
        .def("set",
            pure_virtual(static_cast<void (cAbstCondMean::*)(double, std::string, uint)>(&cAbstCondMean::Set)),
            (boost::python::arg("value"), boost::python::arg("param_name"), boost::python::arg("index") = 0),
            "Sets a parameter value by its name.")
        .def("set",
            pure_virtual(static_cast<void (cAbstCondMean::*)(const cDVector&, std::string)>(&cAbstCondMean::Set)),
            (boost::python::arg("values"), boost::python::arg("param_name")),
            "Sets multiple parameter values from a vector by parameter name.")

        // Get methods
        .def("get",
            pure_virtual(static_cast<double (cAbstCondMean::*)(uint, uint)>(&cAbstCondMean::Get)),
            (boost::python::arg("index"), boost::python::arg("param_num")),
            "Gets a parameter value by its numerical index.")
        .def("get",
            pure_virtual(static_cast<cDVector & (cAbstCondMean::*)(uint)>(&cAbstCondMean::Get)),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_num"),
            "Gets all parameter values for a given parameter number as a vector.")
        .def("get",
            pure_virtual(static_cast<double (cAbstCondMean::*)(std::string, uint)>(&cAbstCondMean::Get)),
            (boost::python::arg("param_name"), boost::python::arg("index")),
            "Gets a parameter value by its name and index.")
        .def("get",
            pure_virtual(static_cast<cDVector & (cAbstCondMean::*)(std::string)>(&cAbstCondMean::Get)),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Gets all parameter values for a given parameter name as a vector.")

        // ReAlloc methods
        .def("realloc",
            pure_virtual(static_cast<void (cAbstCondMean::*)(uint, uint)>(&cAbstCondMean::ReAlloc)),
            (boost::python::arg("size"), boost::python::arg("param_num") = 0),
            "Reallocates memory for a parameter of the specified size.")
        .def("realloc",
            pure_virtual(static_cast<void (cAbstCondMean::*)(const cDVector&, uint)>(&cAbstCondMean::ReAlloc)),
            (boost::python::arg("values"), boost::python::arg("param_num") = 0),
            "Reallocates memory for a parameter based on the vector's size.")
        .def("realloc_proxy_mean_parameters",
            pure_virtual(&cAbstCondMean::ReAllocProxyMeanParameters),
            (boost::python::arg("old_n_param") = 0),
            "Reallocates proxy mean parameters.")
        .def("update_proxy_mean_parameters",
            pure_virtual(&cAbstCondMean::UpdateProxyMeanParameters),
            "Updates proxy mean parameters.")

        // Other methods
        .def("compute_mean",
            pure_virtual(&cAbstCondMean::ComputeMean),
            (boost::python::arg("date"), boost::python::arg("data")),
            "Computes the conditional mean at the specified date given the data.")
        .def("get_n_param",
            pure_virtual(&cAbstCondMean::GetNParam),
            "Returns the number of parameters in the model.")
        .def("get_n_lags",
            pure_virtual(&cAbstCondMean::GetNLags),
            "Returns the number of lags required for gradient computation.")
        .def("compute_grad",
            pure_virtual(&cAbstCondMean::ComputeGrad),
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("beg_index")),
            "Computes the gradient of the model at the specified date.")
        .def("reg_arch_param_to_vector",
            pure_virtual(&cAbstCondMean::RegArchParamToVector),
            (boost::python::arg("dest_vect"), boost::python::arg("index") = 0),
            "Converts model parameters to a vector representation.")
        .def("vector_to_reg_arch_param",
            pure_virtual(&cAbstCondMean::VectorToRegArchParam),
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Converts a vector representation back to model parameters.")

        // SetDefaultInitPoint methods
        .def("set_default_init_point",
            pure_virtual(static_cast<void (cAbstCondMean::*)(double, double)>(&cAbstCondMean::SetDefaultInitPoint)),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Sets default initial parameter values based on mean and variance.")
        .def("set_default_init_point",
            pure_virtual(static_cast<void (cAbstCondMean::*)(cRegArchValue&)>(&cAbstCondMean::SetDefaultInitPoint)),
            boost::python::arg("value"),
            "Sets default initial parameter values based on RegArch values.")

        // Hessian computation methods
        .def("compute_hess",
            pure_virtual(&cAbstCondMean::ComputeHess),
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("beg_index")),
            "Computes the Hessian of the model at the specified date.")
        .def("compute_grad_and_hess",
            pure_virtual(&cAbstCondMean::ComputeGradAndHess),
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("beg_index")),
            "Computes both gradient and Hessian in one operation.")
        ;
}