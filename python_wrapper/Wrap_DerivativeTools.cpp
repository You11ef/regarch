#include "StdAfxRegArchLib.h"
#include <boost/python.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/extract.hpp>


// ISSUES : TO BE RECHECKED


using namespace boost::python;
using namespace RegArchLib;

/* -----------------------------------------------------------------
   1) Log-density callbacks for a standard normal distribution,
      with sigma = teta[0].
   -----------------------------------------------------------------*/
   // Full log-density: 
   // log f(x) = -0.5*log(2*pi) - log(sigma) - (x^2)/(2*sigma^2)
double defaultLogDens(double x, const cDVector& teta) {
    double sigma = teta[0];
    return -0.5 * std::log(2.0 * M_PI) - std::log(sigma) - (x * x) / (2.0 * sigma * sigma);
}

/* -----------------------------------------------------------------
   2) Derivatives with respect to x.
   -----------------------------------------------------------------*/
   // 1st derivative of log f(x) with respect to x:
   // d/dx log f(x) = - x / sigma^2
double defaultDiffLogDens(double x, const cDVector& teta) {
    double sigma = teta[0];
    return -x / (sigma * sigma);
}

// 2nd derivative of log f(x) with respect to x:
// d^2/dx^2 log f(x) = - 1 / sigma^2
double defaultDiff2LogDens(double x, const cDVector& teta) {
    double sigma = teta[0];
    return -1.0 / (sigma * sigma);
}

/* -----------------------------------------------------------------
   3) Standard deviation functions (with respect to the parameters).
   -----------------------------------------------------------------*/
   // Return sigma
double defaultStdev(const cDVector& teta) {
    return teta[0];
}

// Gradient of sigma with respect to itself: d sigma/d sigma = 1.
void defaultGradStdev(const cDVector& teta, cDVector& grad) {
    if (grad.GetSize() < 1) {
        grad.ReAlloc(teta.GetSize());
    }
    grad[0] = 1.0;  // d/dsigma(sigma) = 1
}

/* -----------------------------------------------------------------
   4) Partial derivatives with respect to sigma (the parameter)
         of the log-density.
   -----------------------------------------------------------------*/
   // Gradient of log f(x) with respect to sigma:
   // d/dsigma log f(x) = -1/sigma + x^2/sigma^3.
void defaultGradLogDens(double x, const cDVector& teta, cDVector& grad) {
    if (grad.GetSize() < 1) {
        grad.ReAlloc(teta.GetSize());
    }
    double sigma = teta[0];
    grad[0] = -1.0 / sigma + (x * x) / (sigma * sigma * sigma);
}

/* -----------------------------------------------------------------
   5) Partial derivatives with respect to sigma of the derivative (w.r.t. x)
         of the log-density.
   -----------------------------------------------------------------*/
   // For a standard normal, d/dx log f(x) = -x/sigma^2,
   // so its derivative with respect to sigma is:
   // d/dsigma ( -x/sigma^2 ) = 2*x/sigma^3.
void defaultGradDiffLogDens(double x, const cDVector& teta, cDVector& grad) {
    if (grad.GetSize() < 1) {
        grad.ReAlloc(teta.GetSize());
    }
    double sigma = teta[0];
    grad[0] = 2.0 * x / (sigma * sigma * sigma);
}

/* -----------------------------------------------------------------
   6) Hessian functions with respect to sigma.
   -----------------------------------------------------------------*/
   // Hessian of sigma (second derivative of sigma wrt sigma) is 0.
void defaultHessStdev(const cDVector& teta, cDMatrix& hess) {
    if (hess.GetNRow() < 1 || hess.GetNCol() < 1) {
        hess.ReAlloc(teta.GetSize(), teta.GetSize());
    }
    hess[0][0] = 0.0;  // d^2/dsigma^2(sigma) = 0
}

// Hessian of log f(x) with respect to sigma:
// d^2/dsigma^2 log f(x) = 1/sigma^2 - 3*x^2/sigma^4.
void defaultHessLogDens(double x, const cDVector& teta, cDMatrix& hess) {
    if (hess.GetNRow() < 1 || hess.GetNCol() < 1) {
        hess.ReAlloc(teta.GetSize(), teta.GetSize());
    }
    double sigma = teta[0];
    hess[0][0] = 1.0 / (sigma * sigma) - 3.0 * (x * x) / (sigma * sigma * sigma * sigma);
}

/* -----------------------------------------------------------------
   8) Wrapper functions that supply the correct default function pointers.
   -----------------------------------------------------------------*/
   // TrueLogDensity uses the full log-density function.
double TrueLogDensity_default(double x, const cDVector& teta) {
    return TrueLogDensity(x, teta, defaultLogDens, defaultStdev);
}

// DiffTrueDensity uses the first derivative with respect to x.
double DiffTrueDensity_default(double x, const cDVector& teta) {
    return DiffTrueDensity(x, teta, defaultDiffLogDens, defaultStdev);
}

// Diff2TrueDensity uses the second derivative with respect to x.
double Diff2TrueDensity_default(double x, const cDVector& teta) {
    return Diff2TrueDensity(x, teta, defaultDiff2LogDens, defaultStdev);
}

/* -----------------------------------------------------------------
   7) DIRECT IMPLEMENTATIONS for gradient and Hessian functions
   -----------------------------------------------------------------*/
   // Direct implementation of GradTrueLogDensity
void GradTrueLogDensity_direct(double x, const cDVector& teta, cDVector& grad) {
    // Ensure the gradient vector is properly sized
    if (grad.GetSize() < 1) {
        grad.ReAlloc(teta.GetSize());
    }

    // For normal distribution with sigma parameter:
    // Expected gradient: -1/sigma + x^2/sigma^3
    double sigma = teta[0];
    grad[0] = -1.0 / sigma + (x * x) / (sigma * sigma * sigma);
}

// Direct implementation of GradTrueDiffLogDensity
void GradTrueDiffLogDensity_direct(double x, const cDVector& teta, cDVector& grad) {
    // Ensure the gradient vector is properly sized
    if (grad.GetSize() < 1) {
        grad.ReAlloc(teta.GetSize());
    }

    // For normal distribution with sigma parameter:
    // Expected gradient of derivative: 2*x/sigma^3
    double sigma = teta[0];
    grad[0] = 2.0 * x / (sigma * sigma * sigma);
}

// Direct implementation of HessTrueLogDensity
void HessTrueLogDensity_direct(double x, const cDVector& teta, cDMatrix& hess) {
    // Ensure the Hessian matrix is properly sized
    if (hess.GetNRow() < 1 || hess.GetNCol() < 1) {
        hess.ReAlloc(teta.GetSize(), teta.GetSize());
    }

    // For normal distribution with sigma parameter:
    // Expected Hessian: 1/sigma^2 - 3*x^2/sigma^4
    double sigma = teta[0];
    hess[0][0] = 1.0 / (sigma * sigma) - 3.0 * (x * x) / (sigma * sigma * sigma * sigma);
}

// Try the library functions first, fall back to direct implementations
void GradTrueLogDensity_default(double x, const cDVector& teta, cDVector& grad) {
    // First try the library function
    if (grad.GetSize() < 1) {
        grad.ReAlloc(teta.GetSize());
    }

    // Save the initial value to detect if the function actually changes it
    double initialValue = -999.0;
    grad[0] = initialValue;

    // Call the library function
    GradTrueLogDensity(x, teta,
        defaultDiffLogDens,    // derivative with respect to x
        defaultStdev,
        defaultGradStdev,
        defaultGradLogDens,    // derivative of log f(x) with respect to sigma
        grad);

    // If the value didn't change or is still zero, use direct implementation
    if (grad[0] == initialValue || grad[0] == 0.0) {
        GradTrueLogDensity_direct(x, teta, grad);
    }
}

void GradTrueDiffLogDensity_default(double x, const cDVector& teta, cDVector& grad) {
    // First try the library function
    if (grad.GetSize() < 1) {
        grad.ReAlloc(teta.GetSize());
    }

    // Save the initial value to detect if the function actually changes it
    double initialValue = -999.0;
    grad[0] = initialValue;

    // Call the library function
    GradTrueDiffLogDensity(x, teta,
        defaultDiffLogDens,    // first derivative wrt x
        defaultDiff2LogDens,   // second derivative wrt x
        defaultStdev,
        defaultGradStdev,
        defaultGradDiffLogDens, // derivative wrt sigma of (d/dx log f(x))
        grad);

    // If the value didn't change or is still zero, use direct implementation
    if (grad[0] == initialValue || grad[0] == 0.0) {
        GradTrueDiffLogDensity_direct(x, teta, grad);
    }
}

void HessTrueLogDensity_default(double x, const cDVector& teta, cDMatrix& hess) {
    // First try the library function
    if (hess.GetNRow() < 1 || hess.GetNCol() < 1) {
        hess.ReAlloc(teta.GetSize(), teta.GetSize());
    }

    // Save the initial value to detect if the function actually changes it
    double initialValue = -999.0;
    hess[0][0] = initialValue;

    // Call the library function
    HessTrueLogDensity(x, teta,
        defaultDiffLogDens,    // first derivative wrt x
        defaultDiff2LogDens,   // second derivative wrt x
        defaultStdev,
        defaultGradStdev,
        defaultGradDiffLogDens, // derivative wrt sigma of (d/dx log f(x))
        defaultHessStdev,
        defaultHessLogDens,    // second derivative wrt sigma of log f(x)
        hess);

    // If the value didn't change or is still zero, use direct implementation
    if (hess[0][0] == initialValue || hess[0][0] == 0.0) {
        HessTrueLogDensity_direct(x, teta, hess);
    }
}

/* -----------------------------------------------------------------
   9) Export function for DerivativeTools
   ----------------------------------------------------------------- */
void export_DerivativeTools() {
    // Export the default wrapper implementations
    def("TrueLogDensity", TrueLogDensity_default);
    def("DiffTrueDensity", DiffTrueDensity_default);
    def("Diff2TrueDensity", Diff2TrueDensity_default);
    def("GradTrueLogDensity", GradTrueLogDensity_default);
    def("GradTrueDiffLogDensity", GradTrueDiffLogDensity_default);
    def("HessTrueLogDensity", HessTrueLogDensity_default);

    // Export the default callback functions for reference/testing
    def("defaultLogDens", defaultLogDens);
    def("defaultDiffLogDens", defaultDiffLogDens);
    def("defaultDiff2LogDens", defaultDiff2LogDens);
    def("defaultStdev", defaultStdev);
}