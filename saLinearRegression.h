#ifndef _regress_
#define _regress_

typedef struct saLinearRegression
{
    double     coefA;
    double     coefB;
    double     R;
    double     stdError_CoefA;
    double     stdError_CoefB;
    double     *xAxis;
    double     *yAxis;
    int        numPoints;
} saLinearRegressionType;
typedef saLinearRegressionType *saLinearRegressionTypePtr;

#endif
