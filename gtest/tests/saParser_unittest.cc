// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <limits.h>
#include <stdio.h>
#include "../../saExpression.h"
#include "../../saToken.h"
#include "gtest/gtest.h"

#define MAXROWSIZE (1024*1024)
extern "C" {
int saParserParse(char *, char [], saExpressionTypePtrArray);
saExpressionTypePtrArray generateExpStack(void);
}

TEST(ParserTest, IS_Clause) {

    char whereLine[MAXROWSIZE / 32] = "Height is Tall";
    char tempbuf[MAXROWSIZE];
    saExpressionTypePtrArray expStack = generateExpStack();
    int stackSize = saParserParse(whereLine, tempbuf, expStack);
    EXPECT_EQ(stackSize, 3);
    EXPECT_EQ(expStack[0]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[0]->field, "Height");
    EXPECT_EQ(expStack[1]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[1]->field, "Tall");
    EXPECT_EQ(expStack[2]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[2]->field, "is");
}

TEST(ParserTest, IN_Clause) {

    char whereLine[MAXROWSIZE / 32] = "Height in MaleHeight is Tall";
    char tempbuf[MAXROWSIZE];
    saExpressionTypePtrArray expStack = generateExpStack();
    int stackSize = saParserParse(whereLine, tempbuf, expStack);

    EXPECT_EQ(stackSize, 5);
    EXPECT_EQ(expStack[0]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[0]->field, "Height");
    EXPECT_EQ(expStack[1]->type, SA_TOKEN_FUZZYTERMSET);
    EXPECT_STREQ(expStack[1]->field, "MaleHeight");
    EXPECT_EQ(expStack[2]->type, SA_TOKEN_IN);
    EXPECT_STREQ(expStack[2]->field, "in");
    EXPECT_EQ(expStack[3]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[3]->field, "Tall");
    EXPECT_EQ(expStack[4]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[4]->field, "is");
}

TEST(ParserTest, BY_Clause) {
    char whereLine[MAXROWSIZE / 32] = "Height by Gender is Tall";
    char tempbuf[MAXROWSIZE];
    saExpressionTypePtrArray expStack = generateExpStack();
    int stackSize = saParserParse(whereLine, tempbuf, expStack);

    EXPECT_EQ(stackSize, 5);
    EXPECT_EQ(expStack[0]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[0]->field, "Height");
    EXPECT_EQ(expStack[1]->type, SA_TOKEN_FUZZYTERMSET);
    EXPECT_STREQ(expStack[1]->field, "Gender");
    EXPECT_EQ(expStack[2]->type, SA_TOKEN_BY);
    EXPECT_STREQ(expStack[2]->field, "by");
    EXPECT_EQ(expStack[3]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[3]->field, "Tall");
    EXPECT_EQ(expStack[4]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[4]->field, "is");
}

TEST(ParserTest, IN_Clause_and_BY_Clause) {
    char whereLine[MAXROWSIZE / 32] = "Height in MaleHeight by Age is Tall";
    char tempbuf[MAXROWSIZE];
    saExpressionTypePtrArray expStack = generateExpStack();
    int stackSize = saParserParse(whereLine, tempbuf, expStack);

    EXPECT_EQ(stackSize, 7);
    EXPECT_EQ(expStack[0]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[0]->field, "Height");
    EXPECT_EQ(expStack[1]->type, SA_TOKEN_FUZZYTERMSET);
    EXPECT_STREQ(expStack[1]->field, "MaleHeight");
    EXPECT_EQ(expStack[2]->type, SA_TOKEN_FUZZYTERMSET);
    EXPECT_STREQ(expStack[2]->field, "Age");
    EXPECT_EQ(expStack[3]->type, SA_TOKEN_BY);
    EXPECT_STREQ(expStack[3]->field, "by");
    EXPECT_EQ(expStack[4]->type, SA_TOKEN_IN);
    EXPECT_STREQ(expStack[4]->field, "in");
    EXPECT_EQ(expStack[5]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[5]->field, "Tall");
    EXPECT_EQ(expStack[6]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[6]->field, "is");
}

