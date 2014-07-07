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
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "saAutoRegression.h"

#define MAXSIZE 40

dataRecordTypePtr saAutoRegressionInitDataRecord(int size, double *X)
{
    bool zExists = false;
    dataRecordTypePtr p = malloc(sizeof(dataRecordType));
    p->XArray = X;
    p->YArray = NULL;
    p->evidenceArray = malloc(sizeof(double)*size);

    int i;
    for(i=0; i<size; i++)
        p->evidenceArray[i] = 1.0;
    p->size = size;

    for(i=0; i<SA_AUTOREGRESSION_MAXCOEFFICIENTS; i++)
        p->coef[i] = 0;

    return(p);
}

void saAutoRegressionRegress(dataRecordTypePtr p, int length)
{
    double a[3][3];
    double b[3];
    int i, j;

    for(i=0; i<3; i++)
    {
        b[i] = 0.0;
        for(j=0; j<3; j++)
            a[i][j] = 0.0;
    }

    for(i=0; i < length; i++)
    {
       double y = p->XArray[i];
       double x = (double)i;
       a[0][0] += 1.0;
       a[0][1] += x;
       a[0][2] += x*x;
       a[1][2] += x*x*x;
       a[2][2] += x*x*x*x;
       b[0]    += y;
       b[1]    += y*x;
       b[2]    += y*x*x;
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

     for(i = 0; i < 3; i++)
       for(j = 0; j < 3; j++)
           p->coef[i] += c[i][j] * b[j];
}

