// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <iostream>
#include <stdbool.h>
#include "../../saHedge.h"
#include "../../saConcept.h"
#include "gtest/gtest.h"

extern "C" {
saConceptTypePtr saHedgeApply(int hedge, saConceptTypePtr concept);
bool saHedgeLoadLookup(FILE *infile, saSynonymTableTypePtr synonymPtr);
char *saHedgeLookup(saSynonymTableTypePtr synonymPtr, char *newWord);
}

TEST(saContext, saHedgeLoadLookup) {
    char path[1024] = "etc/apps/xtreme/lookups/synonyms.csv";
    saSynonymTableType synonyms;
    FILE *f = fopen(path, "r");
    bool worked = saHedgeLoadLookup(f, &synonyms);

    EXPECT_EQ(true, worked);
}

TEST(saContext, saHedgeLookup) {
    char path[1024] = "etc/apps/xtreme/lookups/synonyms.csv";
    char word1[128] = "of";
    char word2[128] = "around";
    char word3[128] = "beyond";
    char word4[128] = "down";
    char word5[128] = "approaching";
    char word6[128] = "exceedingly";
    char word7[128] = "mostly";
    char word8[128] = "partially";
    saSynonymTableType synonyms;
    FILE *f = fopen(path, "r");
    bool worked = saHedgeLoadLookup(f, &synonyms);
    EXPECT_EQ(true, worked);

    char *hedgeWord = saHedgeLookup(&synonyms, word1);
    EXPECT_STREQ("_noise", hedgeWord);
    hedgeWord = saHedgeLookup(&synonyms, word2);
    EXPECT_STREQ("about", hedgeWord);
    hedgeWord = saHedgeLookup(&synonyms, word3);
    EXPECT_STREQ("above", hedgeWord);
    hedgeWord = saHedgeLookup(&synonyms, word4);
    EXPECT_STREQ("below", hedgeWord);
    hedgeWord = saHedgeLookup(&synonyms, word5);
    EXPECT_STREQ("close", hedgeWord);
    hedgeWord = saHedgeLookup(&synonyms, word6);
    EXPECT_STREQ("extremely", hedgeWord);
    hedgeWord = saHedgeLookup(&synonyms, word7);
    EXPECT_STREQ("generally", hedgeWord);
    hedgeWord = saHedgeLookup(&synonyms, word8);
    EXPECT_STREQ("somewhat", hedgeWord);
}

TEST(saContext, saHedgeApply) {
}

