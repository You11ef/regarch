#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>


using namespace boost::python;
using namespace RegArchLib;

// Helper function: capture Print() output into a string.
std::string cRegArchModel_print(cRegArchModel& model)
{
    std::ostringstream oss;
    model.Print(oss);
    return oss.str();
}

void export_cRegArchModel()
{
    class_<cRegArchModel>("cRegArchModel", "General RegArch Model")
        .def("GetNMean", &cRegArchModel::GetNMean)
        .def("SetMean", &cRegArchModel::SetMean)
        .def("AddOneMean", &cRegArchModel::AddOneMean)
        .def("GetOneMean", &cRegArchModel::GetOneMean,
            return_value_policy<reference_existing_object>())
        .def("SetVar", &cRegArchModel::SetVar)
        .def("GetVar", &cRegArchModel::GetVar,
            return_value_policy<reference_existing_object>())
        .def("SetResid", &cRegArchModel::SetResid)
        .def("GetResid", &cRegArchModel::GetResid,
            return_value_policy<reference_existing_object>())
        .def("Print", cRegArchModel_print)
        .def("GetNParam", &cRegArchModel::GetNParam)
        .def("GetNLags", &cRegArchModel::GetNLags)
        ;
}
