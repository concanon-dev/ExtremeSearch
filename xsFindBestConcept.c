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
#include "saHash.h"
#include "saLicensing.h"
#include "saSignal.h"
#include "saSplunk.h"

#define BESTCIX "BestCIX"
#define BESTTERM "BestConcept"
#define UOM "UOM"

static saHashtableTypePtr contextTable;

static char headerbuf[SA_CONSTANTS_MAXROWSIZE];
static char inbuf[SA_CONSTANTS_MAXROWSIZE];
static char tempbuf[SA_CONSTANTS_MAXROWSIZE];
static char *fieldList[SA_CONSTANTS_MAXROWSIZE / 32];
static char *headerList[SA_CONSTANTS_MAXROWSIZE / 32];

extern saHashtableTypePtr saHashCreateDefault();
extern void *saHashGet(saHashtableTypePtr, char *);
extern void saHashSet(saHashtableTypePtr, char *, char *);

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
    int numHeaderFields;
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
        fprintf(stderr, "usage: xsFindBestConcept -c contextName -f fieldName [-s scope]");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsFindBestConcept-F-103: Can't get info header\n");
        exit(EXIT_FAILURE);
    }

    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsFindBestConcept-F-105: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    // Parse the first (header) line of input
    if ((numHeaderFields = saCSVGetLine(headerbuf, headerList)) == 0)
       exit(0);

    // Write out the header fields separated by ","
    int i;
    for(i=0; i<numHeaderFields;i++)
    {
        if (i)
        {
            fputs(",", stdout);
        }
        fputs(headerList[i], stdout);
    }

    sprintf(tempbuf, "\"%s\"", UOM);
    if (strcmp(headerList[numHeaderFields-1], tempbuf))
    {
        fprintf(stdout, ",\"%s\",\"%s\",\"%s\"", BESTTERM, BESTCIX, UOM);
        wroteContextHeaders = true;
    }
    fputs("\n", stdout);

    // find the column we wish to lookup in the context
    int fieldIndex = 0;
    bool found = false;
    while(!found && fieldIndex < numHeaderFields)
    {
        if (!saCSVCompareField(headerList[fieldIndex], fieldName))
            found = true;
        else
            fieldIndex++;
    }

    // if the headers are already written, only write out the passed in fields
    int fieldsToWrite = numHeaderFields;
    if (wroteContextHeaders == false)
        fieldsToWrite = numHeaderFields - 3;

    contextTable = saHashCreateDefault();

    // Initialize the results array and read all of the input 
    bool done = false;
    char realContextName[1024];
    double results[64];
    float bestMatchValue;
    int bestMatch;
    strcpy(realContextName, contextName);
    while(!done)
    {
        saCSVGetLine(inbuf, fieldList);
        if (!feof(stdin))
        {
            // Find the context in the cache or load it from the file
            contextPtr = (saContextTypePtr)saHashGet(contextTable, realContextName);
            if (contextPtr == NULL)
            {
                contextPtr = saSplunkContextLoad(realContextName, &scope, p->app, p->user);
                if (contextPtr == NULL)
                {
                    int q;
                    found = false;
                    q = 0;

                    while(found == false && q < numHeaderFields)
                    {
                        if (!saCSVCompareField(headerList[q], contextName))
                        {
                            sprintf(realContextName, "%s%s", saCSVExtractField(fieldList[q]), headerList[fieldIndex]);
                            found = true;
                        }
                        else
                            q++;
                    }
                    if (found == false)
                    {
                        fprintf(stderr, "xsFindBestConcept-F-109: Can't find field for context: %s\n", contextName);
                        exit(EXIT_FAILURE);
                    }
                    contextPtr = (saContextTypePtr)saHashGet(contextTable, realContextName);
                    if (contextPtr == NULL)
                    {
                        contextPtr = saSplunkContextLoad(realContextName, &scope, p->app, p->user);
                        if (contextPtr == NULL)
                        {
                            fprintf(stderr, "xsFindBestConcept-F-107: Can't load context: %s\n", contextName);
                            exit(EXIT_FAILURE);
                        }
                    }
                }
                saHashSet(contextTable, realContextName, (void *)contextPtr);
            }
                
            numConcepts = contextPtr->numConcepts;

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
                for(k=0; k<contextPtr->numConcepts; k++)
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
                fprintf(stdout, ",\"%s\",%.10f,\"%s\"", contextPtr->concepts[bestMatch]->name, results[bestMatch],
                        contextPtr->uom);
            else
                fputs(",\"\",0.00,\"\"", stdout);
            fputs("\n", stdout);
        }
        else
            done = true;
    }
    exit(0);
}

