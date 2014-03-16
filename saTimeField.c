/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "saTimeField.h"

extern int saFieldVarianceLong(long, float);

long saTimeFieldCalcTime(char *timeRef)
{
    char timeStr[128];
    int i;

    if (timeRef == NULL || *timeRef == '\0')
        return((long)0);

    strcpy(timeStr, timeRef);
    bool allnum = true;
    i = 1;
    while (allnum == true && i<strlen(timeStr))
        allnum = isdigit(timeStr[i++]) ? true : false;

    if (allnum == true && (timeStr[0] == '-' || isdigit(timeStr[0])))
        return(time(NULL) + atol(timeStr));

    long t = atol(timeStr);

    char m = '0';
    if (strlen(timeStr) >= 5 && timeStr[strlen(timeStr)-2] == '@')
    {
        m = timeStr[strlen(timeStr)-1];
        timeStr[strlen(timeStr)-2] = '\0';
    }
    char c = 's';
    if (strlen(timeStr) >= 3)
    {
        c = timeStr[strlen(timeStr)-1];
        if (!isalpha(c))
        {
            fprintf(stderr, "malformed time %s, returning 0\n", timeRef);
            return((long)0);
        }
        timeStr[strlen(timeStr)-1] = '\0';
        if (c == 's' || c == 'S')
            t = (time(NULL) + (long)atol(timeStr));
        else if (c == 'm' || c == 'M')
            t = (time(NULL) + (long)atol(timeStr) * 60);
        else if (c == 'h' || c == 'H')
            t = (time(NULL) + (long)atol(timeStr) * 60 * 60);
        else if (c == 'd' || c == 'D')
            t = (time(NULL) + (long)atol(timeStr) * 60 * 60 * 24);

        if (m == 'm' || m == 'M')
            while(t % 60 != 0)
                  t--;
        else if (m == 'h' || m == 'H')
            while(t % (60 * 60) != 0)
                  t--;
        else if (m == 'd' || m == 'D')
        {
            time_t clock = (time_t)t;
            struct tm *timePtr = localtime(&clock);
            timePtr->tm_hour = 0;
            timePtr->tm_min = 0;
            timePtr->tm_sec = 0;
            t = (long)mktime(timePtr);
        }
        return(t);
    }
    fprintf(stderr, "unable to calc time '%s', returning 0\n", timeRef);
    return((long)0);
}

long saTimeFieldConvertToSeconds(char *timeStr)
{
    int i;

    if (timeStr == NULL)
        return((long)0);

    bool allnum = true;
    i = 0;
    while (allnum == true && i<strlen(timeStr))
        allnum = isdigit(timeStr[i++]) ? true : false;

    if (allnum == true)
        return(atol(timeStr));

    // see if this is malformed (meaning non-digits interspersed with the digits)
    if (i != strlen(timeStr))
    {
        fprintf(stderr, "malformed increment %s, defaulting to 1 second\n", timeStr);
        return((long)1);
    }

    char c = timeStr[i-1];
    timeStr[i-1] = '\0';
    if (c == 's' || c == 'S')
        return(atol(timeStr));
    else if (c == 'm' || c == 'M')
        return(atol(timeStr) * (long)60);
    else if (c == 'h' || c == 'H')
        return(atol(timeStr) * (long)60 * (long)60);
    else if (c == 'D' || c == 'D')
        return(atol(timeStr) * (long)60 * (long)60 * (long) 24);
    else
    {
        fprintf(stderr, "unknown unit of time '%c', assuming 's'\n", c);
        return(atol(timeStr));
    }
}

bool saTimeFieldGetDayHour(saTimeFieldPtr p, int *dh)
{
     time_t clock = (time_t)p->value;
     struct tm *timePtr = localtime(&clock);
     dh[0] = timePtr->tm_wday;
     dh[1] = timePtr->tm_hour;
     return(true);
}

long saTimeFieldGetValue(saTimeFieldPtr p)
{
     return(p->value);
}

char *saTimeFieldGetValueFormatted(saTimeFieldPtr p)
{
     time_t clock = (time_t)p->value;
     struct tm *timePtr = localtime(&clock);
     strftime(p->valueFormatted, SATIMEFIELD_MAXFORMAT, p->format, timePtr);
     return(p->valueFormatted);
}

bool saTimeFieldIncrement(saTimeFieldPtr p)
{
     p->value += p->increment + saFieldVarianceLong(p->increment, p->incrementVariance);
     return(true);
}
