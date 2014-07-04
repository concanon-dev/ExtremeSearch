/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: saQuadRegression

Description:
    Perform Quadratic Regression to generate an algorithm of the form y = coef0*x*x + coef1*x + coef2.  The
    coef's are returned along with std estimate of error.

Functions:
    External:
    saQuadRegressionInitDataRecord
    saQuadRegressionRegress
*/
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "saQuadRegression.h"

static double a[3][3];
static double c[3][3];

inline dataRecordTypePtr saQuadRegressionInitDataRecord(int size, double *X, double *Y, double *Z)
{
    bool zExists = false;
    dataRecordTypePtr p = malloc(sizeof(dataRecordType));
    p->XArray = X;
    p->YArray = Y;
    if (Z != NULL)
    {
        p->ZArray = Z;
        zExists = true;
    }
    else
        p->ZArray = malloc(sizeof(double)*size);
    p->evidenceArray = malloc(sizeof(double)*size);

    int i;
    for(i=0; i<size; i++)
    {
        if (zExists == false)
            p->ZArray[i] = 1.0;
        p->evidenceArray[i] = 1.0;
    }
    p->size = size;

    for(i=0; i<SA_QUADREGRESSION_MAXCOEFFICIENTS; i++)
        p->coef[i] = 0;

    return(p);
}

inline int saQuadRegressionRegress(dataRecordTypePtr p, int length)
{
    double a[3][3];
    double b[3];
    int i;

    for(i=0; i < length; i++)
    {
       double y = p->YArray[i];
       double x = p->XArray[i];
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

    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            p->coef[i] += c[i][j] * b[j];

    return(0);
}

