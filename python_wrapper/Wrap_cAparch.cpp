#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

// Helper function for printing
static void cAparchPrint(cAparch& self)
{
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

// Helper function to allow Python-style overloaded methods
static void cAparchSet(cAparch& self, const object& value_or_vector, const object& index_or_name, const object& num_param = object())
{
    // Check if first argument is a vector or a scalar
    extract<cDVector> vector_extract(value_or_vector);
    extract<double> value_extract(value_or_vector);

    // Check if second argument is a string or an index
    extract<std::string> name_extract(index_or_name);
    extract<uint> index_extract(index_or_name);

    if (vector_extract.check()) {
        cDVector vector = vector_extract();

        if (name_extract.check()) {
            // Set(cDVector, string)
            std::string name = name_extract();
            self.Set(vector, name);
        }
        else if (index_extract.check()) {
            // Set(cDVector, uint)
            uint num = extract<uint>(num_param);
            self.Set(vector, num);
        }
        else {
            throw std::runtime_error("Invalid arguments for set method");
        }
    }
    else if (value_extract.check()) {
        double value = value_extract();

        if (name_extract.check()) {
            // Set(double, string, uint)
            std::string name = name_extract();
            uint index = 0;
            if (!num_param.is_none())
                index = extract<uint>(num_param);
            self.Set(value, name, index);
        }
        else if (index_extract.check()) {
            // Set(double, uint, uint)
            uint index = index_extract();
            uint num = 0;
            if (!num_param.is_none())
                num = extract<uint>(num_param);
            self.Set(value, index, num);
        }
        else {
            throw std::runtime_error("Invalid arguments for set method");
        }
    }
    else {
        throw std::runtime_error("First argument must be a double or cDVector");
    }
}

// Helper function for get
static object cAparchGet(cAparch& self, const object& index_or_name, const object& num_param = object())
{
    // Check if first argument is a string or an index
    extract<std::string> name_extract(index_or_name);
    extract<uint> index_extract(index_or_name);

    if (name_extract.check()) {
        std::string name = name_extract();

        if (num_param.is_none()) {
            // Get(string) -> cDVector&
            return object(boost::ref(self.Get(name)));
        }
        else {
            // Get(string, uint) -> double
            uint index = extract<uint>(num_param);
            return object(self.Get(name, index));
        }
    }
    else if (index_extract.check()) {
        uint index = index_extract();

        if (num_param.is_none()) {
            // Get(uint) -> cDVector&
            return object(boost::ref(self.Get(index)));
        }
        else {
            // Get(uint, uint) -> double
            uint num = extract<uint>(num_param);
            return object(self.Get(index, num));
        }
    }
    else {
        throw std::runtime_error("First argument must be a string or uint");
    }
}

// Helper for realloc
static void cAparchReAlloc(cAparch& self, const object& size_or_vector, const object& num_param = object())
{
    // Check if first argument is a vector or a scalar
    extract<cDVector> vector_extract(size_or_vector);
    extract<uint> size_extract(size_or_vector);

    uint param = 0;
    if (!num_param.is_none())
        param = extract<uint>(num_param);

    if (vector_extract.check()) {
        // ReAlloc(cDVector, uint)
        cDVector vector = vector_extract();
        self.ReAlloc(vector, param);
    }
    else if (size_extract.check()) {
        // ReAlloc(uint, uint)
        uint size = size_extract();
        self.ReAlloc(size, param);
    }
    else {
        throw std::runtime_error("First argument must be a uint or cDVector");
    }
}

void export_cAparch()
{
    class_<cAparch, bases<cAbstCondVar> >("cAparch",
        "Concrete APARCH conditional variance model.",
        init<optional<int, int> >(
            (boost::python::arg("n_arch") = 0, boost::python::arg("n_garch") = 0),
            "cAparch(int n_arch=0, int n_garch=0)\n\n"
            "Creates an APARCH(p,q) model with p=n_arch and q=n_garch."
        )
    )
        // Copy constructor
        .def(init<const cAparch&>(boost::python::arg("other")))

        // Assignment operator
        .def("__assign__", &cAparch::operator=,
            return_value_policy<reference_existing_object>(),
            "Assigns another APARCH model to this one.")

        // Basic housekeeping
        .def("delete", &cAparch::Delete,
            "Releases memory used by the model.")

        // Printing
        .def("print", &cAparchPrint,
            "Prints model parameters to standard output.")

        // SetDefaultInitPoint methods - Pythonic naming
        .def("set_default_init_point",
            static_cast<void (cAparch::*)(double, double)>(&cAparch::SetDefaultInitPoint),
            (boost::python::arg("mean"), boost::python::arg("var")),
            "Sets default initial parameter values based on mean and variance.")
        .def("set_default_init_point",
            static_cast<void (cAparch::*)(cRegArchValue&)>(&cAparch::SetDefaultInitPoint),
            boost::python::arg("value"),
            "Sets default initial parameter values based on RegArch values.")

        // ReAlloc with unified Python interface
        .def("realloc", &cAparchReAlloc,
            (boost::python::arg("size_or_vector"), boost::python::arg("param_num") = object()),
            "Reallocates memory for model parameters.\n"
            "Can take either a size (uint) or a vector (cDVector).")

        // Overloaded 'Set' with unified Python interface
        .def("set", &cAparchSet,
            (boost::python::arg("value_or_vector"), boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Sets model parameters.\n"
            "Can be called as:\n"
            "  set(value, index, [param_num])\n"
            "  set(value, name, [index])\n"
            "  set(vector, param_num)\n"
            "  set(vector, name)")

        // Overloaded 'Get' with unified Python interface
        .def("get", &cAparchGet,
            (boost::python::arg("index_or_name"), boost::python::arg("num_param") = object()),
            "Gets model parameters.\n"
            "Can be called as:\n"
            "  get(index, param_num) -> double\n"
            "  get(param_num) -> cDVector\n"
            "  get(name, index) -> double\n"
            "  get(name) -> cDVector")

        // For backwards compatibility and access to specific methods
        // Original overloaded 'Set' methods
        .def("set_value_index",
            static_cast<void (cAparch::*)(double, uint, uint)>(&cAparch::Set),
            (boost::python::arg("value"), boost::python::arg("index"), boost::python::arg("param_num")),
            "Sets a parameter value by numerical index.")
        .def("set_vector_index",
            static_cast<void (cAparch::*)(const cDVector&, uint)>(&cAparch::Set),
            (boost::python::arg("values"), boost::python::arg("param_num")),
            "Sets parameters from a vector by numerical index.")
        .def("set_value_name",
            static_cast<void (cAparch::*)(double, std::string, uint)>(&cAparch::Set),
            (boost::python::arg("value"), boost::python::arg("param_name"), boost::python::arg("index") = 0),
            "Sets a parameter value by name.")
        .def("set_vector_name",
            static_cast<void (cAparch::*)(const cDVector&, std::string)>(&cAparch::Set),
            (boost::python::arg("values"), boost::python::arg("param_name")),
            "Sets parameters from a vector by name.")

        // Original overloaded 'Get' methods
        .def("get_value_index",
            static_cast<double (cAparch::*)(uint, uint)>(&cAparch::Get),
            (boost::python::arg("index"), boost::python::arg("param_num")),
            "Gets a parameter value by numerical index.")
        .def("get_vector_index",
            static_cast<cDVector & (cAparch::*)(uint)>(&cAparch::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_num"),
            "Gets parameters as a vector by numerical index.")
        .def("get_value_name",
            static_cast<double (cAparch::*)(std::string, uint)>(&cAparch::Get),
            (boost::python::arg("param_name"), boost::python::arg("index")),
            "Gets a parameter value by name.")
        .def("get_vector_name",
            static_cast<cDVector & (cAparch::*)(std::string)>(&cAparch::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"),
            "Gets parameters as a vector by name.")

        // Added SyncSizeParam method
        .def("sync_size_param", &cAparch::SyncSizeParam,
            boost::python::arg("num_param"),
            "Synchronizes parameter sizes.")

        // Added proxy parameter methods
        .def("realloc_proxy_var_parameters", &cAparch::ReAllocProxyVarParameters,
            (boost::python::arg("old_n_param") = 0),
            "Reallocates proxy variance parameters.")
        .def("update_proxy_var_parameters", &cAparch::UpdateProxyVarParameters,
            "Updates proxy variance parameters.")

        // Key APARCH methods
        .def("compute_var", &cAparch::ComputeVar,
            (boost::python::arg("date"), boost::python::arg("value")),
            "Computes conditional variance at the specified date given the data.")
        .def("get_n_param", &cAparch::GetNParam,
            "Returns the number of parameters in the model.")
        .def("get_n_lags", &cAparch::GetNLags,
            "Returns the number of lags required for gradient computation.")
        .def("reg_arch_param_to_vector", &cAparch::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index") = 0),
            "Converts model parameters to a vector representation.")
        .def("vector_to_reg_arch_param", &cAparch::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0),
            "Converts a vector representation back to model parameters.")
        .def("compute_grad", &cAparch::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("residuals") = object()),
            "Computes the gradient of the model at the specified date.")
        .def("compute_hess", &cAparch::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("residuals") = object()),
            "Computes the Hessian of the model at the specified date.")
        .def("compute_grad_and_hess", &cAparch::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"),
                boost::python::arg("grad_data"), boost::python::arg("hess_data"),
                boost::python::arg("residuals") = object()),
            "Computes both gradient and Hessian in one operation.")

        // Parameter name getters
        .def("get_param_name_char",
            static_cast<void (cAparch::*)(uint, char**)>(&cAparch::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C-style string.")
        .def("get_param_name_string",
            static_cast<void (cAparch::*)(uint, std::string[])>(&cAparch::GetParamName),
            boost::python::arg("index"),
            "Gets parameter name as a C++ string.")
        ;
}