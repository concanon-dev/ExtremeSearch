// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <iostream>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../saCSV3.h"
#include "../../saCSV.h"
#include "../../saConstants.h"
#include "gtest/gtest.h"

extern "C" {
}

TEST(saCSV3, saCSVOpen_saCSV3GetLine) {
    saCSVType csv;
    char headerbuf[SA_CONSTANTS_MAXROWSIZE];
    char inbuf[SA_CONSTANTS_MAXROWSIZE];
    char *headerList[SA_CONSTANTS_MAXROWSIZE/31];
    char *fields[SA_CONSTANTS_MAXROWSIZE/32];
    char fileName[256];
    sprintf(fileName, "etc/apps/xtreme/lookups/locations.csv");

    FILE *f = fopen(fileName, "r");
    if (f == NULL) {
        printf("Unable to open file: %s\n", fileName);
        return;
    }

    saCSVOpen(&csv, f);

    //int numHeaderFields = saCSV3GetLine(&csv, headerbuf, headerList);
    int numHeaderFields = saCSVFGetHeaderLine(f, headerbuf, headerList);
    EXPECT_EQ(20,numHeaderFields);

    printf("HERE");
    exit(0);
    bool done = false;
    while(done == false)
    {
        int numFields = saCSV3GetLine(&csv, inbuf, fields);
        if (numFields != 0)
        {
            int i;
            for(i=0; i<numFields; i++)
            {
                if (i == 0)
                    fputs(fields[0], stdout);
                else
                    fprintf(stdout, ",%s", fields[i]);
            }
            fputs("\n", stdout);
        }
        else
            done = true;
    }
}
