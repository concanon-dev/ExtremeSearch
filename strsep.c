/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: strsep

Description:
    Utility function for windows as strsep is not available in gnu/cygwin

Functions:
    strsep
*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strsep(char **inStr, const char *separators)
{
    if (*inStr == NULL)
        return(NULL);

    char *s = *inStr;
    int sepLen = strlen(separators);

    char *t = s;
    while(*t != '\0')
    {
        bool innerDone = false;
        int i=0;
        while(i<sepLen && innerDone == false)
        {
            if (*t == separators[i])
                innerDone = true;
            else
                i++;
        }
        if (innerDone == true)
        {
            *t = '\0';
            *inStr = t + 1;
            return(s);
        }
        else
            t++;
    }
    *inStr = NULL;
    return(s);
}
