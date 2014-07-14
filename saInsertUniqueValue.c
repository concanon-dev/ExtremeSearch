/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: insertUniqueValue

Description: insert a unique value in an array (used by xspre* commands)

Functions:
    insertUniqueValue

*/
#define _SA_INSERTUNIQUEVALUE_C
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "saInsertUniqueValue.h"

inline char *insertUniqueValue(char *values[], char *field, int *numUniqueValues)
{
    int i=0;
    bool found = false;
    while(i<*numUniqueValues && found == false)
    {
        if (!strcmp(values[i], field))
            found = true;
        else
            i++;
    }
    if (!found)
    {
        values[*numUniqueValues] = malloc(strlen(field));
        strcpy(values[*numUniqueValues], field);
        *numUniqueValues += 1;
        return(values[*numUniqueValues-1]);
    }
    return(values[i]);
}
