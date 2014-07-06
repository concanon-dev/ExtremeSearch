/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: saAutoRegression

Description:
    Perform Auto Regression to generate an algorithm of the form y = coef0*x*x + coef1*x + coef2.  The
    coef's are returned along with std estimate of error.  In the case of auto regress, x is assumed to
    represent some order and X[i] is used as Y.

Functions:
    External:
    saAutoRegressionRegress
*/
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

void saAutoRegressionRegress(double *X, int length, double *coef)
{
    double a[3][3];
    double b[3];
    int i;

    for(i=0; i < length; i++)
    {
       a[0][0] += 1.0;
       a[0][1] += i;
       a[0][2] += i*i;
       a[1][2] += i*i*i;
       a[2][2] += i*i*i*i;
       b[0]    += X[i];
       b[1]    += X[i]*i;
       b[2]    += X[i]*i*i;
    }
    a[1][0] = a[0][1];
    a[1][1] = a[0][2];
    a[2][0] = a[1][1];
    a[2][1] = a[1][2];

    double det = a[0][0]*(a[1][1]*a[2][2]-a[2][1]*a[1][2])
                -a[0][1]*(a[1][0]*a[2][2]-a[1][2]*a[2][0])
                +a[0][2]*(a[1][0]*a[2][1]-a[1][1]*a[2][0]);

    double c[3][3];
    c[0][0] = (a[1][1]*a[2][2] - a[2][1]*a[1][2]) / det;
    c[0][1] = (a[0][2]*a[2][1] - a[0][1]*a[2][2]) / det;
    c[0][2] = (a[0][1]*a[1][2] - a[0][2]*a[1][1]) / det;
    c[1][0] = (a[1][2]*a[2][0] - a[1][0]*a[2][2]) / det;
    c[1][1] = (a[0][0]*a[2][2] - a[0][2]*a[2][0]) / det;
    c[1][2] = (a[1][0]*a[0][2] - a[0][0]*a[1][2]) / det;
    c[2][0] = (a[1][0]*a[2][1] - a[2][0]*a[1][1]) / det;
    c[2][1] = (a[2][0]*a[0][1] - a[0][0]*a[2][1]) / det;
    c[2][2] = (a[0][0]*a[1][1] - a[1][0]*a[0][1]) / det;

    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            coef[i] += c[i][j] * b[j];
}

