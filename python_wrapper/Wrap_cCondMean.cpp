#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

// We NO LONGER include "PythonListConversion.h" 
// because we decided not to provide a .set(...) method for cCondMean.

using namespace boost::python;
using namespace RegArchLib;

// A helper function to print the object to std::cout
static void cCondMeanPrint(cCondMean& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// Helper: retrieve the i-th mean component from the internal array.
static cAbstCondMean* getOneMeanIndex(const cCondMean& self, uint index) {
    cAbstCondMean** arr = self.GetCondMean();
    if (arr && index < self.GetNMean()) {
        return arr[index];
    }
    return nullptr;
}

// Helper for GetCondMeanType that returns a Python list
static boost::python::list GetCondMeanTypeList(const cCondMean& self) {
    boost::python::list result;
    uint numMean = self.GetNMean();
    if (numMean > 0) {
        eCondMeanEnum* types = new eCondMeanEnum[numMean];
        self.GetCondMeanType(types);
        for (uint i = 0; i < numMean; i++) {
            result.append(types[i]);
        }
        delete[] types;
    }
    return result;
}

// Helper to get parameter names as a list
static boost::python::list GetParamNamesList(cCondMean& self) {
    boost::python::list result;
    uint nParam = self.GetNParam();
    if (nParam > 0) {
        std::string* names = new std::string[nParam];
        self.GetParamName(names);
        for (uint i = 0; i < nParam; i++) {
            if (!names[i].empty()) {
                result.append(names[i]);
            }
        }
        delete[] names;
    }
    return result;
}

void export_cCondMean()
{
    class_<cCondMean>("cCondMean",
        "Container for multiple conditional mean components.\n\n"
        "The conditional mean model in RegArch is a sum of 'elementary' conditional mean components\n"
        "This class provides methods to manage these components, compute the overall conditional mean,\n"
        "and handle parameters collectively.",
        init<>()
    )
        // Copy constructor
        .def(init<const cCondMean&>(
            boost::python::arg("other"),
            "Create a copy of an existing conditional mean model.\n\n"
            "Parameters:\n"
            "    other (cCondMean): The conditional mean model to copy."
        ))

        // Pythonic naming
        .def("add_one_mean", &cCondMean::AddOneMean,
            boost::python::arg("abstract_cond_mean"),
            "Add a new conditional mean component.\n\n"
            "Parameters:\n"
            "    abstract_cond_mean (cAbstCondMean): The conditional mean component to add.\n\n"
            "Note: The component will be added at the first available position in the internal array."
        )
        .def("delete_one_mean_enum",
            static_cast<void (cCondMean::*)(eCondMeanEnum)>(&cCondMean::DeleteOneMean),
            boost::python::arg("cond_mean_enum"),
            "Delete a conditional mean component by type.\n\n"
            "Parameters:\n"
            "    cond_mean_enum (eCondMeanEnum): The type of the component to delete."
        )
        .def("delete_one_mean_name",
            static_cast<void (cCondMean::*)(std::string)>(&cCondMean::DeleteOneMean),
            boost::python::arg("param_name"),
            "Delete a conditional mean component by name.\n\n"
            "Parameters:\n"
            "    param_name (str): The name of the component to delete."
        )
        .def("get_n_mean", &cCondMean::GetNMean,
            "Get the number of conditional mean components.\n\n"
            "Returns:\n"
            "    int: The number of components."
        )
        .def("get_one_mean_index", &getOneMeanIndex,
            return_value_policy<reference_existing_object>(),
            boost::python::arg("index"),
            "Get a conditional mean component by index.\n\n"
            "Parameters:\n"
            "    index (int): The index of the component.\n\n"
            "Returns:\n"
            "    cAbstCondMean or None: The component at the specified index, or None if not found."
        )
        .def("get_one_mean_name",
            static_cast<cAbstCondMean * (cCondMean::*)(std::string) const>(&cCondMean::GetOneMean),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Get a conditional mean component by name.\n\n"
            "Parameters:\n"
            "    param_name (str): The name of the component.\n\n"
            "Returns:\n"
            "    cAbstCondMean or None: The component with the specified name, or None if not found."
        )
        .def("get_cond_mean_type", &GetCondMeanTypeList,
            "Get a list of types for all conditional mean components.\n\n"
            "Returns:\n"
            "    list: A list of eCondMeanEnum values representing the types of all components."
        )
        .def("set_default_init_point",
            static_cast<void (cCondMean::*)(double, double)>(&cCondMean::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Set default initial parameter values for all components based on mean and variance.\n\n"
            "Parameters:\n"
            "    mean (double): The mean value.\n"
            "    var (double): The variance value."
        )
        .def("set_default_init_point_value",
            static_cast<void (cCondMean::*)(cRegArchValue&)>(&cCondMean::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Set default initial parameter values for all components based on RegArch values.\n\n"
            "Parameters:\n"
            "    value (cRegArchValue): The RegArch values object."
        )
        .def("update_proxy_mean_parameters", &cCondMean::UpdateProxyMeanParameters,
            "Update proxy mean parameters for all components."
        )
        .def("compute_mean", &cCondMean::ComputeMean,
            (boost::python::arg("date"), boost::python::arg("data")),
            "Compute the overall conditional mean at the specified date.\n\n"
            "Parameters:\n"
            "    date (int): The date index.\n"
            "    data (cRegArchValue): The data object.\n\n"
            "Returns:\n"
            "    double: The computed conditional mean value."
        )
        .def("get_n_param", &cCondMean::GetNParam,
            "Get the total number of parameters across all components.\n\n"
            "Returns:\n"
            "    int: The total number of parameters."
        )
        .def("get", &cCondMean::Get,
            (boost::python::arg("num_mean") = 0, boost::python::arg("index") = 0, boost::python::arg("num_param") = 0),
            "Get a parameter value.\n\n"
            "Parameters:\n"
            "    num_mean (int, optional): The index of the mean component. Default is 0.\n"
            "    index (int, optional): The parameter index. Default is 0.\n"
            "    num_param (int, optional): The parameter number. Default is 0.\n\n"
            "Returns:\n"
            "    double: The parameter value."
        )
        .def("get_n_lags", &cCondMean::GetNLags,
            "Get the number of lags required for computation.\n\n"
            "Returns:\n"
            "    int: The number of lags."
        )
        .def("compute_grad", &cCondMean::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data")),
            "Compute the gradient of the model at the specified date.\n\n"
            "Parameters:\n"
            "    date (int): The date index.\n"
            "    data (cRegArchValue): The data object.\n"
            "    grad_data (cRegArchGradient): The gradient object to store results."
        )
        .def("reg_arch_param_to_vector", &cCondMean::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index")),
            "Convert all components' parameters to a vector representation.\n\n"
            "Parameters:\n"
            "    dest_vect (cDVector): The destination vector.\n"
            "    index (int): The starting index."
        )
        .def("vector_to_reg_arch_param", &cCondMean::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Convert a vector representation back to all components' parameters.\n\n"
            "Parameters:\n"
            "    src_vect (cDVector): The source vector.\n"
            "    index (int, optional): The starting index. Default is 0."
        )
        .def("compute_hess", &cCondMean::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data")),
            "Compute the Hessian of the model at the specified date.\n\n"
            "Parameters:\n"
            "    date (int): The date index.\n"
            "    data (cRegArchValue): The data object.\n"
            "    grad_data (cRegArchGradient): The gradient object.\n"
            "    hess_data (cRegArchHessien): The Hessian object to store results."
        )
        .def("compute_grad_and_hess", &cCondMean::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("hess_data")),
            "Compute both gradient and Hessian in one operation.\n\n"
            "Parameters:\n"
            "    date (int): The date index.\n"
            "    data (cRegArchValue): The data object.\n"
            "    grad_data (cRegArchGradient): The gradient object to store results.\n"
            "    hess_data (cRegArchHessien): The Hessian object to store results."
        )
        .def("get_param_name_char",
            static_cast<void (cCondMean::*)(char**)>(&cCondMean::GetParamName),
            boost::python::arg("name"),
            "Get parameter names as C-style strings.\n\n"
            "Parameters:\n"
            "    name (char**): Array to store parameter names.\n\n"
            "Note: This method is primarily for internal use. Use get_param_name_string instead."
        )
        .def("get_param_name_string",
            static_cast<void (cCondMean::*)(std::string[])>(&cCondMean::GetParamName),
            boost::python::arg("name"),
            "Get parameter names as C++ strings.\n\n"
            "Parameters:\n"
            "    name (string[]): Array to store parameter names."
        )
        .def("get_param_names", &GetParamNamesList,
            "Get all parameter names as a Python list.\n\n"
            "Returns:\n"
            "    list: List of parameter names."
        )
        .def("print", &cCondMeanPrint,
            "Print information about this conditional mean model to standard output."
        )
        .def("delete", &cCondMean::Delete,
            "Free memory used by this model."
        )

        // Original C++ naming for compatibility
        .def("AddOneMean", &cCondMean::AddOneMean)
        .def("DeleteOneMeanEnum", static_cast<void (cCondMean::*)(eCondMeanEnum)>(&cCondMean::DeleteOneMean))
        .def("DeleteOneMeanName", static_cast<void (cCondMean::*)(std::string)>(&cCondMean::DeleteOneMean))
        .def("GetNMean", &cCondMean::GetNMean)
        .def("GetOneMeanIndex", &getOneMeanIndex, return_value_policy<reference_existing_object>())
        .def("GetOneMeanName", static_cast<cAbstCondMean * (cCondMean::*)(std::string) const>(&cCondMean::GetOneMean),
            return_value_policy<reference_existing_object>())
        .def("GetCondMeanType", &GetCondMeanTypeList)
        .def("SetDefaultInitPoint", static_cast<void (cCondMean::*)(double, double)>(&cCondMean::SetDefaultInitPoint))
        .def("SetDefaultInitPointValue", static_cast<void (cCondMean::*)(cRegArchValue&)>(&cCondMean::SetDefaultInitPoint))
        .def("UpdateProxyMeanParameters", &cCondMean::UpdateProxyMeanParameters)
        .def("ComputeMean", &cCondMean::ComputeMean)
        .def("GetNParam", &cCondMean::GetNParam)
        .def("Get", &cCondMean::Get)
        .def("GetNLags", &cCondMean::GetNLags)
        .def("ComputeGrad", &cCondMean::ComputeGrad)
        .def("RegArchParamToVector", &cCondMean::RegArchParamToVector)
        .def("VectorToRegArchParam", &cCondMean::VectorToRegArchParam)
        .def("ComputeHess", &cCondMean::ComputeHess)
        .def("ComputeGradAndHess", &cCondMean::ComputeGradAndHess)
        .def("GetParamNameChar", static_cast<void (cCondMean::*)(char**)>(&cCondMean::GetParamName))
        .def("GetParamNameString", static_cast<void (cCondMean::*)(std::string[])>(&cCondMean::GetParamName))
        .def("Print", &cCondMeanPrint)
        .def("Delete", &cCondMean::Delete)

        // Assignment operator
        .def("__assign__", &cCondMean::operator=, return_internal_reference<>(),
            "Assignment operator - copies another conditional mean model.\n\n"
            "Parameters:\n"
            "    other (cCondMean): The source conditional mean model.\n\n"
            "Returns:\n"
            "    cCondMean: Reference to this model after assignment."
        )
        ;
}
