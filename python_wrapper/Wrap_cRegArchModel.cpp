#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>
#include <sstream> // for std::ostringstream

using namespace boost::python;
using namespace RegArchLib;

/**
 * Helper function to convert the Print output to a string for Python's __str__ method
 */
std::string cRegArchModel_str(const cRegArchModel& model) {
    std::ostringstream oss;
    model.Print(oss);
    return oss.str();
}

/**
 * Helper function to create a more informative __repr__ output
 */
std::string cRegArchModel_repr(const cRegArchModel& model) {
    std::ostringstream oss;
    oss << "<RegArchModel: ";
    int nMean = const_cast<cRegArchModel&>(model).GetNMean();
    oss << nMean << " mean component(s)";

    cAbstCondVar* var = const_cast<cRegArchModel&>(model).GetVar();
    if (var != nullptr) {
        oss << ", variance model=" << var->GetCondVarType();
    }
    else {
        oss << ", no variance model";
    }

    cAbstResiduals* resids = const_cast<cRegArchModel&>(model).GetResid();
    if (resids != nullptr) {
        oss << ", residuals=" << resids->GetDistrType();
    }
    else {
        oss << ", no residuals";
    }

    oss << ">";
    return oss.str();
}

/**
 * Helper function to extract parameter names as a Python list
 */
boost::python::list cRegArchModel_get_param_names(cRegArchModel& model) {
    boost::python::list result;
    uint nParam = model.GetNParam();

    if (nParam > 0) {
        // get_param_name can store them in an array
        std::string* names = new std::string[nParam];
        model.GetParamName(names);

        for (uint i = 0; i < nParam; i++) {
            if (!names[i].empty()) {
                result.append(names[i]);
            }
            else {
                // If the name is empty or invalid, you could skip or append something like "<unnamed>"
            }
        }

        delete[] names;
    }

    return result;
}

/**
 * Helper function to convert the model parameters to a Python list
 */
boost::python::list cRegArchModel_params_to_list(cRegArchModel& model) {
    boost::python::list result;
    uint nParam = model.GetNParam();
    if (nParam > 0) {
        cDVector params(nParam);
        model.RegArchParamToVector(params);
        for (uint i = 0; i < nParam; i++) {
            result.append(params[i]);
        }
    }
    return result;
}

/**
 * Helper function to set model parameters from a Python list
 */
void cRegArchModel_params_from_list(cRegArchModel& model, const boost::python::list& params) {
    uint nParam = model.GetNParam();
    uint listLen = len(params);

    if (nParam > 0 && listLen > 0) {
        cDVector paramVec(nParam);

        for (uint i = 0; i < nParam && i < listLen; i++) {
            paramVec[i] = extract<double>(params[i]);
        }

        model.VectorToRegArchParam(paramVec);
    }
}

void export_cRegArchModel() {
    class_<cRegArchModel>("cRegArchModel",
        "Main model class for RegArch (Regression ARCH) models.\n\n"
        "This class combines three main components:\n"
        "1. Conditional mean model (can have multiple components)\n"
        "2. Conditional variance model (single component)\n"
        "3. Residuals distribution model (single component)\n\n"
        "A RegArch model is used to represent and analyze time series data with\n"
        "time-varying mean and variance structures. The model is specified by its\n"
        "components, and this class provides methods to manage these components,\n"
        "estimate parameters, and perform various analyses.",
        init<>()
    )
        // Constructors
        .def(init<cCondMean&, cAbstCondVar&, cAbstResiduals&>(
            (boost::python::arg("mean"), boost::python::arg("var"), boost::python::arg("residuals")),
            "Create a RegArch model with specified mean, variance, and residuals.\n\n"
            "Parameters\n"
            "----------\n"
            "mean : cCondMean\n"
            "    The conditional mean model (can contain multiple components)\n"
            "var : cAbstCondVar\n"
            "    The conditional variance model (e.g., GARCH, ARCH)\n"
            "residuals : cAbstResiduals\n"
            "    The residuals distribution model (e.g., Normal, Student-t)"
        ))
        .def(init<const cRegArchModel&>(
            boost::python::arg("model"),
            "Create a copy of an existing RegArch model."
        ))

        // Memory management
        .def("delete", &cRegArchModel::Delete,
            "Free all memory associated with this model.\n\n"
            "Call this when the model is no longer needed to prevent leaks.")

        // Assignment
        .def("__assign__", &cRegArchModel::operator=,
            return_value_policy<reference_existing_object>(),
            boost::python::arg("other"),
            "Assign from another RegArch model."
        )

        // Mean model management
        .def("get_n_mean", &cRegArchModel::GetNMean,
            "Number of conditional mean components.")
        .def("set_mean", &cRegArchModel::SetMean, boost::python::arg("cond_mean"),
            "Replace all mean components with the ones from the provided object.")
        .def("add_one_mean", &cRegArchModel::AddOneMean, boost::python::arg("one_mean"),
            "Add a single mean component to the model.")
        .def("get_one_mean", &cRegArchModel::GetOneMean,
            return_value_policy<reference_existing_object>(),
            boost::python::arg("num_mean"),
            "Get a specific mean component by index.")
        .def("delete_one_mean", &cRegArchModel::DeleteOneMean,
            (boost::python::arg("mean_type")),
            "Delete a mean component of the specified type.")
        .def("is_good_mean_type", &cRegArchModel::IsGoodMeanType,
            (boost::python::arg("mean_type"), boost::python::arg("index")),
            "Check if a mean type is valid for the given index")

        // Variance model
        .def("set_var", &cRegArchModel::SetVar, boost::python::arg("cond_var"),
            "Set the conditional variance model.")
        .def("get_var", &cRegArchModel::GetVar,
            return_value_policy<reference_existing_object>(),
            "Get the conditional variance model.")

        // Residuals
        .def("set_resid", &cRegArchModel::SetResid, boost::python::arg("cond_resids"),
            "Set the residuals distribution model.")
        .def("get_resid", &cRegArchModel::GetResid,
            return_value_policy<reference_existing_object>(),
            "Get the residuals distribution model.")

        // Print methods
        .def("print_mean", &cRegArchModel::PrintMean,
            "Print the mean model components.")
        .def("print_var", &cRegArchModel::PrintVar,
            "Print the variance model.")
        .def("print_residuals", &cRegArchModel::PrintResiduals,
            "Print the residuals model.")
        .def("print", &cRegArchModel::Print,
            "Print all model components.")

        // Pythonic string representation
        .def("__str__", &cRegArchModel_str)
        .def("__repr__", &cRegArchModel_repr)

        // Parameter init
        .def("set_default_init_point", &cRegArchModel::SetDefaultInitPoint,
            boost::python::arg("value"),
            "Set default initial parameter values based on the given data.")
        .def("get_n_param", &cRegArchModel::GetNParam,
            "Get the total number of parameters across all components.")
        .def("get_n_lags", &cRegArchModel::GetNLags,
            "Get the maximum number of lags required by any component.")

        // Parameter names / values
        .def("get_param_names", &cRegArchModel_get_param_names,
            "Return a list of all parameter names.")
        .def("to_param_vector", &cRegArchModel_params_to_list,
            "Convert all model parameters to a Python list of floats.")
        .def("from_param_vector", &cRegArchModel_params_from_list, boost::python::arg("params"),
            "Update model parameters from a Python list of floats.")

        .def("reg_arch_param_to_vector",
            (void (cRegArchModel::*)(cDVector&) const) & cRegArchModel::RegArchParamToVector,
            boost::python::arg("dest_vect"),
            "Write all parameter values into the provided cDVector.")
        .def("vector_to_reg_arch_param", &cRegArchModel::VectorToRegArchParam,
            boost::python::arg("src_param"),
            "Update all parameter values from the provided cDVector.")

        // Gradient / Hessian
        .def("compute_grad", &cRegArchModel::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("value"), boost::python::arg("grad_data")),
            "Compute gradient at the specified date.")
        .def("numeric_compute_grad", &cRegArchModel::NumericComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("residuals"), boost::python::arg("num_deriv")),
            "Compute gradient numerically.")
        .def("compute_grad_and_hess", &cRegArchModel::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("residuals")),
            "Compute gradient and Hessian at the specified date.")
        .def("numeric_compute_grad_and_hess", &cRegArchModel::NumericComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data"), boost::python::arg("residuals"), boost::python::arg("num_deriv")),
            "Compute gradient and Hessian numerically.")

        // Directly expose public members if you want (e.g., mMean, mVar, mResids):
        .def_readwrite("mMean", &cRegArchModel::mMean,
            "Conditional mean model component(s).")
        .def_readwrite("mVar", &cRegArchModel::mVar,
            "Conditional variance model component.")
        .def_readwrite("mResids", &cRegArchModel::mResids,
            "Residuals distribution model.")
        ;
}
