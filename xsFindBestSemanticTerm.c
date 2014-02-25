/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
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

#define BESTCIX "BestCIX"
#define BESTTERM "BestSemanticTerm"

static char inbuf[SA_CONSTANTS_MAXROWSIZE];
static char tempbuf[SA_CONSTANTS_MAXROWSIZE];
static char *fieldList[SA_CONSTANTS_MAXROWSIZE / 32];

extern void saContextLookup(saContextTypePtr, double, double *);;;;
extern saContextTypePtr saSplunkContextLoad(char *, int *, char *, char *);
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
    int c;
    int numFields;
    int numSemanticTerms;
    int scope = saSplunkGetScope(NULL);

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));  
    argError = false;
    contextName[0] ='\0';
    fieldName[0] ='\0';
    while ((c = getopt(argc, argv, "c:f:s:")) != -1) 
    {
        switch (c)
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
        fprintf(stderr, "usage: xsFindBestSemanticTerm -c contextName -f fieldName [-s scope]");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsFindBestSemanticTerm-F-103: Can't get info header\n");
        exit(EXIT_FAILURE);
    }

    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsFindBestSemanticTerm-F-105: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    contextPtr = saSplunkContextLoad(contextName, &scope, p->app, p->user);
    if (contextPtr == NULL)
    {
        fprintf(stderr, "xsFindBestSemanticTerm-F-107: Can't load context: %s\n", contextName);
        exit(EXIT_FAILURE);
    }

    // Parse the first (header) line of input
    numFields = saCSVGetLine(inbuf, fieldList);
    numSemanticTerms = contextPtr->numSemanticTerms;

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

    sprintf(tempbuf, "\"%s\"", BESTCIX);
    if (strcmp(fieldList[numFields-1], tempbuf))
    {
        fprintf(stdout, ",\"%s\",\"%s\"", BESTTERM, BESTCIX);
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
        fieldsToWrite = numFields - 2;

    // Initialize the results array and read all of the input 
    double results[contextPtr->numSemanticTerms];
    bool done = false;
    float bestMatchValue;
    int bestMatch;
    while(!done)
    {
        saCSVGetLine(inbuf, fieldList);
        if (!feof(stdin))
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
                bestMatch = 0;
                bestMatchValue = -2.0;
                int k;
                for(k=0; k<contextPtr->numSemanticTerms; k++)
                    if (results[k] > bestMatchValue)
                    {
                        bestMatchValue = results[k];
                        bestMatch = k;
                    }
            }

            // write out the values of each field passed in
            int i;
    	    for(i=0; i<fieldsToWrite; i++)
            {
                if (i)
                    fputs(",", stdout);

                fputs(fieldList[i], stdout);
            }

            if (found)
                fprintf(stdout, ",%s,%.10f", contextPtr->semanticTerms[bestMatch]->name, results[bestMatch]);
            else
                fputs(",\"\",0.00", stdout);
            fputs("\n", stdout);
        }
        else
            done = true;
    }
    exit(0);
}

