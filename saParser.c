/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "saExpression.h"
#include "saToken.h"

#define MAXCHARS 256

/* Generate Expressions, given a specific token */
// Infix Tokens
saExpressionTypePtr andExpression(saExpressionTypePtr, saTokenTypePtr);
saExpressionTypePtr inExpression(saExpressionTypePtr, saTokenTypePtr);
saExpressionTypePtr isExpression(saExpressionTypePtr, saTokenTypePtr);
saExpressionTypePtr orExpression(saExpressionTypePtr, saTokenTypePtr);

// Name Tokens
saExpressionTypePtr fieldExpression(saTokenTypePtr); 
saExpressionTypePtr semanticTermExpression(saTokenTypePtr);
saExpressionTypePtr contextExpression(saTokenTypePtr);
saExpressionTypePtr parenExpression(saTokenTypePtr);

// Prefix (unary) Tokens
saExpressionTypePtr modifierOperatorExpression(saTokenTypePtr);
saExpressionTypePtr notOperatorExpression(saTokenTypePtr);

// Token Management functions
saTokenTypePtr getNextToken(int);
saTokenTypePtr lookAtNextToken();
bool reclassify(saTokenTypePtr [], int, char *);
void tokenize(char *, saTokenTypePtr [], int *, char *);

// Expression Management functions
int buildExpressionStack(saExpressionTypePtr, saExpressionTypePtrArray, int);
saExpressionTypePtrArray generateExpStack(void);
int parse(char *, char [], saExpressionTypePtrArray);
saExpressionTypePtr parseExpression(int);
void walkExpressionStack(saExpressionTypePtrArray, int);

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
    walkExpressionStack(expStack, parse(where_line, errMsg, expStack));
}
#endif

int parse(char *where_line, char errMsg[], saExpressionTypePtrArray expStack)
{
    int i;

    // parse where line into tokens
    tokenize(where_line, global_tokenStack, &global_tokenCount, errMsg);
    if (global_tokenCount == -1)
    {
        fprintf(stderr, "tokenize failure: %s\n", errMsg);
        return(-1);
    }
    // Reset the token type if necessary
    if (!reclassify(global_tokenStack, global_tokenCount, errMsg))
       return(-1);

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
        left = semanticTermExpression(currToken); // nameExpression
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

void walkExpressionStack(saExpressionTypePtrArray expStack, int stackIndex)
{
    fprintf(stderr, "count=%d\n", stackIndex);
    int i;
    for(i=0; i<stackIndex; i++)
        fprintf(stderr, "stack[%d]=%s type=%d\n", i, expStack[i]->field, expStack[i]->type);

    return;
}

bool reclassify(saTokenTypePtr tokenStack[], int tokenCount, char *errMsg)
{
    int i;

    // replace might/must/should be with is "hedge"
    for(i=0; i<tokenCount-1; i++)
    {
        if (!strcmp(tokenStack[i]->token, "might") && !strcmp(tokenStack[i+1]->token, "be"))
        {
            tokenStack[i]->token = "is";
            tokenStack[i]->token_type = SA_TOKEN_IS;
            tokenStack[i]->token_precedence = SA_PRECEDENCE_IS;

            tokenStack[i+1]->token = "slightly";
            tokenStack[i+1]->token_type = SA_TOKEN_FIELD;
            tokenStack[i+1]->token_precedence = SA_PRECEDENCE_FIELD;
        }
        else if (!strcmp(tokenStack[i]->token, "must") && !strcmp(tokenStack[i+1]->token, "be"))
        {
            tokenStack[i]->token = "is";
            tokenStack[i]->token_type = SA_TOKEN_IS;
            tokenStack[i]->token_precedence = SA_PRECEDENCE_IS;

            tokenStack[i+1]->token = "extremely";
            tokenStack[i+1]->token_type = SA_TOKEN_FIELD;
            tokenStack[i+1]->token_precedence = SA_PRECEDENCE_FIELD;
        }
        else if (!strcmp(tokenStack[i]->token, "should") && !strcmp(tokenStack[i+1]->token, "be"))
        {
            tokenStack[i]->token = "is";
            tokenStack[i]->token_type = SA_TOKEN_IS;
            tokenStack[i]->token_precedence = SA_PRECEDENCE_IS;

            tokenStack[i+1]->token = "very";
            tokenStack[i+1]->token_type = SA_TOKEN_FIELD;
            tokenStack[i+1]->token_precedence = SA_PRECEDENCE_FIELD;
        }
    }

    bool foundIS = false;
    // find all the MODIFIERS (hedges) and Term Sets
    for(i=0; i<tokenCount; i++)
    {
        if (tokenStack[i]->token_type == SA_TOKEN_IS)
        {
            foundIS = true;
            int j = i+1;
            bool done = false;
            while(done == false)
            {
                  if (j == tokenCount)
                      done = true;
                  else if (tokenStack[j]->token_type == SA_TOKEN_FIELD)
                  {
                      tokenStack[j]->token_type = SA_TOKEN_MODIFIER;
                      tokenStack[j++]->token_precedence = SA_PRECEDENCE_PREFIX;
                  }
                  else if (tokenStack[j]->token_type == SA_TOKEN_NOT)
                      j++;
                  else
                      done = true;
            }
            if (tokenStack[j-1]->token_type == SA_TOKEN_MODIFIER)
            {
               tokenStack[j-1]->token_type = SA_TOKEN_FUZZYSET;
               tokenStack[j-1]->token_precedence = SA_PRECEDENCE_FIELD;
            }
        }
        else if (tokenStack[i]->token_type == SA_TOKEN_IN)
        {
            tokenStack[i+1]->token_type = SA_TOKEN_FUZZYTERMSET;
            tokenStack[i+1]->token_precedence = SA_PRECEDENCE_FIELD;
        }
    }
    if (foundIS == false)
    {
        strcpy(errMsg, "Missing IS operator");
        return(false);
    }
    return(true);
}


void tokenize(char *line, saTokenTypePtr tokenStack[], int *tokenCount, char *errmsg)
{
    char buffer[MAXCHARS];
    int i;
    int localCount = 0;
    int parencount = 0;

    errmsg[0] = '\0';
    *tokenCount = -1;
    bool done = false;
    while(done == false)
    { 
          buffer[0] = '\0';
          i = 0;
          while(*line == ' ')
                line++;
          if (*line == '\0')
              done = true;
          if (done == false)
          {
              if (*line == '(')
              {
                  tokenStack[localCount] = malloc(sizeof(saTokenType));
                  tokenStack[localCount]->token = "(";
                  tokenStack[localCount]->token_type = SA_TOKEN_LEFT_PAREN;
                  tokenStack[localCount]->token_precedence = SA_PRECEDENCE_FIELD;
                  localCount++;
                  parencount++;
                  line++;
              }
              else if (*line == ')')
              {
                  tokenStack[localCount] = malloc(sizeof(saTokenType));
                  tokenStack[localCount]->token = ")";
                  tokenStack[localCount]->token_type = SA_TOKEN_RIGHT_PAREN;
                  tokenStack[localCount]->token_precedence = SA_PRECEDENCE_FIELD;
                  localCount++;
                  parencount--;
                  if (parencount < 0)
                  {
                      sprintf(errmsg, "Unbalanced Parenthesis '%s'", line);
                      return;
                  }
                  line++;
              }
              else if(!isalnum(*line) && (*line != '-') && (*line != '_') && (*line != '.'))
              {
                  sprintf(errmsg, "Illegal Character %c", *line);
                  return;
              }
              else
              {
                  while(isalnum(*line) || (*line == '-') || (*line == '_') || (*line == '.'))
                  {
                        buffer[i++] = *line;
                        line++;
                  }
                  buffer[i] = '\0';
                  tokenStack[localCount] = malloc(sizeof(saTokenType));
                  tokenStack[localCount]->token = (char *)malloc(sizeof(buffer)+1);
                  strcpy(tokenStack[localCount]->token, buffer);
                  if (!strcmp(buffer, "and") || !strcmp(buffer, "AND")
                      || !strcmp(buffer, "but") || !strcmp(buffer, "BUT")
                      || !strcmp(buffer, "yet") || !strcmp(buffer, "YET"))
                  {
                      tokenStack[localCount]->token_type = SA_TOKEN_AND;
                      tokenStack[localCount]->token_precedence = SA_PRECEDENCE_AND;
                  }
                  else if (!strcmp(buffer, "or") || !strcmp(buffer, "OR"))
                  {
                      tokenStack[localCount]->token_type = SA_TOKEN_OR;
                      tokenStack[localCount]->token_precedence = SA_PRECEDENCE_OR;
                  }
                  else if (!strcmp(buffer, "is") || !strcmp(buffer, "IS"))
                  {
                      tokenStack[localCount]->token_type = SA_TOKEN_IS;
                      tokenStack[localCount]->token_precedence = SA_PRECEDENCE_IS;
                  }
                  else if (!strcmp(buffer, "in") || !strcmp(buffer, "IN"))
                  {
                      tokenStack[localCount]->token_type = SA_TOKEN_IN;
                      tokenStack[localCount]->token_precedence = SA_PRECEDENCE_IN;
                  }
                  else if (!strcmp(buffer, "not") || !strcmp(buffer, "NOT"))
                  {
                      tokenStack[localCount]->token_type = SA_TOKEN_NOT;
                      tokenStack[localCount]->token_precedence = SA_PRECEDENCE_PREFIX;
                  }
                  else
                  {
                      tokenStack[localCount]->token_type = SA_TOKEN_FIELD;
                      tokenStack[localCount]->token_precedence = SA_PRECEDENCE_FIELD;
                  }
                  localCount++;
              }
         }
    }
    if (parencount > 0)
    {
        sprintf(errmsg, "Missing ')' %d", parencount);
        return;
    }
    tokenStack[localCount] = malloc(sizeof(saTokenType));
    tokenStack[localCount]->token = "";
    tokenStack[localCount]->token_type = SA_TOKEN_EOF;
    tokenStack[localCount]->token_precedence = SA_PRECEDENCE_FIELD;
    *tokenCount = localCount+1;
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

saExpressionTypePtr semanticTermExpression(saTokenTypePtr currToken) 
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

