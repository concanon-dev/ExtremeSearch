/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: saAutoRegression

Description:
    performs maxentropy autoregression

Functions:
    External:
    saAutoRegression

*/
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "saAutoRegression.h"

bool saAutoRegression(double *inputSeries, int numSeries, double *coef)
{
    double autoMatrix[SA_AUTOREGRESSION_NUMCOEFS+1][SA_AUTOREGRESSION_NUMCOEFS+1];
    double g[SA_AUTOREGRESSION_NUMCOEFS+2];
    double h[SA_AUTOREGRESSION_NUMCOEFS+1];
    double tmpF[numSeries+1];
    double tmpR[numSeries+1];
    int i, j;
 
    for(i=1; i<=SA_AUTOREGRESSION_NUMCOEFS; i++) 
    {
        double sNumerator = 0.0;
        double sDenominator = 0.0;
        double t1, t2;
        int endI = numSeries - i;
        
        for(j=0; j<endI; j++)
        {
            t1 = inputSeries[j+i] + tmpF[j];
            t2 = inputSeries[j] + tmpR[j];
            sNumerator -= 2.0 * t1 * t2;
            sDenominator += (t1 * t1) + (t2 * t2);
        }
        g[i] = sNumerator / sDenominator;
        t1 = g[i];
        
        if (i != 1) 
        {
            for(j=1; j<i; j++)
                h[j] = g[j] + t1 * g[i-j];
            for (j=1; j<i; j++)
                g[j] = h[j];
            endI--;
        }
        for(j=0; j<endI; j++)
        {
            tmpR[j] += t1 * tmpF[j] + t1 * inputSeries[i+j];
            tmpF[j] = tmpF[j+1] + t1 * tmpR[j+1] + t1 * inputSeries[j+1];
        }

        for(j=0; j<i; j++)
            autoMatrix[i][j] = g[j+1];
    }

    // Get the coefs
    for(i=0; i<SA_AUTOREGRESSION_NUMCOEFS; i++)
        coef[i] = -autoMatrix[SA_AUTOREGRESSION_NUMCOEFS][i];
    return(true);
}

#ifdef _MAIN_
int main(int argc, char *argv[])
{
    double X[10000];
    double coef[3];
    
    int i;
    for(i=0; i<10000; i++)
    {
        X[i] = (double) i * 10.0;
    }
    
    bool worked = saAutoRegression(X, 10000, coef);
    
    if (worked == true)
        for(i=0; i<3; i++)
            fprintf(stderr, "coef[%d]=%.4f\n", i, coef[i]);
    else
        fprintf(stderr, "oops\n");
    
}
#endif
