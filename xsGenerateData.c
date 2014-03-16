/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "saDice.h"
#include "saField.h"
#include "saLicensing.h"
#include "saOutput.h"
#include "saSignal.h"
#include "saStringField.h"
#include "saProperties.h"
#include "saTimeField.h"

extern saFieldPtr *saFieldInitArray();
extern bool saFieldConfigure(saPropertiesPtr, saFieldPtr *);
extern void *saFieldGetByName(saFieldPtr *, char *);
extern bool saFieldIncrementAll(saFieldPtr *, char *);
extern bool saFieldLoad(saPropertiesPtr, saFieldPtr *);

extern saOutputPtr saOutputConfigure(saPropertiesPtr);
extern bool saOutputClose(saOutputPtr);
extern bool saOutputOpen(saOutputPtr, char *);
extern bool saOutputWrite(saOutputPtr, saFieldPtr *);
extern bool saOutputWriteHeader(saOutputPtr);

extern saPropertiesPtr saPropertiesLoad(char *);
extern int saPropertiesLookup(saPropertiesPtr, char *);
extern int saPropertiesLookupNext(saPropertiesPtr, char *, int);
extern int saPropertiesMatch(saPropertiesPtr, char *);

long saTimeFieldCalcTime(char *);
extern long saTimeFieldGetValue(saTimeFieldPtr);
extern char *saTimeFieldGetValueFormatted(saTimeFieldPtr);

extern char *optarg;
extern int optind, optopt;

int main(int argc, char *argv[])
{
    saFieldPtr *fieldArray;
    bool argError = false;
    bool debug = false;
    bool realtime = false;
    char propsFile[256];
    int count = -1;
    long stoptime = -1;
    int c, i, j, k;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));

    propsFile[0] = '\0';
    while ((c = getopt(argc, argv, "c:dp:rs:")) != -1) 
    {
        switch (c)
        {
            case 'c':
                count = atoi(optarg);
                break;
            case 'd':
                debug = true;
                break;
            case 'p':
                strcpy(propsFile, optarg);
                break;
            case 'r':
                realtime = true;
                break;
            case 's':
                stoptime = saTimeFieldCalcTime(optarg);
                break;
            case '?':
                fprintf(stderr, "xsGenerateData-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }

    if (!strlen(propsFile))
    {
        fprintf(stderr, "xsGenerateData-F-103: No Properties file\n");
        exit(0);
    }

    // load the properties file
    saPropertiesPtr p = saPropertiesLoad(propsFile);
    if (p == NULL)
    {
        fprintf(stderr, "xsGenerateData-F-105: Can't load properties file: %s\n", propsFile);
        exit(1);
    }

    // print out the fields and values
    if (debug)
        for(i=0; i<p->numFields; i++)
            fprintf(stderr, "|%s| = |%s|\n", p->keyList[i], p->valueList[i]);

    // set up the output
    saOutputPtr outputPtr = saOutputConfigure(p);
    if (outputPtr == NULL)
    {
        fprintf(stderr, "Output not configured, exiting\n");
        exit(1);
    }

    // load the fields from the properties
    fieldArray = saFieldInitArray();
    if (saFieldLoad(p, fieldArray) == false)
    {
        fprintf(stderr, "Failed to load fields, exiting\n");
        exit(1);
    }

    // build the fields themselves
    if (saFieldConfigure(p, fieldArray) == false)
    {
        fprintf(stderr, "Failed to configure fields, exiting\n");
        exit(1);
    }

    if (debug)
    {
        for(i=0; i<SAFIELD_MAX; i++)
            if (fieldArray[i] != NULL)
            {
                fprintf(stderr, "field.%d.name=%s\n", i, fieldArray[i]->name);
                fprintf(stderr, "field.%d.type=%s\n", i, fieldArray[i]->type);
            }
    }

    //  If count is not set on the cmd line, then check the properties file for when to stop
    if (count == -1)
    {
        k = saPropertiesMatch(p, "eventcount");
        if (k != -1)
            count = atoi(saPropertiesGetValue(p, k));
        else
        {
            k = saPropertiesMatch(p, "stoptime");
            if (k != -1)
                stoptime = saTimeFieldCalcTime(saPropertiesGetValue(p, k));
            else
            {
                k = saPropertiesMatch(p, "realtime");
                if (k != -1)
                    realtime = true;
            }
        }
    }

    if (saOutputOpen(outputPtr, "w") == false)
    {
        fprintf(stderr, "Can't open file %s, exiting...\n", outputPtr->outputFile);
        exit(1);
    }
    saOutputWriteHeader(outputPtr);

    if (count != -1)
    {
        for(i=0; i<count; i++)
        {
            if (saOutputWrite(outputPtr, fieldArray) == false)
                exit(1);
            saFieldIncrementAll(fieldArray, outputPtr->outputClock);
        }
    }
    else if (stoptime != -1)
    {
        char *clockName = "time";
        k = saPropertiesMatch(p, "clock");
        if (k != -1)
            clockName = saPropertiesGetValue(p, k);
        saTimeFieldPtr tPtr = (saTimeFieldPtr)saFieldGetByName(fieldArray, clockName);
        if (tPtr == NULL)
        {
            fprintf(stderr, "Can't find clock field named '%s', exiting\n", clockName);
            exit(1);
        }
        while(stoptime > saTimeFieldGetValue(tPtr))
        {
            if (saOutputWrite(outputPtr, fieldArray) == false)
                exit(1);
            saFieldIncrementAll(fieldArray, outputPtr->outputClock);
        }
    }
    else if (realtime == true)
    {
        fprintf(stderr, "This feature is not yet implemented, exiting...\n");
        exit(1);
    }
    saOutputClose(outputPtr);
}
