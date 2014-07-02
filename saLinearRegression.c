/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: saLinearRegression

Description:
    Perform Linear Regression to generate an algorithm of the type y = A*x + B.  Return A, B, std estimate of 
    error as a structure. 

Functions:
    External:
    saLinearRegressioncreateLR
    saLinearRegressionGetLine

    Internal:
    computeResiduals

*/
#include <stdlib.h>
#include <math.h>

#include "saLinearRegression.h"

/**
 * Reads in a sequence of pairs of double numbers and computes the
 * best fit (least squares) line y  = ax + b through the set of points.
 * Also computes the correlation coefficient and the standard errror
 * of the regression coefficients.
 **/

inline void computeResiduals(saLinearRegressionTypePtr, double, double, double, double);

saLinearRegressionTypePtr saLinearRegressionCreateLR(double X[], double Y[], int numPoints)
{
    saLinearRegressionTypePtr lrPtr = malloc(sizeof(saLinearRegressionType));
    lrPtr->numPoints = numPoints;
    lrPtr->xAxis = X;
    lrPtr->yAxis = Y;
    lrPtr->coefA = 0.0;
    lrPtr->coefB = 0.0;
    lrPtr->R = 0.0;
    lrPtr->stdError_CoefA = 0.0;
    lrPtr->stdError_CoefB = 0.0;
    return(lrPtr);
}

   /***
   * saLinearRegressionGetLine(). Computes the regression properties and
   * returns the slope and the x-intercept for the equation. This
   * way of returning the regression equation allows us to add
   * an addData() method later to incrementally compute the linear
   * regression line (which will be important in the behavior model).
   ***/
void saLinearRegressionGetLine(saLinearRegressionTypePtr lrPtr)
{
    //
    //---------------------------------------------------------------
    //--first pass: compute the sum of the x and y values and also
    //  the sum of the squares for the x-axis (data value) stream.
    //---------------------------------------------------------------
    //
    double sumx = 0.0;
    double sumy = 0.0;
    double sumx2 = 0.0;

    int n;
    for(n=0; n<lrPtr->numPoints; n++)
    {
         sumx  += lrPtr->xAxis[n];
         sumx2 += lrPtr->xAxis[n] * lrPtr->xAxis[n];
         sumy  += lrPtr->yAxis[n];
    }

    double xbar = sumx / lrPtr->numPoints;
    double ybar = sumy / lrPtr->numPoints;

    //
    //---------------------------------------------------------------
    //--second pass: compute the correlations and develop the
    //--slope and the intercept for the line.
    //---------------------------------------------------------------
    //
    double xxbar = 0.0;
    double yybar = 0.0;
    double xybar = 0.0;
    int i;
    for(i=0; i < lrPtr->numPoints; i++) 
    {
       xxbar += (lrPtr->xAxis[i] - xbar) * (lrPtr->xAxis[i] - xbar);
       yybar += (lrPtr->yAxis[i] - ybar) * (lrPtr->yAxis[i] - ybar);
       xybar += (lrPtr->xAxis[i] - xbar) * (lrPtr->yAxis[i] - ybar);
    }

    //
    //---------------------------------------------------------------
    //--now we compute the coefficients and then the residual data
    //--used to measure the degrees of error in the estimates.
    //---------------------------------------------------------------
    //
    // y  = ((m_coefA) * x) + m_coefB
    //
    lrPtr->coefA = xybar / xxbar;
    lrPtr->coefB = ybar - lrPtr->coefA * xbar;
    //coeff[0]=lrPtr->coefA;  //--slope
    //coeff[1]=lrPtr->coefB;  //--intercept

    computeResiduals(lrPtr, xbar, ybar, yybar, xxbar);
}


   /***
   * computeResiduals(). Given the properties of the regression line, we now
   * loop back across the data points and create an estimate for each actual
   * point (see the 'fit' variable which is the result of the regression
   * equation applied to each data point). From this fit we compute the
   * Pearson's r for the x and y streams as well as the standard error of
   * estimate for the slop and the intercept.
   ***/
inline void computeResiduals(saLinearRegressionTypePtr lrPtr, double xbar, double ybar, double yybar,
                      double xxbar)
{
      int df = lrPtr->numPoints - 2; //--degrees of freedom
      double rss = 0.0;      //--residual sum of squares
      double ssr = 0.0;      //--regression sum of squares

      int i;
      for(i=0; i < lrPtr->numPoints; i++) 
      {
          double fit = lrPtr->coefA *  lrPtr->xAxis[i] + lrPtr->coefB;
          rss += (fit - lrPtr->yAxis[i]) * (fit - lrPtr->yAxis[i]);
          ssr += (fit - ybar) * (fit - ybar);
      }

      lrPtr->R = pow(ssr / yybar, 0.5);
      double svar  = rss / df;
      double svar1 = svar / xxbar;
      double svar0 = svar/lrPtr->numPoints + xbar*xbar*svar1;
      lrPtr->stdError_CoefA = sqrt(svar1);
      lrPtr->stdError_CoefB = sqrt(svar0);
}
