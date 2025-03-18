#include <boost/python.hpp>
using namespace boost::python;


void export_cDVector_converters();


void export_DerivativeTools();
void export_RegArchDef();
void export_cGSLVector();
void export_cGSLMatrix();
void export_cError();

void export_cRegArchValue();

void export_cAbstCondMean();
void export_cAbstCondVar();
void export_cAbstResiduals();

void export_cRegArchHessien();
void export_cRegArchGradient();

void export_cNumericDerivative();

void export_cAparch();
void export_cAr();
void export_cArch();
void export_cArfima();
void export_cConst();
void export_cCondMean();
void export_cConstCondVar();
void export_cEgarch();
void export_cFigarch();
void export_cGarch();
void export_cGedResiduals();
void export_cGtarch();
void export_cLinReg();
void export_cMa();
void export_cMixNormResiduals();
void export_cNagarch();
void export_cNgarch();
void export_cNormResiduals();
void export_cSqrgarch();
void export_cStdDevInMean();
void export_cStudentResiduals();
void export_cTarch();
void export_cTsgarch();
void export_cUgarch();
void export_cVarInMean();

void export_SomeDistribution();
void export_cRegArchModel();
void export_RegArchCompute();

void export_cPolynom();


BOOST_PYTHON_MODULE(regarch_wrapper)
{

    export_cDVector_converters();

    export_DerivativeTools();
    export_RegArchDef();
    export_cGSLVector();
    export_cGSLMatrix();
    export_cError();

    export_cRegArchValue();

    export_cAbstCondMean();
    export_cAbstCondVar();
    export_cAbstResiduals();

    export_cRegArchHessien();
    export_cRegArchGradient();

    export_cNumericDerivative();


    export_cAparch();
    export_cAr();
    export_cArch();
    export_cArfima();
    export_cConst();
    export_cCondMean();
    export_cConstCondVar();
    export_cEgarch();
    export_cFigarch();
    export_cGarch();
    export_cGedResiduals();
    export_cGtarch();
    export_cLinReg();
    export_cMa();
    export_cMixNormResiduals();
    export_cNagarch();
    export_cNgarch();
    export_cNormResiduals();
    export_cSqrgarch();
    export_cStdDevInMean();
    export_cStudentResiduals();
    export_cTarch();
    export_cTsgarch();
    export_cUgarch();
    export_cVarInMean();

    export_SomeDistribution();
    export_cRegArchModel();
    export_RegArchCompute();
    export_cPolynom();
}
