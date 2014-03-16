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
#include <unistd.h>
#include "saDoubleField.h"
#include "saFloatField.h"
#include "saField.h"
#include "saIntField.h"
#include "saOutput.h"
#include "saProperties.h"
#include "saStringField.h"
#include "saTimeField.h"
 
static char buffer[SAOUTPUT_OUTBUFSIZE];
static char outBuf[SAOUTPUT_OUTBUFSIZE];

extern double saDoubleFieldGetValue(saDoubleFieldPtr);

extern float saFloatFieldGetValue(saFloatFieldPtr);

extern long saIntFieldGetValue(saIntFieldPtr);

extern int saPropertiesMatch(saPropertiesPtr, char *);

extern char *saStringFieldGetValue(saStringFieldPtr);

extern bool saTimeFieldGetDayHour(saTimeFieldPtr, int *);
extern char *saTimeFieldGetValueFormatted(saTimeFieldPtr);

char *replace_str(char *oldStr, char *newStr, char *orig, char *rep)
{
    char *p;
    if (!(p = strstr(oldStr, orig)))  // Is 'orig' even in 'str'?
        return(oldStr);

    strncpy(newStr, oldStr, p-oldStr);
    newStr[p-oldStr] = '\0';
    strcpy(newStr+(p-oldStr), rep);
    strcpy(newStr+(p-oldStr)+strlen(rep), oldStr+(p-oldStr)+strlen(orig));
    return(newStr);
}

saOutputPtr saOutputConfigure(saPropertiesPtr p)
{
    int k;
    saOutputPtr outputPtr = (saOutputPtr)malloc(sizeof(saOutputType));

    /*
        Set up the output
        we need at least an output file (output.file) and output template (output.template)
    */

    if ((outputPtr->outputClock = saPropertiesGetValue(p, saPropertiesMatch(p, "output.clock"))) == NULL)
         outputPtr->outputClock = "time";

    outputPtr->outputFile = saPropertiesGetValue(p, saPropertiesMatch(p, "output.file"));
    if (outputPtr->outputFile == NULL)
    {
        fprintf(stderr, "No output file defined 'output.file'\n");
        return(NULL);
    }

    outputPtr->outputHeader = saPropertiesGetValue(p, saPropertiesMatch(p, "output.header"));

    outputPtr->outputTemplate = saPropertiesGetValue(p, saPropertiesMatch(p, "output.template"));
    if (outputPtr->outputTemplate == NULL)
    {
        fprintf(stderr, "No output template defined 'output.template'\n");
        return(NULL);
    }

    return(outputPtr);
}

bool saOutputClose(saOutputPtr p)
{
    if (p->f != NULL)
        fclose(p->f);
    else
        return(false);
    return(true);
}

bool saOutputOpen(saOutputPtr p, char *mode)
{
    p->f = fopen(p->outputFile, mode);
    if (p->f == NULL)
        return(false);

    return(true);
}

bool saOutputWrite(saOutputPtr p, saFieldPtr fieldArray[])
{
    char replStr[1024];
    char valueStr[SASTRINGFIELD_MAXVALUE];
    int dayHour[2];
    int i;

    // get the hour and day of the "current" event so we know which field to write 
    i=0;
    int tIndex = -1;
    while(tIndex == -1 && i<SAFIELD_MAX)
    {
        if (fieldArray[i] != NULL)
            if (!strcmp(p->outputClock, fieldArray[i]->name))
                tIndex = i;
        i++;
    }
    if (tIndex == -1)
    {
        fprintf(stderr, "Clock field '%s' does not exist, exiting...\n", p->outputClock);
        return(false);
    }
    saTimeFieldGetDayHour(((saTimeFieldPtr)fieldArray[tIndex]->fieldPtr), dayHour);
    
    strcpy(outBuf, p->outputTemplate);    
    for(i=0; i<SAFIELD_MAX; i++)
    {
        if (fieldArray[i] != NULL)
        {
            if (fieldArray[i]->valid[dayHour[0]][dayHour[1]] == 1)
            {
                sprintf(replStr, "$%s$", fieldArray[i]->name);
                if (!strcmp(fieldArray[i]->type, SAFIELD_DOUBLE))
                {
                    sprintf(valueStr, "%.4f", saDoubleFieldGetValue((saDoubleFieldPtr)fieldArray[i]->fieldPtr));
                }
                else if (!strcmp(fieldArray[i]->type, SAFIELD_FLOAT))
                {
                    sprintf(valueStr, "%.4f", saFloatFieldGetValue((saFloatFieldPtr)fieldArray[i]->fieldPtr));
                }
                else if (!strcmp(fieldArray[i]->type, SAFIELD_INT))
                {
                    sprintf(valueStr, "%ld", saIntFieldGetValue((saIntFieldPtr)fieldArray[i]->fieldPtr));
                }
                else if (!strcmp(fieldArray[i]->type, SAFIELD_STRING))
                {
                    sprintf(valueStr, "%s", saStringFieldGetValue((saStringFieldPtr)fieldArray[i]->fieldPtr));
                }
                else if (!strcmp(fieldArray[i]->type, SAFIELD_TIMESTAMP))
                {
                    sprintf(valueStr, "%s", saTimeFieldGetValueFormatted((saTimeFieldPtr)fieldArray[i]->fieldPtr));
                }
                char *newBuf = replace_str(outBuf, buffer, replStr, valueStr);
                if (newBuf != outBuf)
                    strcpy(outBuf, newBuf);
            }
        }
    }
    fputs(outBuf, p->f);
    fputs("\n", p->f);
    return(true);    
}

bool saOutputWriteHeader(saOutputPtr p)
{
    if (p->f == NULL)
        return(false);

    if (p->outputHeader != NULL && p->outputHeader[0] != '\0')
    {
        fputs(p->outputHeader, p->f);
        fputs("\n", p->f);
    }
    return(true);
}

