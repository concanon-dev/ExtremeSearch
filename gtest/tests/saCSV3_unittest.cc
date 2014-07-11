// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

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
    char headerbuf[2048];
    char *headerList[32];
    char inbuf[2048];
    char *fieldList[32];
    char fileName[256];
    sprintf(fileName, "etc/apps/xtreme/lookups/locations.csv");

    FILE *f = fopen(fileName, "r");
    if (f == NULL) {
        printf("Unable to open file: %s\n", fileName);
        return;
    }

    saCSVOpen(&csv, f);

    int numHeaderFields = saCSV3GetLine(&csv, headerbuf, headerList);
    EXPECT_EQ(8,numHeaderFields);
    EXPECT_STREQ("LocId",headerList[0]);
    EXPECT_STREQ("CountryCode",headerList[1]);
    EXPECT_STREQ("Country",headerList[2]);
    EXPECT_STREQ("Region",headerList[3]);
    EXPECT_STREQ("City",headerList[4]);
    EXPECT_STREQ("PostalCode",headerList[5]);
    EXPECT_STREQ("Latitude",headerList[6]);
    EXPECT_STREQ("Longitude",headerList[7]);

    bool done = false;
    while(done == false)
    {
        int numFields = saCSV3GetLine(&csv, inbuf, fieldList);

        if (numFields != 0)
        {
            int i;
            /*
            for(i=0; i<numFields; i++)
            {
                if (i == 0)
                    fputs(fieldList[0], stdout);
                else
                    fprintf(stdout, ",%s", fieldList[i]);
            }
            fputs("\n", stdout);
	    */
        }
        else
            done = true;
    }
}
