// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <iostream>
#include <stdbool.h>
#include "../../saConstants.h"
#include "../../saQuadRegression.h"
#include "gtest/gtest.h"

extern "C" {
dataRecordTypePtr saQuadRegressionInitDataRecord(int size, double *X, double *Y, double *Z);
int saQuadRegressionRegress(dataRecordTypePtr p, int length);
}

static double tempXDoubles[SA_CONSTANTS_MAXROWSIZE];
static double tempYDoubles[SA_CONSTANTS_MAXROWSIZE];
int numTempDoubles = 10;

TEST(saQuadRegression, saQuadRegressionInitDataRecord) {

    int i;
    for (i=0; i < numTempDoubles; i++)
    {
        tempXDoubles[i] = i;
        tempYDoubles[i] = i+1;
    }

    dataRecordTypePtr p = saQuadRegressionInitDataRecord(numTempDoubles, tempXDoubles, tempYDoubles, NULL);

    EXPECT_EQ(numTempDoubles, p->size);

    for(i=0; i<SA_QUADREGRESSION_MAXCOEFFICIENTS; i++)
        EXPECT_EQ(0, p->coef[i]);

    for(i=0; i<numTempDoubles; i++)
        EXPECT_EQ(1.0, p->evidenceArray[i]);

}

TEST(saQuadRegression, saQuadRegressionRegress) {

    dataRecordTypePtr p = saQuadRegressionInitDataRecord(numTempDoubles, tempXDoubles, tempYDoubles, NULL);

    saQuadRegressionRegress(p, numTempDoubles);

    EXPECT_LT(0, p->coef[0]);
    EXPECT_EQ(1, p->coef[1]);
    EXPECT_EQ(0, p->coef[2]);
}
