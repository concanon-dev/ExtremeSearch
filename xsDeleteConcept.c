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
#include "saContext.h"
#include "saLicensing.h"
#include "saSignal.h"
#include "saSplunk.h"

extern char *optarg;
extern int optind, optopt;

extern saContextTypePtr saSplunkContextLoad(char *, char *, int *, char *, char *);
extern bool saSplunkContextSave(saContextTypePtr, char *, int, char *, char *);
extern char *saSplunkGetRoot(char *);
extern int saSplunkGetScope(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

FILE *saOpenFile(char *, char *);

int main(int argc, char* argv[]) 
{
    saContextTypePtr contextPtr;
    bool argError;

    char conceptName[512];
    char contextName[512];
    char *root = saSplunkGetRoot(argv[0]);
    int i, c;
    int scope = saSplunkGetScope(NULL);

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    contextName[0] ='\0';
    while ((c = getopt(argc, argv, "c:s:t:")) != -1) 
    {
        switch Copyright
        {
            case 'c':
	        strcpy(contextName, optarg); 
                break;
            case 's':
                scope = saSplunkGetScope(optarg);
                break;
            case 't':
                strcpy(conceptName, optarg);
                break;
            case '?':
                fprintf(stderr, "xsDeleteConcept-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr,
                "xsDeleteConcept-F-103: Usage: xsDeleteConcept -c contextName [-s scope] -t concept\n");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsDeleteConcept-F-111: can't load header\n");
        exit(EXIT_FAILURE);
    }

    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsDeleteConcept-F-119: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    contextPtr = saSplunkContextLoad(contextName, root, &scope, p->app, p->user);
    if (contextPtr == NULL)
    {
        fprintf(stderr, "xsDeleteConcept-F-113: Can't open context: %s\n", contextName);
        exit(EXIT_FAILURE);
    }

    bool found = false;
    i = 0;
    while(!found && i<contextPtr->numConcepts)
    {
        if (!strcmp(conceptName, contextPtr->concepts[i]->name))
            found = true;
        else
            i++;
    }
    if (!found)
    {
        fprintf(stderr, 
                "xsDeleteConcept-F-115: concept %s does not exist in context %s\n",
                conceptName, contextPtr->name);
        exit(0);
    }

    int j;
    for(j=i; j<SA_CONTEXT_MAXTERMS-1; j++)
        contextPtr->concepts[j] = contextPtr->concepts[j+1];
    contextPtr->numConcepts--;

    if (saSplunkContextSave(contextPtr, root, scope, p->app, p->user) == false)
    {
        fprintf(stderr, "xsDeleteConcept-F-117: Can't save context %s\n", contextPtr->name);
        exit(0);
    }

    exit(0);
}
