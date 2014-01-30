/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern bool saStatisticsAvg(double[], int, int, double *);
extern bool saStatisticsSdev(double[], int, int, double *);

bool saLinearCorrelation(double X[], double Y[], int startIndex, int termCnt, double *R)
{
    double XAgg;
    double XAvg;
    double XStd;
    double XYProd;
    double YAgg;
    double YAvg;
    double YStd;
    int i;

    if (saStatisticsAvg(X, startIndex, termCnt, &XAvg) == false)
        return(false);
    if (saStatisticsAvg(Y, startIndex, termCnt, &YAvg) == false)
        return(false);
    if (saStatisticsSdev(X, startIndex, termCnt, &XStd) == false)
        return(false);
    if (saStatisticsSdev(Y, startIndex, termCnt, &YStd) == false)
        return(false);

    if (fabs(XStd) < 0.001 || fabs(YStd) < 0.001) 
    {
        *R = 0;
        return(false);
    }

    XAgg = 0;
    YAgg = 0;
    XYProd = 0;
    for (i = startIndex; i < termCnt; i++) 
    {
        XAgg = (X[i] - XAvg) / XStd;
        YAgg = (Y[i] - YAvg) / YStd;
        XYProd = XYProd + (XAgg * YAgg);
    }
    *R = XYProd / (double) termCnt;
    return(true);
}
