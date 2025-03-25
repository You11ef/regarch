/******************************************************************************
 * File: Wrap_cAbstCondVar.cpp
 * ---------------------------------------------------------------------------
 * This file wraps the abstract conditional variance class (cAbstCondVar)
 * for use with Boost.Python.
 *****************************************************************************/

#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

 // Include the new helper:
#include "PythonConversion.h"

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

    // Override Print(ostream&) to call Python "print" method if defined.
    virtual void Print(ostream& theOut) const override
    {
        if (override f = this->get_override("print"))
            f(); // ignoring theOut in the bridge
        else
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

    virtual void ReAllocProxyVarParameters(uint theOldNParam = 0) override
    {
        if (override f = this->get_override("realloc_proxy_var_parameters"))
            f(theOldNParam);
        else
            throw std::runtime_error("Python class must implement 'realloc_proxy_var_parameters' method");
    }

    virtual void SyncSizeParam(const uint theNumParam) override
    {
        if (override f = this->get_override("sync_size_param"))
            f(theNumParam);
        else
            throw std::runtime_error("Python class must implement 'sync_size_param' method");
    }

    virtual void UpdateProxyVarParameters(void) override
    {
        if (override f = this->get_override("update_proxy_var_parameters"))
            f();
        else
            throw std::runtime_error("Python class must implement 'update_proxy_var_parameters' method");
    }

    //--- Other pure virtuals ---
    virtual double ComputeVar(uint theDate, const cRegArchValue& theData) const override
    {
        if (override f = this->get_override("compute_var"))
            return f(theDate, boost::cref(theData));
        throw std::runtime_error("Python class must implement 'compute_var' method");
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

    virtual void ComputeGrad(uint theDate, const cRegArchValue& theData,
        cRegArchGradient& theGradData, cAbstResiduals* theResiduals = NULL) override
    {
        if (override f = this->get_override("compute_grad"))
            f(theDate, boost::cref(theData), boost::ref(theGradData), ptr(theResiduals));
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

    virtual void ComputeHess(uint theDate, const cRegArchValue& theData,
        const cRegArchGradient& theGradData, cRegArchHessien& theHessData,
        cAbstResiduals* theResiduals = NULL) override
    {
        if (override f = this->get_override("compute_hess"))
            f(theDate, boost::cref(theData), boost::cref(theGradData), boost::ref(theHessData), ptr(theResiduals));
        else
            throw std::runtime_error("Python class must implement 'compute_hess' method");
    }

    virtual void ComputeGradAndHess(uint theDate, const cRegArchValue& theData,
        cRegArchGradient& theGradData, cRegArchHessien& theHessData,
        cAbstResiduals* theResiduals = NULL) override
    {
        if (override f = this->get_override("compute_grad_and_hess"))
            f(theDate, boost::cref(theData), boost::ref(theGradData), boost::ref(theHessData), ptr(theResiduals));
        else
            throw std::runtime_error("Python class must implement 'compute_grad_and_hess' method");
    }

    virtual void GetParamName(uint theIndex, char** theName) override
    {
        if (override f = this->get_override("get_param_name_char"))
            f(theIndex, theName);
        else
            throw std::runtime_error("Python class must implement 'get_param_name_char' method");
    }

    virtual void GetParamName(uint theIndex, std::string theName[]) override
    {
        if (override f = this->get_override("get_param_name_string"))
            f(theIndex, theName);
        else
            throw std::runtime_error("Python class must implement 'get_param_name_string' method");
    }
};

// Helper function to get parameter names as a Python list
boost::python::list get_param_names_py(cAbstCondVar& self, uint theIndex)
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
// Modified "set" to handle Python list/tuple fallback
// -------------------------------------------------------------------------
void cAbstCondVar_set(cAbstCondVar& self, const object& arg1, const object& arg2, const object& arg3 = object())
{
    extract<cDVector> vector_extract(arg1);
    extract<double>   value_extract(arg1);

    // Check if second argument is a string or a uint
    extract<std::string> name_extract(arg2);
    extract<uint>        index_extract(arg2);

    if (vector_extract.check())
    {
        cDVector vector = vector_extract();
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
    else if (value_extract.check())
    {
        double value = value_extract();
        if (name_extract.check()) {
            uint idx = 0;
            if (arg3 != object()) {
                idx = extract<uint>(arg3);
            }
            self.Set(value, name_extract(), idx);
        }
        else if (index_extract.check()) {
            uint idx_param = 0;
            if (arg3 != object()) {
                idx_param = extract<uint>(arg3);
            }
            self.Set(value, index_extract(), idx_param);
        }
        else {
            throw std::runtime_error("Second argument must be either a parameter name (string) or parameter index (uint)");
        }
    }
    else
    {
        // fallback for python list or tuple
        if (PyList_Check(arg1.ptr()) || PyTuple_Check(arg1.ptr()))
        {
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
            throw std::runtime_error("First argument must be a float, cDVector, or a Python list/tuple of floats");
        }
    }
}

// -------------------------------------------------------------------------
// Modified "realloc" to handle Python list/tuple fallback
// -------------------------------------------------------------------------
void cAbstCondVar_realloc(cAbstCondVar& self, const object& arg1, const object& arg2 = object())
{
    extract<cDVector> vector_extract(arg1);
    extract<uint>     size_extract(arg1);

    uint param_num = 0;
    if (arg2 != object()) {
        param_num = extract<uint>(arg2);
    }

    if (vector_extract.check())
    {
        cDVector vector = vector_extract();
        self.ReAlloc(vector, param_num);
    }
    else if (size_extract.check())
    {
        uint size = size_extract();
        self.ReAlloc(size, param_num);
    }
    else
    {
        // fallback for python list or tuple
        if (PyList_Check(arg1.ptr()) || PyTuple_Check(arg1.ptr()))
        {
            cDVector vector = py_list_or_tuple_to_cDVector(arg1);
            self.ReAlloc(vector, param_num);
        }
        else {
            throw std::runtime_error("First argument must be a uint size, cDVector, or a Python list/tuple of floats");
        }
    }
}


object cAbstCondVar_get(cAbstCondVar& self, const object& arg1, const object& arg2 = object())
{
    extract<std::string> name_extract(arg1);
    extract<uint>        param_extract(arg1);

    if (name_extract.check())
    {
        std::string name = name_extract();
        if (arg2 == object()) {
            return object(boost::ref(self.Get(name)));
        }
        else {
            uint index = extract<uint>(arg2);
            return object(self.Get(name, index));
        }
    }
    else if (param_extract.check())
    {
        uint param_num = param_extract();
        if (arg2 == object()) {
            return object(boost::ref(self.Get(param_num)));
        }
        else {
            uint index = extract<uint>(arg2);
            return object(self.Get(index, param_num));
        }
    }
    else {
        throw std::runtime_error("First argument must be either a parameter name (string) or parameter number (uint)");
    }
}

void export_cAbstCondVar()
{
    class_<cAbstCondVarWrap, boost::noncopyable>("cAbstCondVar",
        init<optional<eCondVarEnum> >((boost::python::arg("var_type") = eNotKnown))
    )
        .def("get_cond_var_type",
            static_cast<eCondVarEnum(cAbstCondVar::*)() const>(&cAbstCondVar::GetCondVarType))
        .def("set_cond_var_type", &cAbstCondVar::SetCondVarType, boost::python::arg("var_type"))
        .def("exist_param_name", &cAbstCondVar::ExistParamName, boost::python::arg("param_name"))
        .def("get_num_param_from_name", &cAbstCondVar::GetNumParamFromName, boost::python::arg("param_name"))
        .def("print", &cAbstCondVarWrap::PrintPython)
        .def("get_param_names", &get_param_names_py, boost::python::arg("index"))

        // Unified "set" and "get"
        .def("set", &cAbstCondVar_set, (boost::python::arg("arg1"), boost::python::arg("arg2"), boost::python::arg("arg3") = object()))
        .def("get", &cAbstCondVar_get, (boost::python::arg("arg1"), boost::python::arg("arg2") = object()))
        .def("realloc", &cAbstCondVar_realloc, (boost::python::arg("arg1"), boost::python::arg("param_num") = object()))

        // pure virtual
        .def("delete", pure_virtual(&cAbstCondVar::Delete))
        .def("realloc_proxy_var_parameters",
            pure_virtual(&cAbstCondVar::ReAllocProxyVarParameters),
            (boost::python::arg("old_n_param") = 0))
        .def("sync_size_param",
            pure_virtual(&cAbstCondVar::SyncSizeParam),
            boost::python::arg("param_num"))
        .def("update_proxy_var_parameters",
            pure_virtual(&cAbstCondVar::UpdateProxyVarParameters))
        .def("compute_var",
            pure_virtual(&cAbstCondVar::ComputeVar),
            (boost::python::arg("date"), boost::python::arg("data")))
        .def("get_n_param",
            pure_virtual(&cAbstCondVar::GetNParam))
        .def("get_n_lags",
            pure_virtual(&cAbstCondVar::GetNLags))
        .def("compute_grad",
            pure_virtual(&cAbstCondVar::ComputeGrad),
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("residuals") = object()))
        .def("reg_arch_param_to_vector",
            pure_virtual(&cAbstCondVar::RegArchParamToVector),
            (boost::python::arg("dest_vect"), boost::python::arg("index") = 0))
        .def("vector_to_reg_arch_param",
            pure_virtual(&cAbstCondVar::VectorToRegArchParam),
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0))
        .def("set_default_init_point",
            pure_virtual(static_cast<void (cAbstCondVar::*)(double, double)>(&cAbstCondVar::SetDefaultInitPoint)),
            (boost::python::arg("mean"), boost::python::arg("var")))
        .def("set_default_init_point",
            pure_virtual(static_cast<void (cAbstCondVar::*)(cRegArchValue&)>(&cAbstCondVar::SetDefaultInitPoint)),
            boost::python::arg("value"))
        .def("compute_hess",
            pure_virtual(&cAbstCondVar::ComputeHess),
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("residuals") = object()))
        .def("compute_grad_and_hess",
            pure_virtual(&cAbstCondVar::ComputeGradAndHess),
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("residuals") = object()))
        ;
}
