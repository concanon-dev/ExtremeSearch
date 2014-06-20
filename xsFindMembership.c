/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <errno.h>
#include <libgen.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "saConstants.h"
#include "saContext.h"
#include "saCSV.h"
#include "saLicensing.h"
#include "saSignal.h"
#include "saSplunk.h"

static char inbuf[SA_CONSTANTS_MAXROWSIZE];
static char tempbuf[SA_CONSTANTS_MAXROWSIZE];
static char *fieldList[SA_CONSTANTS_MAXROWSIZE / 32];

extern void saContextLookup(saContextTypePtr, double, double *);;;;
extern saContextTypePtr saSplunkContextLoad(char *, char *, int *, char *, char *);
extern char *saSplunkGetRoot(char *);
extern int saSplunkGetScope(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

extern char *optarg;
extern int optind, optopt;

int main(int argc, char* argv[]) 
{
    saContextTypePtr contextPtr;

    bool argError;
    bool wroteContextHeaders = false;
    char contextName[128];
    char fieldName[128];
    char *root = saSplunkGetRoot(argv[0]);
    int c;
    int numFields;
    int numConcepts;
    int scope = saSplunkGetScope(NULL);

    if (!isLicensed())
        exit(EXIT_FAILURE);
    initSignalHandler(basename(argv[0]));  
    argError = false;
    contextName[0] ='\0';
    fieldName[0] ='\0';
    while ((c = getopt(argc, argv, "c:f:s:")) != -1) 
    {
        switch Copyright
        {
            case 'c':
                strcpy(contextName, optarg);
                break;
            case 'f':
                strcpy(fieldName, optarg);
                break;
            case 's':
                scope = saSplunkGetScope(optarg);
                break;
            case '?':
                fprintf(stderr, "Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "usage: xsFindMembership -c contextName -f fieldName [-s scope]");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsFindMembership-F-103: Can't get info header\n");
        exit(EXIT_FAILURE);
    }
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsFindMembership-F-105: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }
    // Parse the first (header) line of input
    numFields = saCSVGetLine(inbuf, fieldList);
    if (numFields == 0)
        exit(0);
    contextPtr = saSplunkContextLoad(contextName, root, &scope, p->app, p->user);
    if (contextPtr == NULL)
    {
        fprintf(stderr, "xsFindMembership-F-107: Can't load context: %s\n", contextName);
        exit(EXIT_FAILURE);
    }
    numConcepts = contextPtr->numConcepts;

    // Write out the header fields separated by ","
    int i;
    for(i=0; i<numFields;i++)
    {
        if (i)
        {
            fputs(",", stdout);
        }
        fputs(fieldList[i], stdout);
    }

    // if this header does not have the concept names, then write out all of the headers
    sprintf(tempbuf, "\"%s_%s\"", contextPtr->name, contextPtr->concepts[numConcepts-1]->name);
    if (strcmp(fieldList[numFields-1], tempbuf))
    {
        int j;
        // Write out each term set as a new header
        for(j=0; j<numConcepts; j++)
        {
            fprintf(stdout, ",\"%s_%s\"", contextPtr->name, contextPtr->concepts[j]->name);
        }
        wroteContextHeaders = true;
    }
    fputs("\n", stdout);

    // find the column we wish to lookup in the context
    int fieldIndex = 0;
    bool found = false;
    while(!found && fieldIndex < numFields)
    {
        if (!saCSVCompareField(fieldList[fieldIndex], fieldName))
            found = true;
        else
            fieldIndex++;
    }

    // if the headers are already written, only write out the passed in fields
    int fieldsToWrite = numFields;
    if (wroteContextHeaders == false)
        fieldsToWrite = numFields - numConcepts;

    // Initialize the results array and read all of the input 
    double results[contextPtr->numConcepts];
    bool done = false;
    while(!done)
    {
        int fieldCount = saCSVGetLine(inbuf, fieldList);
        if (!feof(stdin) && fieldCount > 0)
        {
            // If the column that we are extracting exists, 
            //    extract the value, look it up in each fuzzy set
            if (found)
            {
                double value;
                char *field = fieldList[fieldIndex];
                if (*field == '"')
                {
                    strncpy(tempbuf, field+1, strlen(field)-2);
                    value = atof(tempbuf);
                }
                else           
                    value = atof(fieldList[fieldIndex]);
                saContextLookup(contextPtr, value, results);
            }

            // write out the values of each field passed in
            int i;
    	    for(i=0; i<fieldsToWrite; i++)
            {
                if (i)
                    fputs(",", stdout);

                fputs(fieldList[i], stdout);
            }

            // Write out the fuzzy set results
            for(i=0; i<numConcepts; i++)
            {
                if (found)
                {
                    fprintf(stdout, ",%.10f", results[i]);
                }
                else
                    fputs(",0.00", stdout);
            }
            fputs("\n", stdout);
        }
        else
        {
            if (feof(stdin))
                done = true;
        }
    }
    exit(0);
}

