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
#include "saSemanticTerm.h"
#include "saSignal.h"
#include "saSplunk.h"

static double stdDev(double, double);

extern saContextTypePtr saContextInit(char *, double, double, double, double, int, int, char *, char *, char *);
extern void saContextDisplay(saContextTypePtr);
extern saContextTypePtr saSplunkContextLoad(char *, int *, char *, char *);
extern void saContextRecreateSemanticTerms(saContextTypePtr, double, double);
extern bool saSplunkContextSave(saContextTypePtr, int, char *, char *);
extern void saContextCreateSemanticTerm(saContextTypePtr, char *, char *, double, double);

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
    char termName[256];
    char shapeStr[256];
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
    strcpy(shapeStr, SA_SEMANTICTERM_SHAPE_PI); 
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
                fprintf(stderr, "xsCreateSemanticTerm-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, 
                "xsCreateSemanticTerm-F-103: Usage: xsCreateSemanticTerm -c contextName -m min -n name -p shape -s scope -x max");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsCreateSemanticTerm-F-105: Can't get info header\n");
        exit(EXIT_FAILURE);
    }
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsCreateSemanticTerm-F-107: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    saContextTypePtr cPtr = saSplunkContextLoad(contextName, &scope, p->app, p->user);
    if (cPtr == NULL)
    {
        cPtr = saContextInit(contextName, min, max, (double)0.0, (double)0.0, 0, 1,
                             SA_CONTEXT_TYPE_DOMAIN, SA_CONTEXT_NOTES_SINGLE_TERM, "");
    }

    // if the new semantic term changes the domain (lower min or higher max)
    //    recalculate all of the semantic terms over the new domain
    if (cPtr->domainMin > min || cPtr->domainMax < max)
        saContextRecreateSemanticTerms(cPtr, min, max);
    saContextCreateSemanticTerm(cPtr, termName, shapeStr, min, max);
    if (saSplunkContextSave(cPtr, scope, p->app, p->user) == false)
    {
        fprintf(stderr, "xsCreateSemanticTerm-F-109: Can't save Context: %s\n", contextName);
        exit(EXIT_FAILURE);
    }
    saContextDisplay(cPtr);
    exit(0);
}

