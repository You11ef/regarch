// Wrap_cError.cpp
#include <boost/python.hpp>
#include "StdAfxError.h"  // This umbrella header includes cError.h

using namespace boost::python;
namespace EN = ErrorNameSpace;

void export_cError()
{
    // Expose cError to Python. 
    // Note: The cError constructor may cause the program to exit if used in code,
    // so be aware that creating cError in Python might terminate the interpreter.
    class_<EN::cError>("cError",
        "Class used to exit program with an output message",
        init<const char*>()
    )
        .def(init<const std::string&>())
        ;
}
