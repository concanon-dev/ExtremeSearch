/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "saProperties.h"

#define MAXBUFSIZE 1024

char *trim(char *);

char *getLine(FILE *infile)
{
    if (feof(infile))
        return(NULL);
    char *inBuf = (char *)malloc(sizeof(char)*MAXBUFSIZE);
    inBuf[0] = '\0';
    bool done = false;
    while(done == false)
    {
        if (fgets(inBuf+strlen(inBuf), MAXBUFSIZE-strlen(inBuf), infile) == NULL)
            done = true;
        else if (inBuf[strlen(inBuf)-1] == '\n' && inBuf[strlen(inBuf)-2] != '\\')
        {
            inBuf[strlen(inBuf)-1] = '\0';
            done = true;
        }
         else
            inBuf[strlen(inBuf)-2] = '\0';
    }
    return(inBuf);
}

bool fReadField(FILE *f, saPropertiesPtr p)
{
    char *inBuf = getLine(f);    
    if (inBuf == NULL)
        return(false);

    inBuf = trim(inBuf);
    if (inBuf[0] == '\0')
        return(true);
    if (inBuf[0] == '#')
        return(true);

    p->keyList[p->numFields] = inBuf;
    char *cursor = inBuf;
    while(*cursor != '=' && *cursor != '\0' && *cursor != ' ')
          cursor++;
    if (*cursor == '\0')
        return(false);
    
    while(*cursor == ' ')
    {
          *cursor = '\0';
          cursor++;
    }
    
    if (*cursor != '=')
        return(false);
    else
    {
        *cursor = '\0';
        cursor++;
    }
    
    while(*cursor == ' ' && *cursor != '\0')
          cursor++;
    if (*cursor == '\0')
        return(false);

    p->valueList[p->numFields] = cursor;
    cursor = cursor + strlen(cursor) - 1;
    while(*cursor <= ' ' && *cursor != '\0')
          cursor--;
    if (*cursor == '\0')
        return(false);
    *(cursor+1) = '\0';
    p->numFields++;     
    return(true);
}

int lookup(saPropertiesPtr p, char *field, int start)
{
    if (start < 0)
        return(-1);

    int i=start;
    while(i<p->numFields)
    {
       if (!strncmp(p->keyList[i], field, strlen(field)))
           return(i);
       i++; 
    }
    return(-1);
}


int match(saPropertiesPtr p, char *field, int start)
{
    if (start < 0)
        return(-1);

    int i=start;
    while(i<p->numFields)
    {
       if (!strcmp(p->keyList[i], field))
           return(i);
       i++;
    }
    return(-1);
}

void sortFields(saPropertiesPtr p)
{
    char *tmp1, *tmp2;
    int c, d, n;

    n = p->numFields;
    for(c=0; c<(n-1); c++)
        for(d=0; d<(n-c-1); d++)
            if (strcmp(p->keyList[d], p->keyList[d+1]) > 0)
            {
                tmp1 = p->keyList[d];
                tmp2 = p->valueList[d];
                p->keyList[d] = p->keyList[d+1];
                p->valueList[d] = p->valueList[d+1];
                p->keyList[d+1] = tmp1;
                p->valueList[d+1] = tmp2;
            }
}

char *trim(char *str)
{
    if (str == NULL)
        return(NULL);

    while(*str <= ' ' && *str != '\0')
          str++;

    return(str);
}

saPropertiesPtr saPropertiesLoad(char *confFileName)
{
    FILE *f = fopen(confFileName, "r");
    if (f == NULL)
        return(NULL);

    saPropertiesPtr p = malloc(sizeof(saPropertiesType));
    p->numFields = 0;
    while(fReadField(f, p) == true)
          ;
    fclose(f);

    sortFields(p);
    return(p);
}

int saPropertiesLookup(saPropertiesPtr p, char *field)
{
    return(lookup(p, field, 0));
}

int saPropertiesLookupNext(saPropertiesPtr p, char *field, int start)
{
    return(lookup(p, field, start));
}

int saPropertiesMatch(saPropertiesPtr p, char *field)
{
    return(match(p, field, 0));
}

int saPropertiesMatchNext(saPropertiesPtr p, char *field, int start)
{
    return(match(p, field, start));
}
