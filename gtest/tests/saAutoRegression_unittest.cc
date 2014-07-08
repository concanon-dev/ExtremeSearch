// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <iostream>
#include <stdbool.h>
#include "../../saConstants.h"
#include "../../saAutoRegression.h"
#include "gtest/gtest.h"

extern "C" {
dataRecordTypePtr saAutoRegressionInitDataRecord(int size, double *X);
void saAutoRegressionRegress(dataRecordTypePtr p, int length);
}

static double tempXDoubles[SA_CONSTANTS_MAXROWSIZE];
int numTempDoubles = 10;

TEST(saAutoRegression, saAutoRegressionInitDataRecord) {

    int i;
    for (i=0; i < numTempDoubles; i++)
        tempXDoubles[i] = i;

    dataRecordTypePtr p = saAutoRegressionInitDataRecord(numTempDoubles, tempXDoubles);


    EXPECT_EQ(numTempDoubles, p->size);

    for(i=0; i<SA_AUTOREGRESSION_MAXCOEFFICIENTS; i++)
        EXPECT_EQ(0, p->coef[i]);

    for(i=0; i<numTempDoubles; i++)
        EXPECT_EQ(1.0, p->evidenceArray[i]);
}

TEST(saAutoRegression, saAutoRegressionRegress) {

    dataRecordTypePtr p = saAutoRegressionInitDataRecord(numTempDoubles, tempXDoubles);
    saAutoRegressionRegress(p, numTempDoubles);

    EXPECT_EQ(numTempDoubles, p->size);

    EXPECT_GT(0, p->coef[0]);
    EXPECT_EQ(1, p->coef[1]);
    EXPECT_LT(0, p->coef[2]);

    int i = 0;
    for(i=0; i<numTempDoubles; i++)
        EXPECT_EQ(i, p->XArray[i]);

}
