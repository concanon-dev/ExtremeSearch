// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <iostream>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "../../saContext.h"
#include "../../saConcept.h"
#include "../../saSplunk.h"
#include "gtest/gtest.h"

extern "C" {
saContextTypePtr saContextLoad(FILE *infile);
saConceptTypePtr saConceptInit(char *name, double domainMin, double domainMax);
double saConceptLookup(saConceptTypePtr conceptPtr, double value);
void saConceptCopy(saConceptTypePtr newTerm, saConceptTypePtr oldTerm);
saConceptTypePtr saConceptCreateCurveDecrease(char *name, double domainMin, double domainMax, double termMin, double termMax, double inflectionPoint);
saConceptTypePtr saConceptCreateCurveIncrease(char *name, double domainMin, double domainMax, double termMin, double termMax, double inflectionPoint);
saConceptTypePtr saConceptCreateLinearDecrease(char *name, double domainMin, double domainMax, double termMin, double termMax);
saConceptTypePtr saConceptCreateLinearIncrease(char *name, double domainMin, double domainMax, double termMin, double termMax);
saConceptTypePtr saConceptCreatePI(char *name, double domainMin, double domainMax, double termMin, double termMax, double center);
saConceptTypePtr saConceptCreateTrapezoid(char *name, double domainMin, double domainMax, double termMin, double termMax);
saConceptTypePtr saConceptCreateTriangle(char *name, double domainMin, double domainMax, double termMin, double termMax, double center);
}

TEST(saContext, saConceptInit) {

    char name[128] = "TESTCONCEPT";
    double domainMin = 0.0;
    double domainMax = 50.0;

    saConceptTypePtr conceptTypePtr = saConceptInit(name, domainMin, domainMax);

    EXPECT_STREQ("TESTCONCEPT", conceptTypePtr->name);
    char buffer[32];
    sprintf(buffer, "%g", conceptTypePtr->indexVector[0]);
    EXPECT_STREQ("0", buffer);
    sprintf(buffer, "%g", conceptTypePtr->indexVector[1]);
    EXPECT_STREQ("0.195312", buffer);
    sprintf(buffer, "%g", conceptTypePtr->indexVector[2]);
    EXPECT_STREQ("0.390625", buffer);
    sprintf(buffer, "%g", conceptTypePtr->indexVector[3]);
    EXPECT_STREQ("0.585938", buffer);
}

TEST(saContext, saConceptLoookup) {

    char path[256] = "etc/apps/xtreme/contexts/TEST.context";
    FILE *f = fopen(path, "r");
    saContextTypePtr contextPtr = saContextLoad(f);
    fclose(f);

    double value = 70;
    double result = saConceptLookup(contextPtr->concepts[0], value);
    EXPECT_EQ(0, result);
    result = saConceptLookup(contextPtr->concepts[1], value);
    EXPECT_EQ(0, result);
    result = saConceptLookup(contextPtr->concepts[2], value);
    EXPECT_EQ(0.0703125, result);
}

TEST(saContext, saConceptCreateCurveDecrease) {
    char termName[128] = "TESTTERM";
    double domainMin = 0;
    double domainMax = 20;
    double min = 1;
    double max = 19;
    saConceptTypePtr conceptPtr = saConceptCreateCurveDecrease(termName, domainMin, domainMax, min, max, (max+min)/2);
    EXPECT_EQ(1, conceptPtr->points[0]);
    EXPECT_EQ((max+min)/2, conceptPtr->points[1]);
    EXPECT_EQ(19, conceptPtr->points[2]);
}

TEST(saContext, saConceptCreateCurveIncrease) {
    char termName[128] = "TESTTERM";
    double domainMin = 0;
    double domainMax = 20;
    double min = 1;
    double max = 19;
    saConceptTypePtr conceptPtr = saConceptCreateCurveIncrease(termName, domainMin, domainMax, min, max, (max+min)/2);
    EXPECT_EQ(1, conceptPtr->points[0]);
    EXPECT_EQ((max+min)/2, conceptPtr->points[1]);
    EXPECT_EQ(19, conceptPtr->points[2]);
}
/*
TEST(saContext, saConceptCreateLinearDecrease) {
    char termName[128] = "TESTTERM";
    double domainMin = 0;
    double domainMax = 20;
    double min = 1;
    double max = 19;
    saConceptTypePtr conceptPtr = saConceptCreateLinearDecrease(termName, domainMin, domainMax, min, max);
    EXPECT_EQ(1, conceptPtr->points[0]);
    EXPECT_EQ(19, conceptPtr->points[1]);
}
*/
TEST(saContext, saConceptCreateLinearIncrease) {
    char termName[128] = "TESTTERM";
    double domainMin = 0;
    double domainMax = 20;
    double min = 1;
    double max = 19;
    saConceptTypePtr conceptPtr = saConceptCreateLinearIncrease(termName, domainMin, domainMax, min, max);
    EXPECT_EQ(1, conceptPtr->points[0]);
    EXPECT_EQ(19, conceptPtr->points[1]);
}

TEST(saContext, saConceptCreatePI) {
    char termName[128] = "TESTTERM";
    double domainMin = 0;
    double domainMax = 20;
    double min = 1;
    double max = 19;
    saConceptTypePtr conceptPtr = saConceptCreatePI(termName, domainMin, domainMax, min, max, (max+min)/2);
    double termSize = max - min;
    double segmentSize = termSize/4;
    double leftInflection = min + segmentSize;
    double rightInflection = max - segmentSize;
    EXPECT_EQ(1, conceptPtr->points[0]);
    EXPECT_EQ(leftInflection, conceptPtr->points[1]);
    EXPECT_EQ((max+min)/2, conceptPtr->points[2]);
    EXPECT_EQ(rightInflection, conceptPtr->points[3]);
    EXPECT_EQ(19, conceptPtr->points[4]);
}
TEST(saContext, saConceptCreateTrapezoid) {
    char termName[128] = "TESTTERM";
    double domainMin = 0;
    double domainMax = 20;
    double min = 1;
    double max = 19;
    saConceptTypePtr conceptPtr = saConceptCreateTrapezoid(termName, domainMin, domainMax, min, max);
    double termSize = max - min;
    double trapSize = termSize * SA_CONCEPT_TRAPEZOID_SIZE;
    EXPECT_EQ(min, conceptPtr->points[0]);
    EXPECT_EQ(min+trapSize, conceptPtr->points[1]);
    EXPECT_EQ(max-trapSize, conceptPtr->points[2]);
    EXPECT_EQ(max, conceptPtr->points[3]);
}
TEST(saContext, saConceptCreateTriangle) {
    char termName[128] = "TESTTERM";
    double domainMin = 0;
    double domainMax = 20;
    double min = 1;
    double max = 19;
    saConceptTypePtr conceptPtr = saConceptCreateTriangle(termName, domainMin, domainMax, min, max, (max+min)/2);
    EXPECT_EQ(min, conceptPtr->points[0]);
    EXPECT_EQ((max+min)/2, conceptPtr->points[1]);
    EXPECT_EQ(max, conceptPtr->points[2]);
}
