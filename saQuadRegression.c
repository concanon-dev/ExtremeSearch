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

    Internal:
    checkTheOffset
    checkThePeriod
    matrix3Inverse
    rateOfChange
*/
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "saQuadRegression.h"

static double a[3][3];
static double c[3][3];

bool checkTheOffset(int size, int offset)
{
   int thisOffset=size-(abs(offset)+1);
   if (thisOffset>=0) 
       return(true);
   return(false);
}

bool checkThePeriod(int offset, int periods, int size)
{
   if ((offset+periods) <= size)
       return(true);
   return(false);
}

void matrix3Inverse()
{
    double det = 0;
    det = a[0][0]*a[1][1]*a[2][2] +
          a[1][0]*a[2][1]*a[0][2] +
          a[2][0]*a[0][1]*a[1][2] -
          a[0][2]*a[1][1]*a[2][0] -
          a[1][2]*a[2][1]*a[0][0] -
          a[2][2]*a[0][1]*a[1][0];

    c[0][0] = (a[1][1]*a[2][2] - a[1][2]*a[2][1]) / det;
    c[1][0] = (a[1][0]*a[2][2] - a[1][2]*a[2][0]) / det;
    c[2][0] = (a[1][0]*a[2][1] - a[1][1]*a[2][0]) / det;
    c[0][1] = (a[0][1]*a[2][2] - a[0][2]*a[2][1]) / det;
    c[1][1] = (a[0][0]*a[2][2] - a[0][2]*a[2][0]) / det;
    c[2][1] = (a[0][0]*a[2][1] - a[0][1]*a[2][0]) / det;
    c[0][2] = (a[0][1]*a[1][2] - a[0][2]*a[1][1]) / det;
    c[1][2] = (a[0][0]*a[1][2] - a[0][2]*a[1][0]) / det;
    c[2][2] = (a[0][0]*a[1][1] - a[0][1]*a[1][0]) / det;
}

dataRecordTypePtr saQuadRegressionInitDataRecord(int size, double *X, double *Y, double *Z)
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

//double *predict(dataRecordTypePtr p, int length, int offset, int periods, int futurePeriods,
// int *errCode)
int saQuadRegressionRegress(dataRecordTypePtr p, int length, int offset, int periods)
{
    int limit;
    int myOffset;
    int periodStart;

    if (checkTheOffset(length, offset) == false)
        return(1);

    myOffset = offset;

    if (checkThePeriod(myOffset, periods, length) == false)
        return(2);
 
    periodStart = periods;

    limit = (myOffset+periodStart)-1;

    double b[3];
    int n=0;
    int i;
    int j;
    for(i=myOffset; i<=limit; i++)
    {
        double y = p->YArray[i];
        double count = p->ZArray[i];
        for(j=0; j<count; j++)
        {
            double x = (double)n;
            a[0][0] += 1.0;
            a[0][1] += x;
            a[0][2] += x*x;
            a[1][2] += x*x*x;
            a[2][2] += x*x*x*x;
            b[0]    += y;
            b[1]    += y*x;
            b[2]    += y*x*x;
            n++;
        }
    }
    a[1][0] = a[0][1];
    a[1][1] = a[0][2];
    a[2][0] = a[1][1];
    a[2][1] = a[1][2];

    matrix3Inverse();
    for(i=0; i<3; i++)
        for(j=0; j<3; j++)
            p->coef[i] += c[i][j] * b[j];
/*
    int currElm = length;
    int futureElm = currElm + futurePeriods;
    double *predicted = malloc(sizeof(double)*((futureElm-currElm)+1));

    int k = 0;
    for(i=currElm; i<futureElm; i++)
    {
        double x = (double)i;
        predicted[k++] = p->coef[0] + p->coef[1]*x + p->coef[2]*x*x;
    }
    return(predicted);
*/
    return(0);
}

double rateOfChange(dataRecordTypePtr p, int length, int offset, int periods, int *errCode) 
{
    int limit;
    int myOffset;
    int periodStart;

    *errCode = 0;
    if (checkTheOffset(length, offset) == false)
    {
        *errCode = 1;
        return(0.0);
    }
    myOffset = offset;

    if (checkThePeriod(myOffset, periods, length) == false)
    {
        *errCode = 1;
        return(0.0);
    }
    periodStart = periods;
 
    limit = myOffset + periodStart;

    double **a = malloc(sizeof(double)*2*2);
    double  *b = malloc(sizeof(double)*2);

    int i;
    for(i=myOffset; i<limit; i++)
    {
        double t0 = p->XArray[i];
        double y = p->YArray[i];
        double count = p->ZArray[i];

        int j;
        for(j=0; j<count; j++)
        {
            double t = t0 + j;
            a[0][0] += t*t;
            a[0][1] += t;
            a[1][1] += 1.0;
            b[0]    += t*y;
            b[1]    += y;
        }
    }
    double det = a[0][0]*a[1][1] - a[0][1]*a[0][1];
    double slope = (b[0]*a[1][1] - b[1]*a[0][1]) / det;

    return(slope);
}

