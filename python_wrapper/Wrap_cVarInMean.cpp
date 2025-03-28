#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include "PythonConversion.h"

using namespace boost::python;
using namespace RegArchLib;

namespace {  // Anonymous namespace for converters

    // =====================================================================
    //           Python <-> C++ Type Conversions
    // =====================================================================

    // 1. Python to C++ Conversions
    // ----------------------------

    struct cDVector_from_python {
        cDVector_from_python() {
            converter::registry::push_back(
                &convertible,
                &construct,
                type_id<cDVector>()
            );
        }

        static void* convertible(PyObject* obj) {
            return PySequence_Check(obj) ? obj : 0;
        }

        static void construct(PyObject* obj,
            converter::rvalue_from_python_stage1_data* data) {
            object py_obj(handle<>(borrowed(obj)));
            cDVector vec = py_list_or_tuple_to_cDVector(py_obj);
            void* storage = ((converter::rvalue_from_python_storage<cDVector>*)data)->storage.bytes;
            new (storage) cDVector(vec);
            data->convertible = storage;
        }
    };

    struct cDMatrix_from_python {
        cDMatrix_from_python() {
            converter::registry::push_back(
                &convertible,
                &construct,
                type_id<cDMatrix>()
            );
        }

        static void* convertible(PyObject* obj) {
            return PySequence_Check(obj) ? obj : 0;
        }

        static void construct(PyObject* obj,
            converter::rvalue_from_python_stage1_data* data) {
            object py_obj(handle<>(borrowed(obj)));
            cDMatrix mat = py_list_of_lists_to_cDMatrix(py_obj);
            void* storage = ((converter::rvalue_from_python_storage<cDMatrix>*)data)->storage.bytes;
            new (storage) cDMatrix(mat);
            data->convertible = storage;
        }
    };

    // 2. C++ to Python Conversions
    // ----------------------------

    struct cDVector_to_python {
        static PyObject* convert(const cDVector& vec) {
            boost::python::list py_list;
            for (uint i = 0; i < vec.GetSize(); ++i) {
                py_list.append(vec[i]);
            }
            return boost::python::incref(py_list.ptr());
        }
    };

    struct cDMatrix_to_python {
        static PyObject* convert(const cDMatrix& mat) {
            boost::python::list py_matrix;
            for (uint i = 0; i < mat.GetNRow(); ++i) {
                boost::python::list py_row;
                for (uint j = 0; j < mat.GetNCol(); ++j) {
                    py_row.append(mat[i][j]);
                }
                py_matrix.append(py_row);
            }
            return boost::python::incref(py_matrix.ptr());
        }
    };

    // 3. Register Converters
    // ----------------------
    static cDVector_from_python register_cDVector_converter;
    static cDMatrix_from_python register_cDMatrix_converter;

}  // End anonymous namespace

// =====================================================================
//                   cVarInMean Wrapper
// =====================================================================

static void VarInMean_Print(cVarInMean& self) {
#ifndef _RDLL_
    self.Print(std::cout);
#endif
}

void export_cVarInMean() {
    // Register C++ to Python converters
    to_python_converter<cDVector, cDVector_to_python>();
    to_python_converter<cDMatrix, cDMatrix_to_python>();

    class_<cVarInMean, bases<cAbstCondMean> >("cVarInMean",
        "Variance-in-Mean model component",
        init<optional<double> >(
            (boost::python::arg("theVarInMean") = 0.0),
            "cVarInMean(double theVarInMean=0.0)"
        )
    )
        // Core methods
        .def("Delete", &cVarInMean::Delete)
        .def("Print", &VarInMean_Print)

        // Initialization
        .def("SetDefaultInitPoint",
            static_cast<void (cVarInMean::*)(double, double)>(&cVarInMean::SetDefaultInitPoint),
            (boost::python::arg("theMean"), boost::python::arg("theVar")))
        .def("SetDefaultInitPointValue",
            static_cast<void (cVarInMean::*)(cRegArchValue&)>(&cVarInMean::SetDefaultInitPoint))

        // Memory management
        .def("ReAlloc",
            static_cast<void (cVarInMean::*)(uint, uint)>(&cVarInMean::ReAlloc),
            (boost::python::arg("size"), boost::python::arg("param_num") = 0))
        .def("ReAllocVect",
            static_cast<void (cVarInMean::*)(const cDVector&, uint)>(&cVarInMean::ReAlloc),
            (boost::python::arg("vect_param"), boost::python::arg("param_num") = 0))

        // Parameter handling
        .def("SetValueIndex",
            static_cast<void (cVarInMean::*)(double, uint, uint)>(&cVarInMean::Set),
            (boost::python::arg("value"), boost::python::arg("index"), boost::python::arg("param_num")))
        .def("SetVectorIndex",
            static_cast<void (cVarInMean::*)(const cDVector&, uint)>(&cVarInMean::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_num")))
        .def("GetValueIndex",
            static_cast<double (cVarInMean::*)(uint, uint)>(&cVarInMean::Get),
            (boost::python::arg("index"), boost::python::arg("param_num")))
        .def("GetVectorIndex",
            static_cast<cDVector & (cVarInMean::*)(uint)>(&cVarInMean::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_num"))

        // Named parameters
        .def("SetValueName",
            static_cast<void (cVarInMean::*)(double, std::string, uint)>(&cVarInMean::Set),
            (boost::python::arg("value"), boost::python::arg("param_name"), boost::python::arg("index") = 0))
        .def("SetVectorName",
            static_cast<void (cVarInMean::*)(const cDVector&, std::string)>(&cVarInMean::Set),
            (boost::python::arg("vect_param"), boost::python::arg("param_name")))
        .def("GetValueName",
            static_cast<double (cVarInMean::*)(std::string, uint)>(&cVarInMean::Get),
            (boost::python::arg("param_name"), boost::python::arg("index")))
        .def("GetVectorName",
            static_cast<cDVector & (cVarInMean::*)(std::string)>(&cVarInMean::Get),
            return_value_policy<reference_existing_object>(),
            boost::python::arg("param_name"))

        // Core operations
        .def("ComputeMean", &cVarInMean::ComputeMean,
            (boost::python::arg("date"), boost::python::arg("data")))
        .def("GetNParam", &cVarInMean::GetNParam)
        .def("GetNLags", &cVarInMean::GetNLags)

        // Gradient/Hessian
        .def("ComputeGrad", &cVarInMean::ComputeGrad,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"), boost::python::arg("beg_index")))
        .def("ComputeHess", &cVarInMean::ComputeHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"),
                boost::python::arg("hess_data"), boost::python::arg("beg_index")))
        .def("ComputeGradAndHess", &cVarInMean::ComputeGradAndHess,
            (boost::python::arg("date"), boost::python::arg("data"), boost::python::arg("grad_data"),
                boost::python::arg("hess_data"), boost::python::arg("beg_index")))

        // Parameter conversions
        .def("RegArchParamToVector", &cVarInMean::RegArchParamToVector,
            (boost::python::arg("dest_vect"), boost::python::arg("index")))
        .def("VectorToRegArchParam", &cVarInMean::VectorToRegArchParam,
            (boost::python::arg("src_vect"), boost::python::arg("index") = 0))

        // Parameter names
        .def("GetParamNameChar",
            static_cast<void (cVarInMean::*)(uint, char**)>(&cVarInMean::GetParamName),
            boost::python::arg("index"))
        .def("GetParamNameString",
            static_cast<void (cVarInMean::*)(uint, std::string[])>(&cVarInMean::GetParamName),
            boost::python::arg("index"))
        ;
}
