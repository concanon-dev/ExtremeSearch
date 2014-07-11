// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <iostream>
#include <stdbool.h>
#include "../../saConstants.h"
#include "gtest/gtest.h"

extern "C" {
bool saLinearCorrelation(double x[], double y[], int startIndex, int termCnt, double *r);
}

static double x[SA_CONSTANTS_MAXROWSIZE];
static double y[SA_CONSTANTS_MAXROWSIZE];

TEST(saLinearCorrelation, saLinearCorrelation) {

    int startIndex = 0;
    int termCnt = 10;
    double r = 0;


    int i;
    for (i=0; i < termCnt; i++)
    {
        x[i] = i;
        y[i] = i+1;
    }

    bool result = saLinearCorrelation(x, y, startIndex, termCnt, &r);

    EXPECT_EQ(true, result);
    EXPECT_EQ(1, r);
}
