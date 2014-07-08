// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <iostream>
#include <stdbool.h>
#include "../../saConstants.h"
#include "../../saLinearRegression.h"
#include "gtest/gtest.h"

extern "C" {
saLinearRegressionTypePtr saLinearRegressionCreateLR(double X[], double Y[], int numPoints);
void saLinearRegressionGetLine(saLinearRegressionTypePtr lrPtr);
void computeResiduals(saLinearRegressionTypePtr lrPtr, double xbar, double ybar, double yybar, double xxbar);
}

static double tempXDoubles[SA_CONSTANTS_MAXROWSIZE];
static double tempYDoubles[SA_CONSTANTS_MAXROWSIZE];
int numTempDoubles = 10;

TEST(saLinearRegression, saLinearRegressionCreateLR) {



    int i;
    for (i=0; i < numTempDoubles; i++)
    {
        tempXDoubles[i] = i;
        tempYDoubles[i] = i+1;
    }

    saLinearRegressionTypePtr lrPtr = saLinearRegressionCreateLR(tempXDoubles, tempYDoubles, numTempDoubles);

    EXPECT_EQ(0.0, lrPtr->coefA);
    EXPECT_EQ(0.0, lrPtr->coefB);
    EXPECT_EQ(0.0, lrPtr->stdError_CoefA);
    EXPECT_EQ(0.0, lrPtr->stdError_CoefB);
    EXPECT_EQ(0.0, lrPtr->R);
}

TEST(saLinearRegression, saLinearRegressionGetLine) {

    saLinearRegressionTypePtr lrPtr = saLinearRegressionCreateLR(tempXDoubles, tempYDoubles, numTempDoubles);

    saLinearRegressionGetLine(lrPtr);

    EXPECT_EQ(1, lrPtr->coefA);
    EXPECT_EQ(1, lrPtr->coefB);
    EXPECT_EQ(0, lrPtr->stdError_CoefA);
    EXPECT_EQ(0, lrPtr->stdError_CoefB);
    EXPECT_EQ(1, lrPtr->R);
}
