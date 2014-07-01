/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: saParser

Description:
     Perform all parsing functions associated with a conceptual query string.  In general, a query string
     will be passed in and an Expression Stack will be returned.  This stack is then walked to apply the
     query against data.  The order of these calls is:

     generateExpStack() to create a new stack
     saParserParse() to parse a string into an expression stack

     If you wish to view the stack, a utility function walkExpressionStack() is provided

     A main method has been provided for testing purposes.  Use -D_MAIN_ to have this function included.

Functions:
     generateExpStack
     saParserParse
     walkExpressionStack
*/
#include <ctype.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "saExpression.h"
#include "saToken.h"

#define MAXCHARS 256

/* Generate Expressions, given a specific token */
// Infix Tokens
inline saExpressionTypePtr andExpression(saExpressionTypePtr, saTokenTypePtr);
inline saExpressionTypePtr inExpression(saExpressionTypePtr, saTokenTypePtr);
inline saExpressionTypePtr byExpression(saExpressionTypePtr, saTokenTypePtr);
inline saExpressionTypePtr isExpression(saExpressionTypePtr, saTokenTypePtr);
inline saExpressionTypePtr orExpression(saExpressionTypePtr, saTokenTypePtr);

// Name Tokens
inline saExpressionTypePtr fieldExpression(saTokenTypePtr); 
inline saExpressionTypePtr conceptExpression(saTokenTypePtr);
inline saExpressionTypePtr contextExpression(saTokenTypePtr);
inline saExpressionTypePtr parenExpression(saTokenTypePtr);

// Prefix (unary) Tokens
inline saExpressionTypePtr modifierOperatorExpression(saTokenTypePtr);
inline saExpressionTypePtr notOperatorExpression(saTokenTypePtr);

// Token Management functions
inline saTokenTypePtr getNextToken(int);
inline saTokenTypePtr lookAtNextToken();
bool reclassify();
void tokenizer(char *, saTokenTypePtr *, int *, char *);

// Expression Management functions
int buildExpressionStack(saExpressionTypePtr, saExpressionTypePtrArray, int);
inline saExpressionTypePtrArray generateExpStack(void);
int saParserParse(char *, char [], saExpressionTypePtrArray);
inline saExpressionTypePtr parseExpression(int);
void walkExpressionStack(FILE *, saExpressionTypePtrArray, int);

// Global Token Variables
static saTokenTypePtr global_tokenStack[MAXCHARS];
static int global_tokenIndex = 0;
static int global_tokenCount = 0;

// Input Arguments
extern char *optarg;
extern int optind, optopt;

#ifdef _MAIN_
int main(int argc, char* argv[])
{
    char errMsg[MAXCHARS];
    char where_line[MAXCHARS];
    int c;
    bool argError;

    initSignalHandler(basename(argv[0]));
    argError = true;
    while ((c = getopt(argc, argv, "w:")) != -1)
    {
        switch(c)
        {
            case 'w':
                strcpy(where_line, optarg);
                argError = false;
                break;
            case '?':
                fprintf(stderr, "Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "usage: xsParser -w \"where line\"\n");
        exit(EXIT_FAILURE);
    }
    int expCount;
    saExpressionTypePtrArray expStack = generateExpStack();
    walkExpressionStack(stderr, expStack, parse(where_line, errMsg, expStack));
}
#endif

int saParserParse(char *where_line, char errMsg[], saExpressionTypePtrArray expStack)
{
    int i;

    // Reset Token Count/Index - Unit Tests are re-entrant
    global_tokenCount = 0;
    global_tokenIndex = 0;

    // parse where line into tokens, validate grammer
    tokenizer(where_line, global_tokenStack, &global_tokenCount, errMsg);
    if (global_tokenCount == -1)
    {
        return(-1);
    }

    // Build the expression tree
    return(buildExpressionStack(parseExpression(0), expStack, 0));
}


inline saExpressionTypePtrArray generateExpStack()
{
    return(malloc(sizeof(saExpressionTypePtr) * MAXCHARS));
}

inline saTokenTypePtr getNextToken(int token_type)
{
    // Check to see if there is a specific token expected next
    if (token_type != SA_TOKEN_NO_TOKEN)
    {
        saTokenTypePtr token = lookAtNextToken();
        if (token->token_type != token_type)
        {
            fprintf(stderr, "unexpected token, found %d '%s', expected %d\n", 
                    token->token_type, token->token, token_type);
            return(NULL);
        }
    }
    return(global_tokenStack[global_tokenIndex++]);
}

int loadExpStack(FILE *f, saExpressionTypePtrArray expStack)
{
    char inBuf[257];

    fgets(inBuf, 256, f);
    inBuf[strlen(inBuf)-1] = '\0';
    int stackIndex = atoi(inBuf);
    int i;
    for(i=0; i<stackIndex; i++)
    {
        if (fgets(inBuf, 256, f) != NULL)
        {
            inBuf[strlen(inBuf)-1] = '\0';
            expStack[i] = malloc(sizeof(saExpressionType));
            char *s = inBuf;
            char *t = strchr(s, ',');
            *t = '\0';
            t++; 
            char *u = strchr(t, ',');
            *u = '\0';
            u++;
            expStack[i]->field = malloc(strlen(s)+1);
            strcpy(expStack[i]->field, s);
            expStack[i]->type = atoi(t);
            expStack[i]->intvalue = atoi(u);
        }
    }
    return(stackIndex);
}

inline saTokenTypePtr lookAtNextToken()
{
    return(global_tokenStack[global_tokenIndex]);
}


inline saExpressionTypePtr parseExpression(precedence)
{
    saExpressionTypePtr left = NULL;

    // Generate expressions for unary/name tokens
    saTokenTypePtr currToken = getNextToken(SA_TOKEN_NO_TOKEN);
    if (currToken->token_type == SA_TOKEN_LEFT_PAREN)
        left = parenExpression(currToken);
    else if (currToken->token_type == SA_TOKEN_FIELD)
        left = fieldExpression(currToken); // nameExpression
    else if (currToken->token_type == SA_TOKEN_FUZZYSET)
        left = conceptExpression(currToken); // nameExpression
    else if (currToken->token_type == SA_TOKEN_FUZZYTERMSET)
        left = contextExpression(currToken); // nameExpression
    else if (currToken->token_type == SA_TOKEN_NOT)
        left = notOperatorExpression(currToken); // PrefixOperatorExpression
    else if (currToken->token_type == SA_TOKEN_MODIFIER)
        left = modifierOperatorExpression(currToken); // PrefixOpertatorExpression
    else 
    {
        fprintf(stderr, "parseException: Bad token: %s\n", currToken->token);
        fprintf(stderr, "parseException: Bad token: %d\n", currToken->token_type);
        return(NULL);
    }

    // Generate expressions for operator tokens
    while(precedence < lookAtNextToken()->token_precedence)
    {
        currToken = getNextToken(SA_TOKEN_NO_TOKEN);

        if (currToken->token_type == SA_TOKEN_IN)
            left = inExpression(left, currToken);  // binaryOperator
        else if (currToken->token_type == SA_TOKEN_BY)
            left = byExpression(left, currToken);  // binaryOperator
        else if (currToken->token_type == SA_TOKEN_IS)
            left = isExpression(left, currToken);  // binaryOperator
        else if (currToken->token_type == SA_TOKEN_AND)
            left = andExpression(left, currToken); // binaryOperator
        else if (currToken->token_type == SA_TOKEN_OR)
            left = orExpression(left, currToken); // binaryOperator
    }

    return(left);
}


int buildExpressionStack(saExpressionTypePtr expTree, saExpressionTypePtrArray expStack, int counter)
{
    if (expTree->left != NULL)
        counter = buildExpressionStack(expTree->left, expStack, counter);

    if (expTree->right != NULL)
        counter = buildExpressionStack(expTree->right, expStack, counter);

    expStack[counter++] = expTree;

    return(counter);
}

void walkExpressionStack(FILE *f, saExpressionTypePtrArray expStack, int stackIndex)
{
    fprintf(f, "count=%d\n", stackIndex);
    int i;
    for(i=0; i<stackIndex; i++)
        fprintf(f, "stack[%d]=%s type=%d\n", i, expStack[i]->field, expStack[i]->type);

    return;
}

void writeExpressionStack(FILE *f, saExpressionTypePtrArray expStack, int stackIndex)
{
    fprintf(f, "%d\n", stackIndex);
    int i;
    for(i=0; i<stackIndex; i++)
        fprintf(f, "%s,%d,%d\n", expStack[i]->field, expStack[i]->type, expStack[i]->intvalue);

    return;
}

inline saExpressionTypePtr andExpression(saExpressionTypePtr left, saTokenTypePtr currToken) 
{
    saExpressionTypePtr right = parseExpression(SA_PRECEDENCE_AND);
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->left = left;
    exp->right = right;
    exp->type = SA_TOKEN_AND;
    exp->field = "and";
    return(exp);
}

inline saExpressionTypePtr orExpression(saExpressionTypePtr left, saTokenTypePtr currToken) 
{
    saExpressionTypePtr right = parseExpression(SA_PRECEDENCE_OR);
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->left = left;
    exp->right = right;
    exp->type = SA_TOKEN_OR;
    exp->field = "or";
    return(exp);
}


inline saExpressionTypePtr inExpression(saExpressionTypePtr left, saTokenTypePtr currToken)
{
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->left = left;
    saExpressionTypePtr right = parseExpression(SA_PRECEDENCE_IN);
    exp->right = right;
    exp->type = SA_TOKEN_IN;
    exp->field = "in";
    return(exp);
}

inline saExpressionTypePtr byExpression(saExpressionTypePtr left, saTokenTypePtr currToken)
{
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->left = left;
    saExpressionTypePtr right = parseExpression(SA_PRECEDENCE_BY);
    exp->right = right;
    exp->type = SA_TOKEN_BY;
    exp->field = "by";
    return(exp);
}

inline saExpressionTypePtr isExpression(saExpressionTypePtr left, saTokenTypePtr currToken)
{
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->left = left;
    saExpressionTypePtr right = parseExpression(SA_PRECEDENCE_IS);
    exp->right = right;
    exp->type = SA_TOKEN_IS;
    exp->field = "is";
    return(exp);
}

inline saExpressionTypePtr parenExpression(saTokenTypePtr currToken)
{
    saExpressionTypePtr exp = parseExpression(0);
    getNextToken(SA_TOKEN_RIGHT_PAREN);
    return(exp);
}

inline saExpressionTypePtr fieldExpression(saTokenTypePtr currToken) 
{
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->field = currToken->token;
    exp->type = currToken->token_type;
    exp->left = NULL;
    exp->right = NULL;
    return(exp);
}

inline saExpressionTypePtr conceptExpression(saTokenTypePtr currToken) 
{
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->field = currToken->token;
    exp->type = currToken->token_type;
    exp->left = NULL;
    exp->right = NULL;
    exp->intvalue = -1;
    exp->valueptr = 0;
    return(exp);
}

inline saExpressionTypePtr contextExpression(saTokenTypePtr currToken)
{
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->field = currToken->token;
    exp->type = currToken->token_type;
    exp->left = NULL;
    exp->right = NULL;
    exp->intvalue = -1;
    exp->valueptr = 0;
    return(exp);
}

inline saExpressionTypePtr modifierOperatorExpression(saTokenTypePtr currToken) 
{
    saExpressionTypePtr right = parseExpression(currToken->token_precedence);
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->field = currToken->token;
    exp->type = currToken->token_type;
    exp->left = NULL;
    exp->right = right;
    return(exp);
}

inline saExpressionTypePtr notOperatorExpression(saTokenTypePtr currToken) 
{
    saExpressionTypePtr right = parseExpression(currToken->token_precedence);
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->field = currToken->token;
    //exp->type = currToken->token_type;
    exp->type = SA_TOKEN_MODIFIER;
    exp->left = NULL;
    exp->right = right;
    return(exp);
}

