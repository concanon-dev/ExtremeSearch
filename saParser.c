/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
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
saExpressionTypePtr andExpression(saExpressionTypePtr, saTokenTypePtr);
saExpressionTypePtr inExpression(saExpressionTypePtr, saTokenTypePtr);
saExpressionTypePtr byExpression(saExpressionTypePtr, saTokenTypePtr);
saExpressionTypePtr isExpression(saExpressionTypePtr, saTokenTypePtr);
saExpressionTypePtr orExpression(saExpressionTypePtr, saTokenTypePtr);

// Name Tokens
saExpressionTypePtr fieldExpression(saTokenTypePtr); 
saExpressionTypePtr conceptExpression(saTokenTypePtr);
saExpressionTypePtr contextExpression(saTokenTypePtr);
saExpressionTypePtr parenExpression(saTokenTypePtr);

// Prefix (unary) Tokens
saExpressionTypePtr modifierOperatorExpression(saTokenTypePtr);
saExpressionTypePtr notOperatorExpression(saTokenTypePtr);

// Token Management functions
saTokenTypePtr getNextToken(int);
saTokenTypePtr lookAtNextToken();
bool reclassify();
void tokenizer(char *, saTokenTypePtr *, int *, char *);

// Expression Management functions
int buildExpressionStack(saExpressionTypePtr, saExpressionTypePtrArray, int);
saExpressionTypePtrArray generateExpStack(void);
int saParserParse(char *, char [], saExpressionTypePtrArray);
saExpressionTypePtr parseExpression(int);
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
        switch (c)
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

    // parse where line into tokens, validate grammer
    tokenizer(where_line, global_tokenStack, &global_tokenCount, errMsg);
    if (global_tokenCount == -1)
    {
        return(-1);
    }

    // Build the expression tree
    return(buildExpressionStack(parseExpression(0), expStack, 0));
}

saExpressionTypePtrArray generateExpStack()
{
    return(malloc(sizeof(saExpressionTypePtr) * MAXCHARS));
}

saTokenTypePtr getNextToken(int token_type)
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

saTokenTypePtr lookAtNextToken()
{
    return(global_tokenStack[global_tokenIndex]);
}


saExpressionTypePtr parseExpression(precedence)
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

saExpressionTypePtr andExpression(saExpressionTypePtr left, saTokenTypePtr currToken) 
{
    saExpressionTypePtr right = parseExpression(SA_PRECEDENCE_AND);
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->left = left;
    exp->right = right;
    exp->type = SA_TOKEN_AND;
    exp->field = "and";
    return(exp);
}

saExpressionTypePtr orExpression(saExpressionTypePtr left, saTokenTypePtr currToken) 
{
    saExpressionTypePtr right = parseExpression(SA_PRECEDENCE_OR);
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->left = left;
    exp->right = right;
    exp->type = SA_TOKEN_OR;
    exp->field = "or";
    return(exp);
}


saExpressionTypePtr inExpression(saExpressionTypePtr left, saTokenTypePtr currToken)
{
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->left = left;
    saExpressionTypePtr right = parseExpression(SA_PRECEDENCE_IN);
    exp->right = right;
    exp->type = SA_TOKEN_IN;
    exp->field = "in";
    return(exp);
}

saExpressionTypePtr byExpression(saExpressionTypePtr left, saTokenTypePtr currToken)
{
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->left = left;
    saExpressionTypePtr right = parseExpression(SA_PRECEDENCE_BY);
    exp->right = right;
    exp->type = SA_TOKEN_BY;
    exp->field = "by";
    return(exp);
}

saExpressionTypePtr isExpression(saExpressionTypePtr left, saTokenTypePtr currToken)
{
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->left = left;
    saExpressionTypePtr right = parseExpression(SA_PRECEDENCE_IS);
    exp->right = right;
    exp->type = SA_TOKEN_IS;
    exp->field = "is";
    return(exp);
}

saExpressionTypePtr parenExpression(saTokenTypePtr currToken)
{
    saExpressionTypePtr exp = parseExpression(0);
    getNextToken(SA_TOKEN_RIGHT_PAREN);
    return(exp);
}

saExpressionTypePtr fieldExpression(saTokenTypePtr currToken) 
{
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->field = currToken->token;
    exp->type = currToken->token_type;
    exp->left = NULL;
    exp->right = NULL;
    return(exp);
}

saExpressionTypePtr conceptExpression(saTokenTypePtr currToken) 
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

saExpressionTypePtr contextExpression(saTokenTypePtr currToken)
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

saExpressionTypePtr modifierOperatorExpression(saTokenTypePtr currToken) 
{
    saExpressionTypePtr right = parseExpression(currToken->token_precedence);
    saExpressionTypePtr exp = malloc(sizeof(saExpressionType));
    exp->field = currToken->token;
    exp->type = currToken->token_type;
    exp->left = NULL;
    exp->right = right;
    return(exp);
}

saExpressionTypePtr notOperatorExpression(saTokenTypePtr currToken) 
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

