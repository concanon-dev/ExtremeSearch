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
#include "gtest/gtest.h"

extern "C" {
saContextTypePtr saContextInit(char *name, double domainMin, double domainMax, double avg, double sdev, int count, int numConcepts, char *type, char *notes, char *uom);
saContextTypePtr saContextLoad(FILE *infile);
saContextTypePtr saContextMerge(saContextTypePtr c1, saContextTypePtr c2, char *c3Name);
//char *eatTimestamp(char **cursor);
//char *getLine(FILE *infile);
//char *getAttribute(char **cursor, char separator);
//char *getValue(char **cursor);
//time_t getTimestamp(char *buf);
//char *trim(char *str);
bool saContextSave(FILE *outfile, saContextTypePtr contextPtr);
void saContextDisplayWithHeader(saContextTypePtr contextPtr, char *name);
void saContextDisplay(saContextTypePtr contextPtr);
void saContextLookup(saContextTypePtr contextPtr, double value, double *results);
}

TEST(saContext, saContextInit) {
}

TEST(saContext, saContextLoad) {

    char path[256] = "etc/apps/xtreme/contexts/TEST.context";
    FILE *f = fopen(path, "r");
    saContextTypePtr contextPtr = saContextLoad(f);
    fclose(f);

    EXPECT_STREQ("TEST", contextPtr->name);
    EXPECT_STREQ("v2.0", contextPtr->format);

}

TEST(saContext, saContextMerge) {
}

TEST(saContext, saContextSave) {
}

TEST(saContext, saContextDisplayWithHeader) {
}

TEST(saContext, saContextDisplay) {
}

TEST(saContext, saContextLookup) {
}

