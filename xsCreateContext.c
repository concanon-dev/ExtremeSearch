/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <libgen.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "saContext.h"
#include "saSplunk.h"

static double stdDev(double, double);

extern saContextTypePtr saContextCreateAvgCentered(char *, double, double, char *[], int, char *,
                                                   char *, int, char *);
extern saContextTypePtr saContextCreateDomain(char *, double, double, char *[], int, char *,
                                              char *, int, char *);

extern saSplunkInfoPtr saSplunkLoadInfo(char *);

extern char *optarg;
extern int optind, optopt;

int main(int argc, char* argv[]) 
{
    bool argError;
    bool readInput = false;
    bool setAvg = false;
    bool setCount = false;
    bool setMax = false;
    bool setMin = false;
    bool setSDev = false;

    char endShapeStr[256];
    char *semanticTermNames[SA_CONTEXT_MAXTERMS];
    char contextType[256];
    char infoFile[256];
    char name[256];
    char notes[1024];
    char shapeStr[256];
    char *termsList = NULL;
    double avg = 0.0;
    double max = 0.0;
    double min = 0.0;
    double sdev = 0.0;
    double width = 2.0;
    int c;
    int count;
    int fileScope = SA_SPLUNK_SCOPE_GLOBAL;
    int numSemanticTerms = 0;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    strcpy(contextType, SA_CONTEXT_TYPE_DOMAIN);
    strcpy(endShapeStr, SA_SEMANTICTERM_SHAPE_CURVE);
    strcpy(shapeStr, SA_SEMANTICTERM_SHAPE_PI); 
    infoFile[0] = '\0';
    while ((c = getopt(argc, argv, "a:c:d:e:f:i:m:n:o:p:t:w:x:z:")) != -1) 
    {
        switch (c)
        {
            case 'a':
                avg = atof(optarg);
                setAvg = true;
                break;
	    case 'c':
                count = atoi(optarg);
		setCount = true;
		break;
            case 'd':
                sdev = atof(optarg);
                setSDev = true;
                break;
            case 'e':
                strcpy(endShapeStr, optarg);
                break;
            case 'f':
                fileScope = atoi(optarg);
                break;
            case 'i':
                strcpy(infoFile, optarg);
                break;
            case 'm':
                min = atof(optarg);
                setMin = true;
                break;
            case 'n':
                strcpy(name, optarg);
                break;
            case 'o':
                strcpy(notes, optarg);
                break;
            case 'p':
                strcpy(shapeStr, optarg);
                break;
            case 't':
                termsList = malloc(strlen(optarg)+1);
                strcpy(termsList, optarg);
                break;
            case 'w':
                width = atof(optarg);
                break;
            case 'x':
                max = atof(optarg);
                setMax = true;
                break;
            case 'z':
                strcpy(contextType, optarg);
                break;
            case '?':
                fprintf(stderr, "xsCreateContext-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, 
                "xsCreateContext-F-103: Usage: xsCreateContext [-a avg] [-c count] [-d stdev] [-e endShape] [-m min] [-p shape] [-w width] [-x max] [-z contextType] -n contextName -t \"term1,term2,term3...\" ");
        exit(EXIT_FAILURE);
    }
  
    if (!setAvg)
        avg = (min + max) / (double)2.0;

    if (!setSDev)
        sdev = stdDev(min, max);

    // 
    if (termsList == NULL)
    {
        fprintf(stderr, "xsCreateContext-F-117: no semantic terms specified\n");
        exit(0);
    }

    if (*termsList == '"')
        termsList++;
    if (termsList[strlen(termsList)-1] == '"')
        termsList[strlen(termsList)-1] = '\0';
    char *s = strtok(termsList, ",");
    while(s != NULL)
    {
          semanticTermNames[numSemanticTerms++] = s;
          s = strtok(NULL, ",");
    }

    saContextTypePtr cPtr = NULL;

    // Go out and create the context based on type
    if (!strcmp(contextType, SA_CONTEXT_TYPE_AVERAGE_CENTERED))
        cPtr = saContextCreateAvgCentered(name, avg, sdev, semanticTermNames, numSemanticTerms,
                                          shapeStr, endShapeStr, count, notes);
    else if (!strcmp(contextType, SA_CONTEXT_TYPE_DOMAIN))
        cPtr = saContextCreateDomain(name, min, max, semanticTermNames, numSemanticTerms, shapeStr,
                                     endShapeStr, count, notes);
    if (cPtr == NULL)
    {
        fprintf(stderr, "xsCreateContext-F-111: Failed to create a context of type %s\n",
                contextType);
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr iPtr = saSplunkLoadInfo(infoFile);
    if (iPtr == NULL)
    {
        fprintf(stderr, "xsCreateContext-F-115: Can't open info file %s\n", infoFile);
        exit(EXIT_FAILURE);
    }
    saSplunkContextSave(cPtr, fileScope, iPtr->app, iPtr->user);
    saContextDisplay(cPtr);
    exit(0);
}

double stdDev(double min, double max)
{
    double sum = 0;
    double mean = (min+max)/(double)2.0;
    int i;

    for (i = min; i < max; i++)
        sum += pow(i - mean, 2);

    return sqrt(sum / max-min+1);
}
