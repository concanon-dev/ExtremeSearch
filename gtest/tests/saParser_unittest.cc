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
int loadExpStack(FILE *f, saExpressionTypePtrArray expStack);
}

TEST(saParserParse, loadExpStack) {

    saExpressionTypePtrArray expStack = generateExpStack();
    int stackSize = 0;
    char stackFile[2048];
    char infoPath[128] = "etc\\";
    static char tempbuf[MAXROWSIZE];
    char whereLine[MAXROWSIZE / 32] = "Height is Tall";
    strcpy(stackFile, infoPath);

    char *endStr = stackFile + strlen(stackFile) - 1;
    while(*endStr != 47 && *endStr != 92)
          endStr--;
    strcpy(endStr+1, "stack.txt");
    FILE *sFile = fopen(stackFile, "r");
    if (sFile == NULL)
    { 
        stackSize = saParserParse(whereLine, tempbuf, expStack);
        if (stackSize < 0) {
            fprintf(stderr, "xsWhere-F-105: grammer: %s\n", tempbuf);
            exit(EXIT_FAILURE);
        }
        FILE *abcd = fopen(stackFile, "w");
        //writeExpressionStack(abcd, expStack, stackSize);
        fclose(abcd);
    }
    else
    {
        stackSize = loadExpStack(sFile,  expStack);
        fclose(sFile);
    }
}

TEST(saParserParse, IS_Clause) {

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

TEST(saParserParse, IS_Clause_With_Hedge) {

    char whereLine[MAXROWSIZE / 32] = "Height is very Tall";
    char tempbuf[MAXROWSIZE];
    saExpressionTypePtrArray expStack = generateExpStack();
    int stackSize = saParserParse(whereLine, tempbuf, expStack);
    EXPECT_EQ(stackSize, 4);
    EXPECT_EQ(expStack[0]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[0]->field, "Height");
    EXPECT_EQ(expStack[1]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[1]->field, "Tall");
    EXPECT_EQ(expStack[2]->type, SA_TOKEN_MODIFIER);
    EXPECT_STREQ(expStack[2]->field, "very");
    EXPECT_EQ(expStack[3]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[3]->field, "is");
}

TEST(saParserParse, IN_Clause) {

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

TEST(saParserParse, AND_Clause) {

    char whereLine[MAXROWSIZE / 32] = "(Height in MaleHeight is not tiny) and (Height in MaleHeight is not short)";
    char tempbuf[MAXROWSIZE];
    saExpressionTypePtrArray expStack = generateExpStack();
    int stackSize = saParserParse(whereLine, tempbuf, expStack);

    EXPECT_EQ(stackSize, 13);
    EXPECT_EQ(expStack[0]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[0]->field, "Height");
    EXPECT_EQ(expStack[1]->type, SA_TOKEN_FUZZYTERMSET);
    EXPECT_STREQ(expStack[1]->field, "MaleHeight");
    EXPECT_EQ(expStack[2]->type, SA_TOKEN_IN);
    EXPECT_STREQ(expStack[2]->field, "in");
    EXPECT_EQ(expStack[3]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[3]->field, "tiny");
    EXPECT_EQ(expStack[4]->type, SA_TOKEN_MODIFIER);
    EXPECT_STREQ(expStack[4]->field, "not");
    EXPECT_EQ(expStack[5]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[5]->field, "is");
    EXPECT_EQ(expStack[6]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[6]->field, "Height");
    EXPECT_EQ(expStack[7]->type, SA_TOKEN_FUZZYTERMSET);
    EXPECT_STREQ(expStack[7]->field, "MaleHeight");
    EXPECT_EQ(expStack[8]->type, SA_TOKEN_IN);
    EXPECT_STREQ(expStack[8]->field, "in");
    EXPECT_EQ(expStack[9]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[9]->field, "short");
    EXPECT_EQ(expStack[10]->type, SA_TOKEN_MODIFIER);
    EXPECT_STREQ(expStack[10]->field, "not");
    EXPECT_EQ(expStack[11]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[11]->field, "is");
    EXPECT_EQ(expStack[12]->type, SA_TOKEN_AND);
    EXPECT_STREQ(expStack[12]->field, "and");
}

TEST(saParserParse, AND_AND_Clause) {

    char whereLine[MAXROWSIZE / 32] = "Height is tiny and Height is short and Height is medium";
    char tempbuf[MAXROWSIZE];
    saExpressionTypePtrArray expStack = generateExpStack();
    int stackSize = saParserParse(whereLine, tempbuf, expStack);

    EXPECT_EQ(stackSize, 11);
    EXPECT_EQ(expStack[0]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[0]->field, "Height");
    EXPECT_EQ(expStack[1]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[1]->field, "tiny");
    EXPECT_EQ(expStack[2]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[2]->field, "is");
    EXPECT_EQ(expStack[3]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[3]->field, "Height");
    EXPECT_EQ(expStack[4]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[4]->field, "short");
    EXPECT_EQ(expStack[5]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[5]->field, "is");
    EXPECT_EQ(expStack[6]->type, SA_TOKEN_AND);
    EXPECT_STREQ(expStack[6]->field, "and");
    EXPECT_EQ(expStack[7]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[7]->field, "Height");
    EXPECT_EQ(expStack[8]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[8]->field, "medium");
    EXPECT_EQ(expStack[9]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[9]->field, "is");
    EXPECT_EQ(expStack[10]->type, SA_TOKEN_AND);
    EXPECT_STREQ(expStack[10]->field, "and");
}

TEST(saParserParse, OR_Clause) {

    char whereLine[MAXROWSIZE / 32] = "(Height in MaleHeight is tiny) or (Height in MaleHeight is short)";
    char tempbuf[MAXROWSIZE];
    saExpressionTypePtrArray expStack = generateExpStack();
    int stackSize = saParserParse(whereLine, tempbuf, expStack);

    EXPECT_EQ(stackSize, 11);
    EXPECT_EQ(expStack[0]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[0]->field, "Height");
    EXPECT_EQ(expStack[1]->type, SA_TOKEN_FUZZYTERMSET);
    EXPECT_STREQ(expStack[1]->field, "MaleHeight");
    EXPECT_EQ(expStack[2]->type, SA_TOKEN_IN);
    EXPECT_STREQ(expStack[2]->field, "in");
    EXPECT_EQ(expStack[3]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[3]->field, "tiny");
    EXPECT_EQ(expStack[4]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[4]->field, "is");
    EXPECT_EQ(expStack[5]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[5]->field, "Height");
    EXPECT_EQ(expStack[6]->type, SA_TOKEN_FUZZYTERMSET);
    EXPECT_STREQ(expStack[6]->field, "MaleHeight");
    EXPECT_EQ(expStack[7]->type, SA_TOKEN_IN);
    EXPECT_STREQ(expStack[7]->field, "in");
    EXPECT_EQ(expStack[8]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[8]->field, "short");
    EXPECT_EQ(expStack[9]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[9]->field, "is");
    EXPECT_EQ(expStack[10]->type, SA_TOKEN_OR);
    EXPECT_STREQ(expStack[10]->field, "or");
}

TEST(saParserParse, Is_Numeric) {
    char whereLine[MAXROWSIZE / 32] = "Height is 65";
    char tempbuf[MAXROWSIZE];
    saExpressionTypePtrArray expStack = generateExpStack();
    int stackSize = saParserParse(whereLine, tempbuf, expStack);
    EXPECT_EQ(stackSize, 3);
    EXPECT_EQ(expStack[0]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[0]->field, "Height");
    EXPECT_EQ(expStack[1]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[1]->field, "65");
    EXPECT_EQ(expStack[2]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[2]->field, "is");
}

TEST(saParserParse, Is_Hedge_Numeric) {
    char whereLine[MAXROWSIZE / 32] = "Height is very 65";
    char tempbuf[MAXROWSIZE];
    saExpressionTypePtrArray expStack = generateExpStack();
    int stackSize = saParserParse(whereLine, tempbuf, expStack);
    EXPECT_EQ(stackSize, 4);
    EXPECT_EQ(expStack[0]->type, SA_TOKEN_FIELD);
    EXPECT_STREQ(expStack[0]->field, "Height");
    EXPECT_EQ(expStack[1]->type, SA_TOKEN_FUZZYSET);
    EXPECT_STREQ(expStack[1]->field, "65");
    EXPECT_EQ(expStack[2]->type, SA_TOKEN_MODIFIER);
    EXPECT_STREQ(expStack[2]->field, "very");
    EXPECT_EQ(expStack[3]->type, SA_TOKEN_IS);
    EXPECT_STREQ(expStack[3]->field, "is");
}

/*
TEST(saParserParse, BY_Clause) {
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
*/
/*
TEST(saParserParse, IN_Clause_and_BY_Clause) {
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
*/
