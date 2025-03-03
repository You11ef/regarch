// Wrap_cError.cpp
#include <boost/python.hpp>
#include "StdAfxError.h"    // This includes cError.h and TargetVerError.h

using namespace boost::python;
namespace EN = ErrorNameSpace;

void export_cError()
{
    // Expose cError as a type that cannot be constructed from Python.
    class_<EN::cError, boost::noncopyable>("cError", no_init);
}
