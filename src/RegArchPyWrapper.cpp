#include <boost/python.hpp>
using namespace boost::python;

void export_cGSLVector();
void export_cGSLMatrix();
// Other export functions (e.g., export_cError, export_cAbstCondMean, etc.)

BOOST_PYTHON_MODULE(regarch_wrapper)
{
    export_cGSLVector();
    export_cGSLMatrix();
    // Other export calls...
}
