
#ifndef __satoken
#define __fsatoken

#define MAXMODIFIERS 16

#define SA_PRECEDENCE_FIELD 0
#define SA_PRECEDENCE_OR 1
#define SA_PRECEDENCE_AND 2
#define SA_PRECEDENCE_IS 3
#define SA_PRECEDENCE_IN 4
#define SA_PRECEDENCE_PREFIX 5

#define SA_TOKEN_NO_TOKEN -1
#define SA_TOKEN_LEFT_PAREN 0
#define SA_TOKEN_RIGHT_PAREN 1
#define SA_TOKEN_AND 2
#define SA_TOKEN_OR 3
#define SA_TOKEN_FIELD 4
#define SA_TOKEN_IS 5
#define SA_TOKEN_IN 6
#define SA_TOKEN_NOT 7
#define SA_TOKEN_MODIFIER 8
#define SA_TOKEN_FUZZYTERMSET 9
#define SA_TOKEN_FUZZYSET 10
#define SA_TOKEN_COLUMN 11
#define SA_TOKEN_EOF 12

typedef struct saTokenTypeStruct
{
    char *token;
    int token_precedence;
    int token_type;
} saTokenType;
typedef saTokenType *saTokenTypePtr;

#endif
