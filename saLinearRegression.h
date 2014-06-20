/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software
 contains proprietary trade and business secrets.
*/
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
