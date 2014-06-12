/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
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
#include "saLicensing.h"
#include "saSignal.h"
#include "saSplunk.h"

static double stdDev(double, double);

extern saContextTypePtr saContextCreateAvgCentered(char *, double, double, char *[], int, char *,
                                                   char *, int, char *, char *);
extern saContextTypePtr saContextCreateDomain(char *, double, double, char *[], int, char *,
                                              char *, int, char *, char *);
extern void saContextDisplay(saContextTypePtr);
extern saContextTypePtr saContextMerge(saContextTypePtr, saContextTypePtr, char *);
extern saContextTypePtr saSplunkContextLoad(char *, int *, char *, char *);
extern bool saSplunkContextSave(saContextTypePtr, int, char *, char *);
extern int saSplunkGetScope(char *);
extern saSplunkInfoPtr saSplunkLoadInfo(char *);

extern char *optarg;
extern int optind, optopt;

int main(int argc, char* argv[]) 
{
    saContextTypePtr cPtr = NULL;
    saContextTypePtr origCPtr = NULL;
    bool argError;
    bool readInput = false;
    bool setAvg = false;
    bool setCount = false;
    bool setMax = false;
    bool setMin = false;
    bool setSDev = false;
    bool update = false;

    char endShapeStr[256];
    char *conceptNames[SA_CONTEXT_MAXTERMS];
    char contextType[256];
    char infoFile[256];
    char name[256];
    char notes[1024];
    char shapeStr[256];
    char *conceptsList = NULL;
    char uom[256];

    double avg = 0.0;
    double max = 0.0;
    double min = 0.0;
    double sdev = 0.0;
    double width = 2.0;

    int c;
    int count;
    int fileScope = SA_SPLUNK_SCOPE_GLOBAL;
    int numConcepts = 0;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    strcpy(contextType, SA_CONTEXT_TYPE_DOMAIN);
    strcpy(endShapeStr, SA_CONCEPT_SHAPE_CURVE);
    strcpy(shapeStr, SA_CONCEPT_SHAPE_PI); 
    infoFile[0] = '\0';
    uom[0] = '\0';
    while ((c = getopt(argc, argv, "Ua:c:d:e:f:i:m:n:o:p:t:u:w:x:z:")) != -1) 
    {
        switch (c)
        {
            case 'U':
                update = true;
                break;
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
                fileScope = saSplunkGetScope(optarg);
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
                conceptsList = malloc(strlen(optarg)+1);
                strcpy(conceptsList, optarg);
                break;
            case 'u':
                strcpy(uom, optarg);
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
                "xsCreateContext-F-103: Usage: xsCreateContext [-a avg] [-c count] [-d stdev] [-e endShape] [-f fileScope ] [-i infoFile] [-m min] [-p shape] [-t conceptlist] [-u uom] [-w width] [-x max] [-z contextType] -n contextName -t \"term1,term2,term3...\" ");
        exit(EXIT_FAILURE);
    }

    // get scope (app, user) 
    saSplunkInfoPtr iPtr = saSplunkLoadInfo(infoFile);
    if (iPtr == NULL)
    {
        fprintf(stderr, "xsCreateContext-F-115: Can't open info file %s\n", infoFile);
        exit(EXIT_FAILURE);
    }

    // if update flag is set, try to load original context
    if (update == true) 
    {
        if ((origCPtr = saSplunkContextLoad(name, &fileScope, iPtr->app, iPtr->user)) == NULL)
        {
            fprintf(stderr, "xsCreateContext-F-121: Can't load context %s\n", name);
            exit(EXIT_FAILURE);
        }
    }

    if (min >= max)
    {
        fprintf(stderr, "xsCreateContext-F-125: min must be less than max\n");
        exit(0);
    }
    if (!setAvg)
        avg = (min + max) / (double)2.0;

    if (!setSDev)
        sdev = stdDev(min, max);

    if (conceptsList == NULL)
    {
        fprintf(stderr, "xsCreateContext-F-117: no concepts specified\n");
        exit(0);
    }

    if (*conceptsList == '"')
        conceptsList++;
    if (conceptsList[strlen(conceptsList)-1] == '"')
        conceptsList[strlen(conceptsList)-1] = '\0';
    char *s = strtok(conceptsList, ",");
    while(s != NULL)
    {
          while(*s == ' ' && *s != '\0')
                s++;
          if (*s == '\0')
          {
              fprintf(stderr, "xsCreateContext-F-119: concept can't be blank\n");
              exit(0);
          }
          conceptNames[numConcepts++] = s;
          s = strtok(NULL, ",");
    }

    // Go out and create the context based on type
    if (!strcmp(contextType, SA_CONTEXT_TYPE_AVERAGE_CENTERED))
    {
        if (sdev == 0)
        {
            fprintf(stderr, "xsCreateContext-F-113: For an average_centered context, sdev must be > 0\n");
            exit(EXIT_FAILURE);
        }
        cPtr = saContextCreateAvgCentered(name, avg, sdev, conceptNames, numConcepts,
                                          shapeStr, endShapeStr, count, notes, uom);
    }
    else if (!strcmp(contextType, SA_CONTEXT_TYPE_DOMAIN))
        cPtr = saContextCreateDomain(name, min, max, conceptNames, numConcepts, shapeStr,
                                     endShapeStr, count, notes, uom);
    if (cPtr == NULL)
    {
        fprintf(stderr, "xsCreateContext-F-111: Failed to create a context of type %s\n",
                contextType);
        exit(EXIT_FAILURE);
    }

    if (update == true)
    {
        if ((cPtr = saContextMerge(origCPtr, cPtr, origCPtr->name)) == NULL)
        {
            fprintf(stderr, "xsCreateContext-F-123: Failed to merge contexts\n");
            exit(EXIT_FAILURE);
        }
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
