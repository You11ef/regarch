// not used

#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>


using namespace boost::python;
using namespace RegArchLib;

// Exception translator for cError.
void translate_cError(const ErrorNameSpace::cError& e)
{
    // For simplicity, we set a generic error message.
    PyErr_SetString(PyExc_RuntimeError, "RegArch cError thrown");
}

// Expose the cError class from the ErrorNameSpace.
// We expose only the constructor taking a const char*.
void export_cError()
{
    // Register the exception translator.
    register_exception_translator<ErrorNameSpace::cError>(translate_cError);

    // Expose the cError class as "cError" in Python.
    class_<ErrorNameSpace::cError>("cError",
        "Error class for RegArch. This error is thrown when a fatal error occurs.",
        init<const char*>()
    );
}
