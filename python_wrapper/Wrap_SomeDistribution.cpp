// not used


#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;
using namespace RegArchLib;

void export_SomeDistribution()
{
    // Wrap functions returning double directly:
    def("StudentLogDensity", StudentLogDensity);
    def("SkewtLogDensity", SkewtLogDensity);
    def("SkewtDiffLogDensity", SkewtDiffLogDensity);
    def("SkewtExpect", SkewtExpect);
    def("SkewtVar", SkewtVar);

    // Functions that compute gradients; assume that cDVector is already wrapped.
    def("StudentGradLogDensity", StudentGradLogDensity);
    def("SkewtGradLogDensity", SkewtGradLogDensity);
    def("GradSkewtExpect", GradSkewtExpect);
    def("GradSkewtVar", GradSkewtVar);

    // Mixed normal functions – note that some are overloaded.
    def("MixNormSigma", (double(*)(double, double, double))MixNormSigma);
    def("MixNormSigma_vec", (double(*)(const cDVector&))MixNormSigma);

    def("MixNormGradSigma", (void(*)(double, double, double, cDVector&))MixNormGradSigma);
    def("MixNormGradSigma_vec", (void(*)(const cDVector&, cDVector&))MixNormGradSigma);

    def("MixNormHessSigma", (void(*)(double, double, double, cDMatrix&))MixNormHessSigma);
    def("MixNormHessSigma_vec", (void(*)(const cDVector&, cDMatrix&))MixNormHessSigma);

    def("MixNormLogDensity", (double(*)(double, double, double, double))MixNormLogDensity);
    def("MixNormLogDensity_vec", (double(*)(double, const cDVector&))MixNormLogDensity);

    def("MixNormDiffLogDensity", (double(*)(double, double, double, double))MixNormDiffLogDensity);
    def("MixNormDiffLogDensity_vec", (double(*)(double, const cDVector&))MixNormDiffLogDensity);

    def("MixNormDiff2LogDensity", (double(*)(double, double, double, double))MixNormDiff2LogDensity);
    def("MixNormDiff2LogDensity_vec", (double(*)(double, const cDVector&))MixNormDiff2LogDensity);

    def("MixNormGradDiffLogDens", (void(*)(double, double, double, double, cDVector&))MixNormGradDiffLogDens);
    def("MixNormGradDiffLogDens_vec", (void(*)(double, const cDVector&, cDVector&))MixNormGradDiffLogDens);

    def("MixNormGradLogDensity", (void(*)(double, double, double, double, cDVector&))MixNormGradLogDensity);
    def("MixNormGradLogDensity_vec", (void(*)(double, const cDVector&, cDVector&))MixNormGradLogDensity);

    def("MixNormHessLogDensity", (void(*)(double, double, double, double, cDMatrix&))MixNormHessLogDensity);
    def("MixNormHessLogDensity_vec", (void(*)(double, const cDVector&, cDMatrix&))MixNormHessLogDensity);
}
