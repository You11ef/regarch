// Wrap_cTabOfString.cpp
#include <boost/python.hpp>
#include "cTabOfString.h"

namespace bp = boost::python;
using namespace RegArchLib;

void export_cTabOfString() {
    bp::class_<cTabOfString>("cTabOfString",
        bp::init<int>((bp::arg("theSize") = 0),
            "Construct a cTabOfString with a given size (default=0).")
    )
        .def("Delete", &cTabOfString::Delete,
            "Delete (free) the internal string array.")
        .def("ReAlloc", &cTabOfString::ReAlloc,
            bp::arg("theSize"),
            "Reallocate the internal array to a new size.")
        // Expose mSize as a read/write property
        .add_property("mSize",
            bp::make_getter(&cTabOfString::mSize),
            bp::make_setter(&cTabOfString::mSize),
            "The number of strings in the tab.")
        ;
}
