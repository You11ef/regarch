// regarch_wrapper.cpp
#include <boost/python.hpp>
using namespace boost::python;

// Forward declarations for export functions
void export_cError();
void export_cGSLVector();
void export_cGSLMatrix();
void export_cTabOfString();

BOOST_PYTHON_MODULE(regarch_wrapper)
{
    export_cError();
    export_cGSLVector();
    export_cGSLMatrix();
    export_cTabOfString();
    // ... add additional export functions here
}
