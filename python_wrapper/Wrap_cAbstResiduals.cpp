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
        if (override f = this->get_override("print"))
            f(); // ignoring theOut for the Python bridge
        else
            throw std::runtime_error("Python class must implement 'print' method");
    }

    void default_Print(ostream& theOut) const {
        throw std::runtime_error("Python class must implement 'print' method");
    }

    // Convenience method for Python users.
    void PrintPython() {
        Print(std::cout);
    }

    //--- Overridden pure virtual methods ---
    virtual void Generate(const uint theNSample, cDVector& theYt) const override {
        if (override f = this->get_override("generate"))
            f(theNSample, boost::ref(theYt));
        else
            throw std::runtime_error("Python class must implement 'generate' method");
    }

    virtual double LogDensity(double theX) const override {
        if (override f = this->get_override("log_density"))
            return f(theX);
        throw std::runtime_error("Python class must implement 'log_density' method");
    }

    virtual uint GetNParam(void) const override {
        if (override f = this->get_override("get_n_param"))
            return f();
        throw std::runtime_error("Python class must implement 'get_n_param' method");
    }

    virtual double DiffLogDensity(double theX) const override {
        if (override f = this->get_override("diff_log_density"))
            return f(theX);
        throw std::runtime_error("Python class must implement 'diff_log_density' method");
    }

    virtual void ComputeGrad(uint theDate, const cRegArchValue& theData, cRegArchGradient& theGradData) const override {
        if (override f = this->get_override("compute_grad"))
            f(theDate, boost::cref(theData), boost::ref(theGradData));
        else
            throw std::runtime_error("Python class must implement 'compute_grad' method");
    }

    virtual void RegArchParamToVector(cDVector& theDestVect, uint theIndex) const override {
        if (override f = this->get_override("reg_arch_param_to_vector"))
            f(boost::ref(theDestVect), theIndex);
        else
            throw std::runtime_error("Python class must implement 'reg_arch_param_to_vector' method");
    }

    virtual void VectorToRegArchParam(const cDVector& theSrcVect, uint theIndex = 0) override {
        if (override f = this->get_override("vector_to_reg_arch_param"))
            f(boost::cref(theSrcVect), theIndex);
        else
            throw std::runtime_error("Python class must implement 'vector_to_reg_arch_param' method");
    }

    virtual double ComputeEspAbsEps(void) override {
        if (override f = this->get_override("compute_esp_abs_eps"))
            return f();
        throw std::runtime_error("Python class must implement 'compute_esp_abs_eps' method");
    }

    virtual void ComputeGradBetaEspAbsEps(cDVector& theGrad) override {
        if (override f = this->get_override("compute_grad_beta_esp_abs_eps"))
            f(boost::ref(theGrad));
        else
            throw std::runtime_error("Python class must implement 'compute_grad_beta_esp_abs_eps' method");
    }

    virtual void ComputeHessBetaEspAbsEps(cDMatrix& theHess) override {
        if (override f = this->get_override("compute_hess_beta_esp_abs_eps"))
            f(boost::ref(theHess));
        else
            throw std::runtime_error("Python class must implement 'compute_hess_beta_esp_abs_eps' method");
    }

    virtual void SetDefaultInitPoint(void) override {
        if (override f = this->get_override("set_default_init_point"))
            f();
        else
            throw std::runtime_error("Python class must implement 'set_default_init_point' method");
    }

    virtual double Diff2LogDensity(double theX) const override {
        if (override f = this->get_override("diff2_log_density"))
            return f(theX);
        throw std::runtime_error("Python class must implement 'diff2_log_density' method");
    }

    virtual void GradDiffLogDensity(double theX, const cDVector& theDistrParam, cDVector& theGrad) override {
        if (override f = this->get_override("grad_diff_log_density"))
            f(theX, boost::cref(theDistrParam), boost::ref(theGrad));
        else
            throw std::runtime_error("Python class must implement 'grad_diff_log_density' method");
    }

    virtual void ComputeHess(uint theDate, const cRegArchValue& theData, const cRegArchGradient& theGradData, cRegArchHessien& theHessData) override {
        if (override f = this->get_override("compute_hess"))
            f(theDate, boost::cref(theData), boost::cref(theGradData), boost::ref(theHessData));
        else
            throw std::runtime_error("Python class must implement 'compute_hess' method");
    }

    virtual void ComputeGradAndHess(uint theDate, const cRegArchValue& theData, cRegArchGradient& theGradData, cRegArchHessien& theHessData) override {
        if (override f = this->get_override("compute_grad_and_hess"))
            f(theDate, boost::cref(theData), boost::ref(theGradData), boost::ref(theHessData));
        else
            throw std::runtime_error("Python class must implement 'compute_grad_and_hess' method");
    }

    virtual void GetParamName(uint theIndex, char** theName) override {
        if (override f = this->get_override("get_param_name_char"))
            f(theIndex, theName);
        else {
            // Default implementation: Try to use the string version and convert
            std::string names[10];  // Max 10 names
            GetParamName(theIndex, names);
            for (int i = 0; i < 10; i++) {
                if (theName[i] && !names[i].empty()) {
                    strcpy(theName[i], names[i].c_str());
                }
            }
        }
    }

    virtual void GetParamName(uint theIndex, std::string theName[]) override {
        if (override f = this->get_override("get_param_name_string"))
            f(theIndex, theName);
        else
            throw std::runtime_error("Python class must implement 'get_param_name_string' method");
    }

    virtual void NumericComputeGrad(uint theDate, const cRegArchValue& theData,
        cRegArchGradient& theGradData, uint theBegIndex,
        cNumericDerivative& theNumDeriv) override {
        if (override f = this->get_override("numeric_compute_grad"))
            f(theDate, boost::cref(theData), boost::ref(theGradData), theBegIndex, boost::ref(theNumDeriv));
        else {
            cAbstResiduals::NumericComputeGrad(theDate, theData, theGradData, theBegIndex, theNumDeriv);
        }
    }
};

// Helper function to get parameter names as a Python list
boost::python::list get_param_names_py(cAbstResiduals& self, uint theIndex)
{
    // Let's assume the max number of parameter names is 10 (adjust as needed) 
    std::string names[10] = { "" }; // Initialize with empty strings
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

void export_cAbstResiduals()
{

    class_<cAbstResidualsWrap, boost::noncopyable>("cAbstResiduals",
        "Abstract base class for residual distributions in RegArch.\n\n"
        "This class defines the interface for all conditional distribution components.\n"
        "Concrete implementations include normal (cNormResiduals), Student-t (cStudentResiduals),\n"
        "GED (cGedResiduals), and mixed normal (cMixNormResiduals) distributions.\n\n"
        "The conditional distribution component describes the shape of the distribution\n"
        "of residuals after accounting for conditional mean and variance.",
        init<eDistrTypeEnum, cDVector*, bool>(
            (boost::python::arg("distr_type"),
                boost::python::arg("distr_param") = static_cast<cDVector*>(0),
                boost::python::arg("simul_flag") = false),
            "Initialize a residuals distribution model with the specified type and parameters.\n\n"
            "Parameters\n"
            "----------\n"
            "distr_type : DistrTypeEnum\n"
            "    The type of distribution to create\n"
            "distr_param : cDVector, optional\n"
            "    Vector of distribution parameters (default None)\n"
            "simul_flag : bool, optional\n"
            "    Whether this distribution will be used for simulation (default False)"
        )
    )
        // Non-virtual methods
        .def("delete", &cAbstResiduals::Delete,
            "Deletes the distribution and frees associated memory.\n\n"
            "This method releases all resources used by the distribution instance.")
        .def("get_distr_type", &cAbstResiduals::GetDistrType,
            "Returns the enumeration value representing the distribution type.\n\n"
            "Returns\n"
            "-------\n"
            "DistrTypeEnum\n"
            "    The type of this distribution.")
        .def("set_simul", &cAbstResiduals::SetSimul,
            boost::python::arg("simul_flag") = true,
            "Sets whether this distribution is used for simulation.\n\n"
            "Parameters\n"
            "----------\n"
            "simul_flag : bool, optional\n"
            "    True to enable simulation mode, False otherwise (default True)")
        .def("is_for_simul", &cAbstResiduals::IsForSimul,
            "Checks if this distribution is set up for simulation.\n\n"
            "Returns\n"
            "-------\n"
            "bool\n"
            "    True if simulation mode is enabled, False otherwise")
        .def("get", &cAbstResiduals::Get,
            boost::python::arg("index") = 0,
            "Gets a distribution parameter value by index.\n\n"
            "Parameters\n"
            "----------\n"
            "index : int, optional\n"
            "    The parameter index (default 0)\n\n"
            "Returns\n"
            "-------\n"
            "float\n"
            "    The parameter value")
        .def("realloc", &cAbstResiduals::ReAlloc,
            boost::python::arg("size"),
            "Reallocates memory for the distribution parameters.\n\n"
            "Parameters\n"
            "----------\n"
            "size : int\n"
            "    The new size to allocate")
        .def("set", &cAbstResiduals::Set,
            (boost::python::arg("value"), boost::python::arg("index") = 0),
            "Sets a distribution parameter value.\n\n"
            "Parameters\n"
            "----------\n"
            "value : float\n"
            "    The new parameter value\n"
            "index : int, optional\n"
            "    The parameter index (default 0)")

        // Helper method for getting parameter names
        .def("get_param_names", &get_param_names_py,
            boost::python::arg("index"),
            "Returns a list of parameter names for the given index.\n\n"
            "Parameters\n"
            "----------\n"
            "index : int\n"
            "    The parameter group index\n\n"
            "Returns\n"
            "-------\n"
            "list\n"
            "    List of parameter names for the specified index")

        // Print method
        .def("print", &cAbstResidualsWrap::PrintPython,
            "Prints the distribution parameters to standard output.\n\n"
            "This method displays all parameters of the distribution in a readable format.")

        // Virtual methods with Pythonic names
        .def("generate", pure_virtual(&cAbstResiduals::Generate),
            (boost::python::arg("n_sample"), boost::python::arg("yt")),
            "Generates random samples from the distribution.\n\n"
            "Parameters\n"
            "----------\n"
            "n_sample : int\n"
            "    Number of samples to generate\n"
            "yt : cDVector\n"
            "    Vector where generated samples will be stored")
        .def("log_density", pure_virtual(&cAbstResiduals::LogDensity),
            boost::python::arg("x"),
            "Computes the log density function at a given point.\n\n"
            "Parameters\n"
            "----------\n"
            "x : float\n"
            "    Point at which to evaluate the log density\n\n"
            "Returns\n"
            "-------\n"
            "float\n"
            "    Value of the log density function at x")
        .def("get_n_param", pure_virtual(&cAbstResiduals::GetNParam),
            "Returns the number of distribution parameters.\n\n"
            "Returns\n"
            "-------\n"
            "int\n"
            "    Number of parameters in this distribution")
        .def("diff_log_density", pure_virtual(&cAbstResiduals::DiffLogDensity),
            boost::python::arg("x"),
            "Computes the first derivative of the log density function.\n\n"
            "Parameters\n"
            "----------\n"
            "x : float\n"
            "    Point at which to evaluate the derivative\n\n"
            "Returns\n"
            "-------\n"
            "float\n"
            "    Value of the derivative of log density at x")
        .def("compute_grad", pure_virtual(&cAbstResiduals::ComputeGrad),
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data")),
            "Computes the gradient of the distribution parameters.\n\n"
            "Parameters\n"
            "----------\n"
            "date : int\n"
            "    The time index for which to compute the gradient\n"
            "data : cRegArchValue\n"
            "    Container with the time series data and other values\n"
            "grad_data : cRegArchGradient\n"
            "    Gradient container where results will be stored")
        .def("reg_arch_param_to_vector", pure_virtual(&cAbstResiduals::RegArchParamToVector),
            (boost::python::arg("dest_vect"), boost::python::arg("index")),
            "Converts distribution parameters to a vector representation.\n\n"
            "Parameters\n"
            "----------\n"
            "dest_vect : cDVector\n"
            "    Destination vector where parameters will be stored\n"
            "index : int\n"
            "    Starting index in the destination vector")
        .def("vector_to_reg_arch_param", pure_virtual(&cAbstResiduals::VectorToRegArchParam),
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Converts a vector representation back to distribution parameters.\n\n"
            "Parameters\n"
            "----------\n"
            "src_vect : cDVector\n"
            "    Source vector containing the parameter values\n"
            "index : int, optional\n"
            "    Starting index in the source vector (default 0)")
        .def("compute_esp_abs_eps", pure_virtual(&cAbstResiduals::ComputeEspAbsEps),
            "Computes the expected absolute value of the distribution.\n\n"
            "Returns\n"
            "-------\n"
            "float\n"
            "    Expected absolute value")
        .def("compute_grad_beta_esp_abs_eps", pure_virtual(&cAbstResiduals::ComputeGradBetaEspAbsEps),
            boost::python::arg("grad"),
            "Computes the gradient of the expected absolute value with respect to the distribution parameters.\n\n"
            "Parameters\n"
            "----------\n"
            "grad : cDVector\n"
            "    Vector where gradient results will be stored")
        .def("compute_hess_beta_esp_abs_eps", pure_virtual(&cAbstResiduals::ComputeHessBetaEspAbsEps),
            boost::python::arg("hess"),
            "Computes the Hessian of the expected absolute value with respect to the distribution parameters.\n\n"
            "Parameters\n"
            "----------\n"
            "hess : cDMatrix\n"
            "    Matrix where Hessian results will be stored")
        .def("set_default_init_point", pure_virtual(&cAbstResiduals::SetDefaultInitPoint),
            "Sets default initial values for the distribution parameters.\n\n"
            "This method initializes the parameters with suitable default values.")
        .def("diff2_log_density", pure_virtual(&cAbstResiduals::Diff2LogDensity),
            boost::python::arg("x"),
            "Computes the second derivative of the log density function.\n\n"
            "Parameters\n"
            "----------\n"
            "x : float\n"
            "    Point at which to evaluate the second derivative\n\n"
            "Returns\n"
            "-------\n"
            "float\n"
            "    Value of the second derivative of log density at x")
        .def("grad_diff_log_density", pure_virtual(&cAbstResiduals::GradDiffLogDensity),
            (boost::python::arg("x"), boost::python::arg("distr_param"), boost::python::arg("grad")),
            "Computes the gradient of the first derivative of the log density.\n\n"
            "Parameters\n"
            "----------\n"
            "x : float\n"
            "    Point at which to evaluate the gradient\n"
            "distr_param : cDVector\n"
            "    Vector of distribution parameters\n"
            "grad : cDVector\n"
            "    Vector where gradient results will be stored")
        .def("compute_hess", pure_virtual(&cAbstResiduals::ComputeHess),
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data")),
            "Computes the Hessian of the distribution parameters.\n\n"
            "Parameters\n"
            "----------\n"
            "date : int\n"
            "    The time index for which to compute the Hessian\n"
            "data : cRegArchValue\n"
            "    Container with the time series data and other values\n"
            "grad_data : cRegArchGradient\n"
            "    Gradient container with pre-computed gradient values\n"
            "hess_data : cRegArchHessien\n"
            "    Hessian container where results will be stored")
        .def("compute_grad_and_hess", pure_virtual(&cAbstResiduals::ComputeGradAndHess),
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data")),
            "Computes both gradient and Hessian in one operation.\n\n"
            "This method may be more efficient than computing them separately.\n\n"
            "Parameters\n"
            "----------\n"
            "date : int\n"
            "    The time index for which to compute the derivatives\n"
            "data : cRegArchValue\n"
            "    Container with the time series data and other values\n"
            "grad_data : cRegArchGradient\n"
            "    Gradient container where gradient results will be stored\n"
            "hess_data : cRegArchHessien\n"
            "    Hessian container where Hessian results will be stored")
        .def("numeric_compute_grad", &cAbstResiduals::NumericComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("beg_index"),
                boost::python::arg("num_deriv")),
            "Computes the gradient using numerical differentiation.\n\n"
            "This method is useful when analytical derivatives are not available.\n\n"
            "Parameters\n"
            "----------\n"
            "date : int\n"
            "    The time index for which to compute the gradient\n"
            "data : cRegArchValue\n"
            "    Container with the time series data and other values\n"
            "grad_data : cRegArchGradient\n"
            "    Gradient container where results will be stored\n"
            "beg_index : int\n"
            "    Starting index in the gradient vector\n"
            "num_deriv : cNumericDerivative\n"
            "    Numerical derivative helper object")
        ;

    // Expose factory functions with disambiguation and an explicit return value policy.
    def("create_real_cond_residuals",
        static_cast<cAbstResiduals * (*)(const eDistrTypeEnum, cDVector*, bool)>(&CreateRealCondResiduals),
        (boost::python::arg("distr_type"),
            boost::python::arg("distr_param") = static_cast<cDVector*>(0),
            boost::python::arg("simul_flag") = false),
        return_value_policy<manage_new_object>(),
        "Creates a concrete residuals distribution instance based on the specified type.\n\n"
        "This factory function creates the appropriate distribution object based on the type.\n\n"
        "Parameters\n"
        "----------\n"
        "distr_type : DistrTypeEnum\n"
        "    The type of distribution to create\n"
        "distr_param : cDVector, optional\n"
        "    Vector of distribution parameters (default None)\n"
        "simul_flag : bool, optional\n"
        "    Whether this distribution will be used for simulation (default False)\n\n"
        "Returns\n"
        "-------\n"
        "cAbstResiduals\n"
        "    A new instance of the specified distribution type");

    def("create_real_cond_residuals_copy",
        static_cast<cAbstResiduals * (*)(cAbstResiduals&)>(&CreateRealCondResiduals),
        boost::python::arg("residuals"),
        return_value_policy<manage_new_object>(),
        "Creates a copy of an existing residuals distribution instance.\n\n"
        "Parameters\n"
        "----------\n"
        "residuals : cAbstResiduals\n"
        "    The distribution to copy\n\n"
        "Returns\n"
        "-------\n"
        "cAbstResiduals\n"
        "    A new instance that is a copy of the input distribution");
}