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
#include "../../saCSV.h"
#include "../../saHedge.h"
#include "../../saSplunk.h"
#include "gtest/gtest.h"

extern "C" {
bool saSplunkGetContextPath(char *path, char *root, int scope, char *app, char *user);
saContextTypePtr saSplunkContextFind(char *name, char *root, char *app, char *user, int *scope);
saContextTypePtr saSplunkContextLoad(char *name, char *root, int *scope, char *app, char *user);
bool saSplunkContextSave(saContextTypePtr contextPtr, char *root, int scope, char *app, char *user);
bool saSplunkContextDelete(char *name, char *root, int scope, char *app, char *user);
bool saSplunkContextRename(char *name, char *root, int scope, char *app, char *user, char *newName, char *newRoot, int newScope, char *newApp, char *newUser);
char *saSplunkGetRoot(char *root);
int saSplunkGetScope(char *scopeStr);
bool saSplunkHedgeLoad(char *name, char *root, char *app, char *user, int *scope, saSynonymTableTypePtr synonyms);
saSplunkInfoPtr saSplunkLoadInfo(char *infoPath);
}

TEST(saSplunk, saSplunkGetContextPath) {
    char root[128] = "etc";
    char app[128] = "xtreme";
    char user[128] = "admin";
    //int scope = SA_SPLUNK_SCOPE_APP;
    int scope = SA_SPLUNK_SCOPE_GLOBAL;
    char path[1024];

    bool result = saSplunkGetContextPath(path, root, scope, app, user);

    EXPECT_EQ(true, result);
    EXPECT_STREQ("etc/apps/xtreme/contexts", path);

    scope = SA_SPLUNK_SCOPE_APP;
    result = saSplunkGetContextPath(path, root, scope, app, user);
    EXPECT_EQ(true, result);
    EXPECT_STREQ("etc/apps/xtreme/contexts", path);

    scope = SA_SPLUNK_SCOPE_PRIVATE;
    result = saSplunkGetContextPath(path, root, scope, app, user);
    EXPECT_EQ(true, result);
    EXPECT_STREQ("etc/users/admin/xtreme/contexts", path);
}

TEST(saSplunk, saSplunkContextFind) {
    char contextName[128] = "TEST";
    char root[128] = "etc";
    char app[128] = "xtreme";
    char user[128] = "admin";
    int scope = SA_SPLUNK_SCOPE_NONE;

    saContextTypePtr contextPtr = saSplunkContextFind(contextName, root, app, user, &scope);
    EXPECT_STREQ("TEST", contextPtr->name);
}

TEST(saSplunk, saSplunkContextSave) {

    char contextName[128] = "TEST";
    char root[128] = "etc";
    char app[128] = "xtreme";
    char user[128] = "admin";
    char newUser[128] = "test";
    int scope = SA_SPLUNK_SCOPE_GLOBAL;

    saContextTypePtr contextPtr = saSplunkContextLoad(contextName, root, &scope, app, user);
    EXPECT_STREQ("TEST", contextPtr->name);


    scope = SA_SPLUNK_SCOPE_PRIVATE;
    bool result = saSplunkContextSave(contextPtr, root, scope, app, newUser);
    EXPECT_EQ(true, result);


    contextPtr = saSplunkContextLoad(contextName, root, &scope, app, newUser);
    EXPECT_STREQ("TEST", contextPtr->name);

}

TEST(saSplunk, saSplunkContextRename) {
    char name[128] = "TEST";
    char newName[128] = "TESTCOPY";
    char root[128] = "etc";
    char newRoot[128] = "etc";
    char app[128] = "xtreme";
    char newApp[128] = "xtreme";
    char user[128] = "test";
    char newUser[128] = "test";
    int scope = SA_SPLUNK_SCOPE_PRIVATE;
    int newScope = SA_SPLUNK_SCOPE_PRIVATE;


    bool result = saSplunkContextRename(name, root, scope, app, user,
                                        newName, newRoot, newScope, newApp, newUser);
    EXPECT_EQ(true, result);
}
TEST(saSplunk, saSplunkContextDelete) {
    char name[128] = "TESTCOPY";
    char root[128] = "etc";
    char app[128] = "xtreme";
    char user[128] = "test";
    int scope = SA_SPLUNK_SCOPE_PRIVATE;

    bool result = saSplunkContextDelete(name, root, scope, app, user);
    EXPECT_EQ(true, result);
}

TEST(saSplunk, saSplunkGetRoot) {
 
    char root[128] = "etc";
    char *newRoot = saSplunkGetRoot(root);
    //printf("newRoot=%s",newRoot);
    EXPECT_STREQ(".", newRoot);
}

TEST(saSplunk, saSplunkGetScope) {
    char globalScope[128] = "global";
    char appScope[128] = "app";
    char privateScope[128] = "private";
    char noScope[128] = "";

    int scopeInt = saSplunkGetScope(globalScope);
    EXPECT_EQ(3, scopeInt);
    scopeInt = saSplunkGetScope(appScope);
    EXPECT_EQ(2, scopeInt);
    scopeInt = saSplunkGetScope(privateScope);
    EXPECT_EQ(1, scopeInt);
    scopeInt = saSplunkGetScope(noScope);
    EXPECT_EQ(0, scopeInt);
}

TEST(saSplunk, saSplunkLoadHeader) {
}

TEST(saSplunk, saSplunkReadInfoPathFile) {
    // Essentially tested with test below - saSplunkLoadInnfo
}

TEST(saSplunk, saSplunkLoadInfo) {
    char infoFile[128] = "infoPath.csv";
    saSplunkInfoPtr iPtr = saSplunkLoadInfo(infoFile);
    EXPECT_STREQ("admin", iPtr->user);
    EXPECT_STREQ("test", iPtr->app);
}

TEST(saSplunk, saSplunkHedgeLoad) {
    char name[128] = "synonyms";
    char root[128] = "etc";
    char app[128] = "xtreme";
    char user[128] = "test";
    int scope = SA_SPLUNK_SCOPE_PRIVATE;

    static saSynonymTableType synonyms;
    bool result = saSplunkHedgeLoad(name, root, app, user, &scope, &synonyms);
    EXPECT_EQ(true, result);
}

TEST(saSplunk, saSplunkContextLoad) {

    char contextName[128] = "TEST";
    char root[128] = "etc";
    char app[128] = "xtreme";
    char user[128] = "admin";
    int scope = SA_SPLUNK_SCOPE_GLOBAL;

    saContextTypePtr contextPtr = saSplunkContextLoad(contextName, root, &scope, app, user);
    EXPECT_STREQ("TEST", contextPtr->name);

    scope = SA_SPLUNK_SCOPE_APP;
    contextPtr = saSplunkContextLoad(contextName, root, &scope, app, user);
    EXPECT_STREQ("TEST", contextPtr->name);

    scope = SA_SPLUNK_SCOPE_PRIVATE;
    bool result = saSplunkContextSave(contextPtr, root, scope, app, user);
    EXPECT_EQ(true, result);
    contextPtr = saSplunkContextLoad(contextName, root, &scope, app, user);
    EXPECT_STREQ("TEST", contextPtr->name);
}
