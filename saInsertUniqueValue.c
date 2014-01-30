/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

char *insertUniqueValue(char *values[], char *field, int *numUniqueValues)
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
