/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "saStringField.h"

static char *fieldList[1024];
static char inbuf[1024 * 1024];

extern int saCSVFGetLine(FILE *, char [], char *[]);

int saStringFieldCountLines(saStringFieldPtr p)
{
    if (p->file[0] == '\0')
        return(0);

    FILE *f = fopen(p->file, "r");
    if (f == NULL)
        return(0);

    p->fileSize = 0;
    while(saCSVFGetLine(f, inbuf, fieldList) != 0)
          p->fileSize++;

    fclose(f);
    return(p->fileSize);
}

char *saStringFieldGetValue(saStringFieldPtr p)
{
    if (p->file[0] == '\0')
        return(p->value);

    FILE *f = fopen(p->file, "r");
    if (f == NULL)
        return("");
    int whichRow = (rand() % p->fileSize) + 1;
    int rowCount = 0;
    bool done = false;
    while (done == false)
    {
         int numFields = saCSVFGetLine(f, inbuf, fieldList);
         rowCount++;
         if (numFields == 0)
         {
             p->value[0] = '\0';
             done = true;
         }
         else if (rowCount == whichRow)
         {
             if (p->field > numFields)
                 strcpy(p->value, fieldList[numFields-1]);
             else if (p->field <= 0)
                 strcpy(p->value, fieldList[0]);
             else
                 strcpy(p->value, fieldList[p->field-1]);
             done = true;
         }
    }
    fclose(f);
    return(p->value);
}

bool saStringFieldIncrement(saStringFieldPtr p)
{
    return(true);
}
