/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
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

static char headerbuf[SA_CONSTANTS_MAXSTRING];
static char inbuf[SA_CONSTANTS_MAXSTRING];

extern char *optarg;
extern int optind, optopt;

extern int compareField(char *, char []);
extern void saContextDisplayWithHeader(saContextTypePtr, char *);
extern saContextTypePtr saContextLoad(FILE *);
extern FILE *saOpenFile(char *, char *);

int main(int argc, char* argv[]) 
{
    saContextTypePtr termSetPtr;
    bool argError;
    char countName[128];
    char *fieldList[128];
    char header[512];
    char *headerList[128];
    char keyName[128];
    char termSetFileName[128];
    double *countRow;
    double *keyRow;
    double maxCount;
    double minCount;
    int c;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    termSetFileName[0] ='\0';
    while ((c = getopt(argc, argv, "c:f:k:")) != -1) 
    {
        switch (c)
        {
            case 'c':
                strcpy(countName, optarg);
                break;
            case 'f':
	        strcpy(termSetFileName, optarg);
                break;
            case 'k':
                strcpy(keyName, optarg);
                break;
            case '?':
                fprintf(stderr, "xsOverlayContext-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, 
                "xsOverlayContext-F-103: Usage: xsOverlayContext -c countName -f contextFileName -k keyName");
        exit(EXIT_FAILURE);
    }

    FILE *f = saOpenFile(termSetFileName, "r");
    if (f == NULL)
    {
        fprintf(stderr, "xsOverlayContext-F-111: can't open context %s, errno=%d\n",
                termSetFileName, errno);
        exit(0);
    }
    termSetPtr = saContextLoad(f);
    fclose(f);

    int numHeaderFields = saCSVGetLine(headerbuf, headerList);
    int i;
    int countIndex = -1;
    int keyIndex = -1;
    for(i=0; i<numHeaderFields; i++)
        if (!compareField(headerList[i], countName))
            countIndex = i;
        else if (!compareField(headerList[i], keyName))
            keyIndex = i;
    if (countIndex == -1)
    {
        fprintf(stderr, "xsOverlayContext-F-113: Count field %s not found\n", countName);
        exit(0);
    }

    if (keyIndex == -1)
    {
        fprintf(stderr, "xsOverlayContext-F-115: Key field %s not found\n", keyName);
        exit(0);
    }

    countRow = malloc(sizeof(double)*SA_CONSTANTS_MAXNUMROWS);
    keyRow = malloc(sizeof(double)*SA_CONSTANTS_MAXNUMROWS);

    fprintf(stdout, "%s,%s,", keyName, countName);
    for(i=0; i<termSetPtr->numSemanticTerms; i++)
        fprintf(stdout, ",%s", termSetPtr->semanticTerms[i]->name);
    fputs("\n", stdout);

    int j;
    for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
    {
        fprintf(stdout, "%1.5f,", termSetPtr->semanticTerms[0]->indexVector[i]);
        for(j=0; j<termSetPtr->numSemanticTerms; j++)
            fprintf(stdout, ",%1.10f", termSetPtr->semanticTerms[j]->vector[i]);
        fputs("\n", stdout);
    }

    int numRows = 0;
    bool done = false;
    while(done == false)
    {
        int zzz = saCSVGetLine(inbuf, fieldList);
        if (!feof(stdin))
        {
            countRow[numRows] = atof(fieldList[countIndex]);
            keyRow[numRows] = atof(fieldList[keyIndex]);
            if (numRows == 0)
            {
                minCount = countRow[0];
                maxCount = countRow[0];
            }
            else
            {
                if (countRow[numRows] < minCount)
                    minCount = countRow[numRows];
                else if (countRow[numRows] > maxCount)
                    maxCount = countRow[numRows];
            }
            numRows++;
        }
        else
            done = true;
    }

    for(i=0; i<numRows; i++)
    {
        fprintf(stdout, "%.6f,%.6f", keyRow[i], (countRow[i] - minCount) / (maxCount - minCount));
        for(j=0; j<termSetPtr->numSemanticTerms; j++)
            fputs(",", stdout);
        fputs("\n", stdout);
    }
    exit(0);
}
