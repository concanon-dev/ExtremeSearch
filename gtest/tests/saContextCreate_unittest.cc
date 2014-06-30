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
#include "../../saSplunk.h"
#include "gtest/gtest.h"

extern "C" {
saContextTypePtr saContextCreateAvgCentered(char *name, double avg, double sdev, char *terms[], int numTerms, char *shape, char *endShape, int count, char *notes, char *uom);
saContextTypePtr saContextCreateDomain(char *name, double domainMin, double domainMax, char *terms[], int numTerms, char *shape, char *endShape, int count, char *notes, char *uom);
void saContextCreateConcept(saContextTypePtr cPtr, char *termName, char *shape, double min, double max);
saContextTypePtr saContextCreateConcepts(char *name, double domainMin, double domainMax, double avg, double sdev, double count, double halfTerm, char *terms[], int numTerms, char *shape, char *endShape, char *notes, char *uom);
void saContextRecreateConcepts(saContextTypePtr cPtr, double min, double max);
}

TEST(saContextCreate, saContextCreateAvgCentered) {

    char name[128] = "TESTAC";
    double avg = 0;
    double sdev = 1;
    int numTerms = 3;
    char term1[128] = "small";
    char term2[128] = "medium";
    char term3[128] = "large";
    char *terms[SA_CONTEXT_MAXTERMS];
    terms[0] = term1;
    terms[1] = term2;
    terms[2] = term3;
    char shape[128];
    strcpy(shape, SA_CONCEPT_SHAPE_CURVE);
    char endShape[128];
    strcpy(endShape, SA_CONCEPT_SHAPE_PI);
    int count = 0;
    char notes[128] = "This is a note";
    char uom[128] = "size";

    saContextTypePtr contextTypePtr = saContextCreateAvgCentered(name, avg, sdev, terms, numTerms, shape, endShape, count, notes, uom);

    EXPECT_STREQ("TESTAC", contextTypePtr->name);
    EXPECT_EQ(0, contextTypePtr->avg);
    EXPECT_EQ(1, contextTypePtr->sdev);
    EXPECT_EQ(0, contextTypePtr->count);
    EXPECT_EQ(3, contextTypePtr->numConcepts);
    EXPECT_STREQ("This is a note", contextTypePtr->notes);
    EXPECT_STREQ("size", contextTypePtr->uom);

}

TEST(saContextCreate, saContextCreateDomain) {

    char name[128] = "TESTD";
    double domainMin = 0;
    double domainMax = 20;
    int numTerms = 3;
    char term1[128] = "small";
    char term2[128] = "medium";
    char term3[128] = "large";
    char *terms[SA_CONTEXT_MAXTERMS];
    terms[0] = term1;
    terms[1] = term2;
    terms[2] = term3;
    char shape[128];
    strcpy(shape, SA_CONCEPT_SHAPE_CURVE);
    char endShape[128];
    strcpy(endShape, SA_CONCEPT_SHAPE_PI);
    int count = 0;
    char notes[128] = "This is a note";
    char uom[128] = "size";

    saContextTypePtr contextTypePtr = saContextCreateDomain(name, domainMin, domainMax, terms, numTerms, shape, endShape, count, notes, uom);

    EXPECT_STREQ("TESTD", contextTypePtr->name);
    EXPECT_EQ(0, contextTypePtr->domainMin);
    EXPECT_EQ(20, contextTypePtr->domainMax);
    EXPECT_EQ(0, contextTypePtr->count);
    EXPECT_EQ(3, contextTypePtr->numConcepts);
    EXPECT_STREQ("This is a note", contextTypePtr->notes);
    EXPECT_STREQ("size", contextTypePtr->uom);

}

TEST(saContextCreate, saContextCreateConcepts) {
    char name[128] = "TESTD";
    double domainMin = 0;
    double domainMax = 20;
    double avg = 0;
    double sdev = 0;
    int numTerms = 3;
    char term1[128] = "small";
    char term2[128] = "medium";
    char term3[128] = "large";
    char *terms[SA_CONTEXT_MAXTERMS];
    terms[0] = term1;
    terms[1] = term2;
    terms[2] = term3;
    char shape[128];
    strcpy(shape, SA_CONCEPT_SHAPE_CURVE);
    char endShape[128];
    strcpy(endShape, SA_CONCEPT_SHAPE_PI);
    int count = 0;
    char notes[128] = "This is a note";
    char uom[128] = "size";
    double halfTerm = ((domainMax - domainMin) / (numTerms - 1));

    saContextTypePtr contextTypePtr = saContextCreateConcepts(name, domainMin, domainMax, avg, sdev, count, halfTerm, terms, numTerms, shape, endShape, notes, uom);

    EXPECT_STREQ("TESTD", contextTypePtr->name);
    EXPECT_EQ(0, contextTypePtr->domainMin);
    EXPECT_EQ(20, contextTypePtr->domainMax);
    EXPECT_EQ(0, contextTypePtr->count);
    EXPECT_EQ(3, contextTypePtr->numConcepts);
    EXPECT_STREQ("This is a note", contextTypePtr->notes);
    EXPECT_STREQ("size", contextTypePtr->uom);
}

TEST(saContextCreate, saContextCreateConcept) {

    char name[128] = "TESTD";
    double domainMin = 0;
    double domainMax = 20;
    double avg = 0;
    double sdev = 0;
    int numTerms = 3;
    char term1[128] = "small";
    char term2[128] = "medium";
    char term3[128] = "large";
    char *terms[SA_CONTEXT_MAXTERMS];
    terms[0] = term1;
    terms[1] = term2;
    terms[2] = term3;
    char shape[128];
    strcpy(shape, SA_CONCEPT_SHAPE_CURVE);
    char endShape[128];
    strcpy(endShape, SA_CONCEPT_SHAPE_PI);
    int count = 0;
    char notes[128] = "This is a note";
    char uom[128] = "size";
    double halfTerm = ((domainMax - domainMin) / (numTerms - 1));

    saContextTypePtr contextTypePtr = saContextCreateConcepts(name, domainMin, domainMax, avg, sdev, count, halfTerm, terms, numTerms, shape, endShape, notes, uom);

    EXPECT_STREQ("TESTD", contextTypePtr->name);
    EXPECT_EQ(0, contextTypePtr->domainMin);
    EXPECT_EQ(20, contextTypePtr->domainMax);
    EXPECT_EQ(0, contextTypePtr->count);
    EXPECT_EQ(3, contextTypePtr->numConcepts);
    EXPECT_STREQ("This is a note", contextTypePtr->notes);
    EXPECT_STREQ("size", contextTypePtr->uom);

    char termName1[126] = "xsmall";
    char termName2[126] = "xlarge";
    double min1 = 0;
    double min2 = 20;
    double max1 = 3;
    double max2 = 26;
    saContextCreateConcept(contextTypePtr, termName1, shape, min1, max1);
    saContextCreateConcept(contextTypePtr, termName2, shape, min2, max2);

    EXPECT_EQ(5, contextTypePtr->numConcepts);
}

TEST(saContextCreate, saContextRecreateConcepts) {

    char name[128] = "TESTD";
    double domainMin = 0;
    double domainMax = 20;
    double avg = 0;
    double sdev = 0;
    int numTerms = 3;
    char term1[128] = "small";
    char term2[128] = "medium";
    char term3[128] = "large";
    char *terms[SA_CONTEXT_MAXTERMS];
    terms[0] = term1;
    terms[1] = term2;
    terms[2] = term3;
    char shape[128];
    strcpy(shape, SA_CONCEPT_SHAPE_CURVE);
    char endShape[128];
    strcpy(endShape, SA_CONCEPT_SHAPE_PI);
    int count = 0;
    char notes[128] = "This is a note";
    char uom[128] = "size";
    double halfTerm = ((domainMax - domainMin) / (numTerms - 1));

    saContextTypePtr contextTypePtr = saContextCreateConcepts(name, domainMin, domainMax, avg, sdev, count, halfTerm, terms, numTerms, shape, endShape, notes, uom);

    double min = 0;
    double max = 26;
    saContextRecreateConcepts(contextTypePtr, min, max);

    EXPECT_STREQ("TESTD", contextTypePtr->name);
    EXPECT_EQ(0, contextTypePtr->domainMin);
    EXPECT_EQ(26, contextTypePtr->domainMax);
    EXPECT_EQ(0, contextTypePtr->count);
    EXPECT_EQ(3, contextTypePtr->numConcepts);
    EXPECT_STREQ("This is a note", contextTypePtr->notes);
    EXPECT_STREQ("size", contextTypePtr->uom);
}
