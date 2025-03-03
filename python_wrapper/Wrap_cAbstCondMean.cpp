#include <boost/python.hpp>
#include "StdAfxRegArchLib.h"  // This umbrella header includes everything.

using namespace boost::python;
using namespace RegArchLib;    // Now cAbstCondMean is visible as cAbstCondMean

void export_cAbstCondMean()
{
    class_<cAbstCondMean, boost::noncopyable>("cAbstCondMean", no_init)
        // If you want to expose some pure virtual method as well:
        // .def("ComputeMean", &cAbstCondMean::ComputeMean) // Actually can't call a pure virtual
        ;
}
