
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
