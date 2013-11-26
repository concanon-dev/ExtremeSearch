/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

extern double saStatisticsAvg(double[], int, int, int *);
extern double saStatisticsSdev(double[], int, int, int *);

//
static char sStrBuff[256];
//

int saLinearCorrelation(double X[], double Y[], int startIndex, int termCnt, double *R)
{
    double XAvg,
            YAvg,
            XStd,
            YStd,
            XAgg,
            YAgg,
            XYProd;
    int i;
    int ipStatus;

    XAvg = saStatisticsAvg(X, startIndex, termCnt, &ipStatus);
    YAvg = saStatisticsAvg(Y, startIndex, termCnt, &ipStatus);
    XStd = saStatisticsSdev(X, startIndex, termCnt, &ipStatus);
    YStd = saStatisticsSdev(Y, startIndex, termCnt, &ipStatus);
    if (ipStatus != 0)
        return(ipStatus);
    //
    //---------------------------------------------------------------
    //--We now check for a zero or very very small standard deviaiton
    //--which can often result from highly skewed distributions. If
    //--the stddev is zero we cannot divide, thus, we cannot compute
    //--the correlation coefficient. We now set the R to zero, indicate
    //--the condition with a negative status code, and return.
    //---------------------------------------------------------------
    //
    if (fabs(XStd) < .001 || fabs(YStd) < .001) 
    {
        *R = 0;
        return(-1);
    }
    //
    //----------------------------------------------------------------
    //--Now we actually compute the directional "cloud" that describes
    //--the correlation between X and Y. We return the value as "R"
    //----------------------------------------------------------------
    //
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
    return(0);
}
