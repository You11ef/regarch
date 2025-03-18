#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>


using namespace boost::python;
using namespace RegArchLib;

// A helper function to print the object.
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

void export_cCondMean()
{
    class_<cCondMean>("cCondMean",
        "Container for multiple conditional mean components.",
        init<>())
        // Copy constructor
        .def(init<const cCondMean&>())

        // Pythonic naming
        .def("add_one_mean", &cCondMean::AddOneMean,
            boost::python::arg("abstract_cond_mean"),
            "Adds a new conditional mean component.")
        .def("delete_one_mean_enum",
            static_cast<void (cCondMean::*)(eCondMeanEnum)>(&cCondMean::DeleteOneMean),
            boost::python::arg("cond_mean_enum"),
            "Deletes a conditional mean component by type.")
        .def("delete_one_mean_name",
            static_cast<void (cCondMean::*)(std::string)>(&cCondMean::DeleteOneMean),
            boost::python::arg("param_name"),
            "Deletes a conditional mean component by name.")
        .def("get_n_mean", &cCondMean::GetNMean,
            "Returns the number of conditional mean components.")
        .def("get_one_mean_index", &getOneMeanIndex,
            return_value_policy<reference_existing_object>(),
            boost::python::arg("index"),
            "Gets a conditional mean component by index.")
        .def("get_one_mean_name",
            static_cast<cAbstCondMean * (cCondMean::*)(std::string) const>(&cCondMean::GetOneMean),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Gets a conditional mean component by name.")
        .def("get_cond_mean_type", &GetCondMeanTypeList,
            "Returns a list of types for all conditional mean components.")
        .def("set_default_init_point",
            static_cast<void (cCondMean::*)(double, double)>(&cCondMean::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Sets default initial parameter values based on mean and variance.")
        .def("set_default_init_point_value",
            static_cast<void (cCondMean::*)(cRegArchValue&)>(&cCondMean::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Sets default initial parameter values based on RegArch values.")
        .def("update_proxy_mean_parameters", &cCondMean::UpdateProxyMeanParameters,
            "Updates proxy mean parameters.")
        .def("compute_mean", &cCondMean::ComputeMean,
            (boost::python::arg("date"), boost::python::arg("data")),
            "Computes the conditional mean at the specified date.")
        .def("get_n_param", &cCondMean::GetNParam,
            "Returns the total number of parameters.")
        .def("get", &cCondMean::Get,
            (boost::python::arg("num_mean") = 0, boost::python::arg("index") = 0,
                boost::python::arg("num_param") = 0),
            "Gets a parameter value.")
        .def("get_n_lags", &cCondMean::GetNLags,
            "Returns the number of lags required for computation.")
        .def("compute_grad", &cCondMean::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data")),
            "Computes the gradient at the specified date.")
        .def("reg_arch_param_to_vector", &cCondMean::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index")),
            "Converts model parameters to a vector representation.")
        .def("vector_to_reg_arch_param", &cCondMean::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Converts a vector representation back to model parameters.")
        .def("compute_hess", &cCondMean::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data")),
            "Computes the Hessian at the specified date.")
        .def("compute_grad_and_hess", &cCondMean::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data")),
            "Computes both gradient and Hessian at the specified date.")
        .def("get_param_name_char",
            static_cast<void (cCondMean::*)(char**)>(&cCondMean::GetParamName),
            boost::python::arg("name"),
            "Gets parameter names as C-style strings.")
        .def("get_param_name_string",
            static_cast<void (cCondMean::*)(std::string[])>(&cCondMean::GetParamName),
            boost::python::arg("name"),
            "Gets parameter names as C++ strings.")
        .def("print", &cCondMeanPrint,
            "Prints information about this conditional mean model.")
        .def("delete", &cCondMean::Delete,
            "Frees memory used by this model.")

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
            "Assignment operator - copies another conditional mean model.")
        ;
}