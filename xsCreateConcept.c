/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsCreateConcept

 Usage: xsCreateConcept -c contextName -m min -n name -p shape -s scope -x max
    -c name of the context 
    -m min value for the concept
    -n name of the concept
    -p shape of the concept
    -s scope of the context
    -x max value for the concept

 Description:
    Creates a Concept for an existing Context with specified parameters.  
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
#include "saConcept.h"
#include "saSignal.h"
#include "saSplunk.h"

static double stdDev(double, double);

extern saContextTypePtr saContextInit(char *, double, double, double, double, int, int, char *, char *, char *);
extern void saContextDisplay(saContextTypePtr);
extern saContextTypePtr saSplunkContextLoad(char *, char *, int *, char *, char *);
extern void saContextCreateConcept(saContextTypePtr, char *, char *, double, double);
extern void saContextRecreateConcepts(saContextTypePtr, double, double);
extern bool saSplunkContextSave(saContextTypePtr, char *, int, char *, char *);
extern char *saSplunkGetRoot(char *);

extern saSplunkInfoPtr saSplunkLoadHeader();
extern saSplunkInfoPtr saSplunkLoadInfo(char *);
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

extern char *optarg;
extern int optind, optopt;

int main(int argc, char* argv[]) 
{
    bool argError;
    bool readInput = false;
    bool setMax = false;
    bool setMin = false;
    char contextName[256];
    char *root = saSplunkGetRoot(argv[0]);
    char shapeStr[256];
    char termName[256];
    double max = 0.0;
    double min = 0.0;
    int c;
    int scope = SA_SPLUNK_SCOPE_NONE;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    contextName[0] = '\0';
    termName[0] = '\0';
    strcpy(shapeStr, SA_CONCEPT_SHAPE_PI); 
    while ((c = getopt(argc, argv, "c:m:n:p:s:x:")) != -1) 
    {
        switch (c)
        {
            case 'c':
                strcpy(contextName, optarg);
                break;
            case 'm':
                min = atof(optarg);
                setMin = true;
                break;
            case 'n':
                strcpy(termName, optarg);
                break;
            case 'p':
                strcpy(shapeStr, optarg);
                break;
            case 's':
                scope = atoi(optarg);
                break;
            case 'x':
                max = atof(optarg);
                setMax = true;
                break;
            case '?':
                fprintf(stderr, "xsCreateConcept-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, 
                "xsCreateConcept-F-103: Usage: xsCreateConcept -c contextName -m min -n name -p shape -s scope -x max");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsCreateConcept-F-105: Can't get info header\n");
        exit(EXIT_FAILURE);
    }
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsCreateConcept-F-107: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    saContextTypePtr cPtr = saSplunkContextLoad(contextName, root, &scope, p->app, p->user);
    if (cPtr == NULL)
    {
        cPtr = saContextInit(contextName, min, max, (double)0.0, (double)0.0, 0, 1,
                             SA_CONTEXT_TYPE_DOMAIN, SA_CONTEXT_NOTES_SINGLE_TERM, "");
    }

    // make sure the min is < the max
    if (min >= max)
    {
        fprintf(stderr, "xsCreateConcept-F-111: min must be less than max\n");
        exit(EXIT_FAILURE);
    }

    // if the new conceit changes the domain (lower min or higher max)
    //    recalculate all of the concepts over the new domain
    if (cPtr->domainMin > min || cPtr->domainMax < max)
        saContextRecreateConcepts(cPtr, min, max);
    saContextCreateConcept(cPtr, termName, shapeStr, min, max);
    if (saSplunkContextSave(cPtr, root, scope, p->app, p->user) == false)
    {
        fprintf(stderr, "xsCreateConcept-F-109: Can't save Context: %s\n", contextName);
        exit(EXIT_FAILURE);
    }
    saContextDisplay(cPtr);
    exit(0);
}

