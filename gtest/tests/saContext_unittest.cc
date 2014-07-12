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

saContextTypePtr saSplunkContextLoad(char *name, char *root, int *scope, char *app, char *user);
bool saSplunkContextSave(saContextTypePtr, char *, int, char *, char *);
}

TEST(saContext, saContextInit) {

    char name[128] = "TESTDOMAIN";
    double domainMin = 0;
    double domainMax = 100;
    double avg = 0;
    double sdev = 0;
    int count = 0;
    int numTerms = 3;
    char notes[128] = "This is a note";
    char uom[128] = "size";

    saContextTypePtr p = saContextInit(name, domainMin, domainMax, avg, sdev, count, numTerms, SA_CONTEXT_TYPE_DOMAIN, notes, uom);
    EXPECT_STREQ("TESTDOMAIN", p->name);
    EXPECT_EQ(100, p->domainMax);
    EXPECT_EQ(0, p->domainMin);
    EXPECT_EQ(0, p->avg);
    EXPECT_EQ(0, p->sdev);
    EXPECT_EQ(0, p->count);
    EXPECT_EQ(3, p->numConcepts);
    EXPECT_STREQ("This is a note", p->notes);
    EXPECT_STREQ("size", p->uom);

}

TEST(saContext, saContextLoad) {

    char path[256] = "etc/apps/xtreme/contexts/TEST.context";
    FILE *f = fopen(path, "r");
    saContextTypePtr contextPtr = saContextLoad(f);
    fclose(f);

    EXPECT_STREQ("TEST", contextPtr->name);
    EXPECT_STREQ("v2.0", contextPtr->format);

}

TEST(saContext, saContextSave) {
    char path[256] = "etc/apps/xtreme/contexts/TEST.context";
    char path2[256] = "etc/apps/xtreme/contexts/TEST2.context";
    FILE *f = fopen(path, "r");
    saContextTypePtr contextPtr = saContextLoad(f);
    fclose(f);

    EXPECT_STREQ("TEST", contextPtr->name);
    EXPECT_STREQ("v2.0", contextPtr->format);

    strcpy(contextPtr->name, "TEST2");

    FILE *f2 = fopen(path2, "w");
    saContextSave(f2, contextPtr);
    fclose(f2);

    FILE *f3 = fopen(path2, "r");
    saContextTypePtr contextPtr2 = saContextLoad(f3);
    fclose(f3);

    EXPECT_STREQ("TEST2", contextPtr2->name);
    EXPECT_STREQ("v2.0", contextPtr2->format);
}

TEST(saContext, saContextMerge) {


    char path[256] = "etc/apps/xtreme/contexts/TEST.context";
    char path2[256] = "etc/apps/xtreme/contexts/TEST2.context";
    char path3[256] = "etc/apps/xtreme/contexts/TESTM.context";
    char contextNameC[256] = "TESTM";

    FILE *f = fopen(path, "r");
    saContextTypePtr contextPtrA = saContextLoad(f);
    fclose(f);

    FILE *f2 = fopen(path2, "r");
    saContextTypePtr contextPtrB = saContextLoad(f2);
    fclose(f2);

    saContextTypePtr contextPtrC = saContextMerge(contextPtrA, contextPtrB, contextNameC);

    EXPECT_STREQ("TESTM", contextPtrC->name);

    FILE *f3 = fopen(path3, "w");
    saContextSave(f3, contextPtrC);
    fclose(f3);

    /* Alternate Test */
    char root[128] = "etc";
    char app[128] = "xtreme";
    char user[128] = "admin";
    int scopeA = SA_SPLUNK_SCOPE_GLOBAL;
    int scopeB = SA_SPLUNK_SCOPE_GLOBAL;
    int scopeC = SA_SPLUNK_SCOPE_GLOBAL;
    char contextNameA[256] = "TEST";
    char contextNameB[256] = "TEST2";
    char contextNameC2[256] = "TESTM2";


    contextPtrA = saSplunkContextLoad(contextNameA, root, &scopeA, app, user);
    if (contextPtrA == NULL)
    {
        fprintf(stderr, "xsMergeContext-F-107: Can't open context: %s\n", contextNameA);
        exit(EXIT_FAILURE);
    }
    contextPtrB= saSplunkContextLoad(contextNameA, root, &scopeB, app, user);
    if (contextPtrB == NULL)
    {
        fprintf(stderr, "xsMergeContext-F-109: Can't open context: %s\n", contextNameB);
        exit(EXIT_FAILURE);
    }

    contextPtrC = saContextMerge(contextPtrA, contextPtrB, contextNameC2);
    EXPECT_STREQ("TESTM2", contextPtrC->name);

    bool result = saSplunkContextSave(contextPtrC, root, scopeC, app, user);
    EXPECT_EQ(true, result);
}

TEST(saContext, saContextDisplayWithHeader) {

    char contextName[256] = "TEST";
    char root[128] = "etc";
    char app[128] = "xtreme";
    char user[128] = "admin";
    int scope = SA_SPLUNK_SCOPE_GLOBAL;
    saContextTypePtr contextPtr = saSplunkContextLoad(contextName, root, &scope, app, user);

    // TODO-
    // This prints Context to stdout
    //freopen("log.txt", "w", stdout);
    //saContextDisplayWithHeader(contextPtr, contextPtr->name);
}

TEST(saContext, saContextDisplay) {
  // saContextDisplay just calls saContextDisplayWithHeader passing in contextPtr->name
}

TEST(saContext, saContextLookup) {
    char contextName[256] = "TEST";
    char root[128] = "etc";
    char app[128] = "xtreme";
    char user[128] = "admin";
    int scope = SA_SPLUNK_SCOPE_GLOBAL;
    double value = 1;
    double results[64];

    saContextTypePtr contextPtr = saSplunkContextLoad(contextName, root, &scope, app, user);
    saContextLookup(contextPtr, value, results);
    EXPECT_LT(0, results[0]);
}

