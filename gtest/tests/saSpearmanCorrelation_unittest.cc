// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <iostream>
#include <stdbool.h>
#include "../../saConstants.h"
#include "gtest/gtest.h"

extern "C" {
void saSpearmanCorrelation(double x[], double y[], int termCnt, double *r);
}

static double x[SA_CONSTANTS_MAXROWSIZE];
static double y[SA_CONSTANTS_MAXROWSIZE];

TEST(saSpearmanCorrelation, saSpearmanCorrelation) {

    int termCnt = 10;
    double r = 0;


    int i;
    for (i=0; i < termCnt; i++)
    {
        x[i] = i;
        y[i] = i+1;
    }
    saSpearmanCorrelation(x, y, termCnt, &r);

    char result[255];
    sprintf(result, "%f", r);
    EXPECT_STREQ("0.245455", result);
}
