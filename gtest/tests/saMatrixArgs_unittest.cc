// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../saConstants.h"
#include "gtest/gtest.h"

extern "C" {
void saMatrixArgs(char *x, char *y, int size);
}

static char x[SA_CONSTANTS_MAXSTRING] = "a,b,c";
static char y[SA_CONSTANTS_MAXSTRING];
static int size = SA_CONSTANTS_MAXSTRING;

TEST(saMatrixArgs, saMatrixArgs) {

    saMatrixArgs(x, y, size);

    EXPECT_STREQ("a,b,c,a,b,c,a,b,c", x);
    EXPECT_STREQ("a,b,c,b,c,a,c,a,b", y);
}
