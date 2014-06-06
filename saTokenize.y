%{
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "saToken.h"

// stuff from flex that bison needs to know about:
extern int line_num;
extern int col_num;
extern int yylex();
extern FILE *yyin;
extern void setStringSource(char * line);

#define MAXCHARS 256
void yyerror(const char *s);
char parse_error_msg[MAXCHARS];
void addToken(char *tk, int tk_type, int tk_precedence);
void tokenizer(char *line, saTokenTypePtr *stack, int *tokenCount, char *error_msg);
void reclassify();
saTokenTypePtr *tokenStack;
int tokenCount = 0;

%}

// Symbols.
%union
{
    //saTokenType ti;
    char *sval;
};

%token <sval> TOKEN_RIGHT_PAREN
%token <sval> TOKEN_LEFT_PAREN
%token <sval> TOKEN_AND
%token <sval> TOKEN_OR
%token <sval> TOKEN_NOT
%token <sval> TOKEN_IN
%token <sval> TOKEN_IS
%token <sval> TOKEN_FIELD
%type <sval> container is_modifiers is_concept in_clause in_field field or and is in right_paren left_paren 

%%
container:
    is_modifiers
    | is_modifiers and is_modifiers
    | is_modifiers or is_modifiers
    ;

is_modifiers:
    is_modifiers field
    | left_paren is_modifiers field right_paren
    | is_concept field
    | left_paren is_concept field right_paren
    ;

is_concept:
    in_clause is
    ;

in_clause:
    in_field
    | field
    ;

in_field:
    field in field
    ;

/* Push each valid token onto stack */
field:
    TOKEN_FIELD
    {
        addToken($1, SA_TOKEN_FIELD, SA_PRECEDENCE_FIELD);
    }
    | TOKEN_NOT TOKEN_FIELD
    {
        addToken($1, SA_TOKEN_NOT, SA_PRECEDENCE_PREFIX);
        addToken($2, SA_TOKEN_FIELD, SA_PRECEDENCE_FIELD);
    }
    ;
in:
    TOKEN_IN
    {
        addToken($1, SA_TOKEN_IN, SA_PRECEDENCE_IN);
    }
    ;
is:
    TOKEN_IS
    {
        addToken($1, SA_TOKEN_IS, SA_PRECEDENCE_IS);
    }
    ;
and:
    TOKEN_AND
    {
        addToken($1, SA_TOKEN_AND, SA_PRECEDENCE_AND);
    }
    ;
or:
    TOKEN_OR
    {
        addToken($1, SA_TOKEN_OR, SA_PRECEDENCE_OR);
    }
    ;
left_paren:
    TOKEN_LEFT_PAREN
    {
        addToken($1, SA_TOKEN_LEFT_PAREN, SA_PRECEDENCE_FIELD);
    }
    ;

right_paren:
    TOKEN_RIGHT_PAREN
    {
        addToken($1, SA_TOKEN_RIGHT_PAREN, SA_PRECEDENCE_FIELD);
    }
    ;

%%

void addToken(char *tk, int tk_type, int tk_precedence) {
    tokenStack[tokenCount] = malloc(sizeof(saTokenType));
    tokenStack[tokenCount]->token = tk;
    tokenStack[tokenCount]->token_type = tk_type;
    tokenStack[tokenCount]->token_precedence = tk_precedence;
    tokenCount++;
}

void walkStack()
{
    printf("\nStack:\n");
    printf("count=%d\n", tokenCount);
    int i;
    for(i=0; i<tokenCount; i++)
        printf("stack[%d]=%s type=%d\n", i, tokenStack[i]->token, tokenStack[i]->token_type);
}

#ifdef _MAIN_
int main(void) {
    //char str[] = "Height is tall";
    //char str[] = "Height in JOE is tall";
    char str[] = "(Height in JOE is very tall) and (Height in JOE is very short)";
    int count = 0;
    saTokenTypePtr stack[MAXCHARS];
    char error_msg[MAXCHARS];
    tokenizer(str, stack, &count, error_msg);
    reclassify();
    if (strlen(error_msg > 0)) {
        printf("error: %s", error_msg);
    }
}
#endif

void tokenizer(char *line, saTokenTypePtr *stack, int *inCount, char *error_msg) {
    tokenCount = 0;
    parse_error_msg[0] = '\0';
    tokenStack = stack;
    setStringSource(line);
    yyparse();

    if (parse_error_msg[0] != '\0') {
        strcpy(error_msg,parse_error_msg);
        *inCount = -1;
    }
    else {
        tokenStack[tokenCount] = malloc(sizeof(saTokenType));
        tokenStack[tokenCount]->token = "";
        tokenStack[tokenCount]->token_type = SA_TOKEN_EOF;
        tokenStack[tokenCount]->token_precedence = SA_PRECEDENCE_FIELD;
        tokenCount++;
        *inCount = tokenCount;
        //walkStack();
        reclassify();
    }
}

void reclassify()
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

    // find all the MODIFIERS (hedges) and Term Sets
    for(i=0; i<tokenCount; i++)
    {
        if (tokenStack[i]->token_type == SA_TOKEN_IS)
        {
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
        /*
        else if (tokenStack[i]->token_type == SA_TOKEN_BY)
        {
            tokenStack[i+1]->token_type = SA_TOKEN_FUZZYTERMSET;
            tokenStack[i+1]->token_precedence = SA_PRECEDENCE_FIELD;
        }
        */
    }
}

void yyerror(const char *s) {
    sprintf(parse_error_msg, "[%d, %d]: %s", line_num, col_num-2, s);
}
