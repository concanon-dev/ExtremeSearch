/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software
 contains proprietary trade and business secrets.
*/
#ifndef __satoken
#define __satoken

#define MAXMODIFIERS 16

#define SA_PRECEDENCE_FIELD 0
#define SA_PRECEDENCE_OR 1
#define SA_PRECEDENCE_AND 2
#define SA_PRECEDENCE_IS 3
#define SA_PRECEDENCE_IN 4
#define SA_PRECEDENCE_BY 5
#define SA_PRECEDENCE_PREFIX 6

#define SA_TOKEN_NO_TOKEN -1
#define SA_TOKEN_LEFT_PAREN 0
#define SA_TOKEN_RIGHT_PAREN 1
#define SA_TOKEN_AND 2
#define SA_TOKEN_OR 3
#define SA_TOKEN_FIELD 4
#define SA_TOKEN_IS 5
#define SA_TOKEN_IN 6
#define SA_TOKEN_BY 7
#define SA_TOKEN_NOT 8
#define SA_TOKEN_MODIFIER 9
#define SA_TOKEN_FUZZYTERMSET 10
#define SA_TOKEN_FUZZYSET 11
#define SA_TOKEN_COLUMN 12
#define SA_TOKEN_EOF 13

typedef struct saTokenTypeStruct
{
    char *token;
    int token_precedence;
    int token_type;
} saTokenType;
typedef saTokenType *saTokenTypePtr;

#endif
