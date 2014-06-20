/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software
 contains proprietary trade and business secrets.
*/
#ifndef __saExpression
#define __saExpression

typedef struct saExpressionTypeStruct 
{
    char *field;
    int type;
    int intvalue;
    void *valueptr;
    struct saExpressionTypeStruct *left;
    struct saExpressionTypeStruct *right;
} saExpressionType;
typedef saExpressionType *saExpressionTypePtr;

typedef saExpressionType **saExpressionTypePtrArray;

#endif
