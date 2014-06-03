%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "saToken.h"

// stuff from flex that bison needs to know about:
extern int line_num;
extern int yylex();
extern FILE *yyin;
extern void setStringSource(char * line);

#define MAXCHARS 256
void yyerror(const char *s);
char parse_error_msg[MAXCHARS];
void addToken(char *tk, int tk_type, int tk_precedence);
void tokenizer(char *line, saTokenTypePtr *stack, int *tokenCount, char *error_msg);
saTokenTypePtr *tokenStack;
int localCount = 0;

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
%token <sval> TOKEN_BY /* support this ? */
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
    tokenStack[localCount] = malloc(sizeof(saTokenType));
    tokenStack[localCount]->token = tk;
    tokenStack[localCount]->token_type = tk_type;
    tokenStack[localCount]->token_precedence = tk_precedence;
    localCount++;
}

void walkStack()
{
    printf("\nStack:\n");
    printf("count=%d\n", localCount);
    int i;
    for(i=0; i<localCount; i++)
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
    if (error_msg) {
        printf("error: %s", error_msg);
    }
    else {
        printf("token count = %d", count);
    }
}
#endif

void tokenizer(char *line, saTokenTypePtr *stack, int *tokenCount, char *error_msg) {
    parse_error_msg[0] = '\0';
    tokenStack = stack;
    setStringSource(line);
    yyparse();

    if (parse_error_msg[0] != '\0') {
        strcpy(error_msg,parse_error_msg);
    }
    else {
        tokenStack[localCount] = malloc(sizeof(saTokenType));
        tokenStack[localCount]->token = "";
        tokenStack[localCount]->token_type = SA_TOKEN_EOF;
        tokenStack[localCount]->token_precedence = SA_PRECEDENCE_FIELD;
        localCount++;
        *tokenCount = localCount;
        //walkStack();
    }
    //return 0;
}

void yyerror(const char *s) {
    sprintf(parse_error_msg, "Parse Error[%d]: %s", line_num, s);
}
