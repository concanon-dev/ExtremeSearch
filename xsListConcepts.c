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
#include "saContext.h"
#include "saHedge.h"
#include "saLicensing.h"
#include "saSignal.h"
#include "saSplunk.h"
#include "saUtils.h"

#define MAXROWSIZE (8192*11)

extern char *optarg;
extern int optind, optopt;

extern saContextTypePtr saSplunkContextLoad(char *, int *, char *, char *);
extern int saSplunkGetScope(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

FILE *saOpenFile(char *, char *);

int main(int argc, char* argv[]) 
{
    saContextTypePtr contextPtr;
    bool argError;
    char contextString[128];
    char scopeString[256];
    int i, c;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    contextString[0] ='\0';
    strcpy(scopeString, "none");
    while ((c = getopt(argc, argv, "c:p:")) != -1) 
    {
        switch (c)
        {

            case 'c':
                strcpy(contextString, optarg);
                break;
            case 'p':
                strcpy(scopeString, optarg);
                break;
            case '?':
                fprintf(stderr, "xsListConcepts-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsListConcepts-F-103: Usage: xsListConcepts -c context -p scope");
        exit(EXIT_FAILURE);
    }

    // get the header information
    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsListConcepts-F-123: can't get header\n");
        exit(0);
    }

    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsListConcepts-F-125: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    int scope = saSplunkGetScope(scopeString);
    contextPtr = saSplunkContextLoad(contextString, &scope, p->app, p->user);
    if (contextPtr == NULL)
    {
        fprintf(stderr, "xsListConcepts-F-111: can't open context %s\n", contextString);
        exit(0);
    }
    fputs("\"Concept\"\n", stdout);
    for(i=0; i<contextPtr->numConcepts; i++)
    {
          fprintf(stdout, "\"%s\"\n", contextPtr->concepts[i]->name);
    }
    exit(0);
}