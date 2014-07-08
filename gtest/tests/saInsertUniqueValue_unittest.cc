// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "gtest/gtest.h"

extern "C" {
char *insertUniqueValue(char *values[], char *field, int *numUniqueValues);
}

TEST(saInsertUniqueValue, InsertUniqueValue) {

    char *values[255];
    int numUniqueValues = 10;
    int i;
    for (i=0; i < numUniqueValues; i++)
    {
        char str[255];
        sprintf(str, "TEST%d", i);

        values[i] = (char *)malloc(strlen(str));
        strcpy(values[i], str);
    }

    char field[64] = "TEST1";
    char *result = insertUniqueValue(values, field, &numUniqueValues);
    EXPECT_STREQ("TEST1", result);

    char uniqueField[64] = "TEST10";
    result = insertUniqueValue(values, uniqueField, &numUniqueValues);
    EXPECT_STREQ("TEST10", result);
}
