/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsCloneConcept

 Usage: xsCloneConcept -c concept -o oldContext [-O oldScope]  -n newContext [-N newScope]
    -N scope of new context
    -O scope of old context
    -c concept name
    -n new context
    -o old context

 Description:
    Clones a concept by copying it from one context to another.
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

#include "saSignal.h"
#include "saSplunk.h"

#define MAXROWSIZE (8192*11)

extern char *optarg;
extern int optind, optopt;

extern saContextTypePtr saSplunkContextLoad(char *, char *, int *, char *, char *);
extern bool saSplunkContextSave(saContextTypePtr, char *, int, char *, char *);
extern char *saSplunkGetRoot(char *);
extern int saSplunkGetScope(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    saContextTypePtr newContextPtr, oldContextPtr;
    bool argError;

    char conceptName[512];
    char newContextName[512];
    char oldContextName[512];
    char *root = saSplunkGetRoot(argv[0]);
    int i, c;
    int newScope = saSplunkGetScope(NULL);
    int oldScope = saSplunkGetScope(NULL);

    initSignalHandler(basename(argv[0]));
    argError = false;
    conceptName[0] = '\0';
    newContextName[0] ='\0';
    oldContextName[0] ='\0';
    while ((c = getopt(argc, argv, "N:O:c:n:o:")) != -1) 
    {
        switch(c)
        {
            case 'N':
                if ((newScope = saSplunkGetScope(optarg)) == SA_SPLUNK_SCOPE_UNKNOWN)
                {
                    fprintf(stderr, 
                            "xsCloneConcept-F-113: New Scope %s is not legal, try private, app, or global\n",
                            optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'O':
                if ((oldScope = saSplunkGetScope(optarg)) == SA_SPLUNK_SCOPE_UNKNOWN)
                {
                    fprintf(stderr, 
                            "xsCloneConcept-F-113: Old Scope %s is not legal, try private, app, or global\n",
                            optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'c':
                strcpy(conceptName, optarg);
                break;
            case 'n':
                strcpy(newContextName, optarg);
                break;
            case 'o':
                strcpy(oldContextName, optarg);
                break;
            case '?':
                fprintf(stderr, "xsCloneConcept-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsCloneConcept-F-103: Usage: xsCloneConcept -c concept -o oldContext [-O oldScope]  -n newContext [-N newScope]\n");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsCloneContext-F-105: Can't get info header\n");
        exit(EXIT_FAILURE);
    }

    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsCloneConcept-F-105: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    oldContextPtr = saSplunkContextLoad(oldContextName, root, &oldScope, p->app, p->user);
    if (oldContextPtr == NULL)
    {
        fprintf(stderr, "xsCloneConcept-F-107: Can't load context: %s\n", oldContextName);
        exit(EXIT_FAILURE);
    }

    if (newScope == SA_SPLUNK_SCOPE_NONE)
        newScope = oldScope;

    newContextPtr = saSplunkContextLoad(newContextName, root, &newScope, p->app, p->user);
    if (newContextPtr == NULL)
    {
        fprintf(stderr, "xsCloneConcept-F-109: Can't load context: %s\n", newContextName);
        exit(EXIT_FAILURE);
    }

    bool found = false;
    i = 0;
    while(!found && i<oldContextPtr->numConcepts)
    {
        if (!strcmp(conceptName, oldContextPtr->concepts[i]->name))
            found = true;
        else
            i++;
    }
    if (!found)
    {
        fprintf(stderr, 
                "xsCloneConcept-F-115: concept %s does not exist in context %s\n",
                conceptName, oldContextPtr->name);
        exit(0);
    }

    newContextPtr->concepts[newContextPtr->numConcepts++] = oldContextPtr->concepts[i];

    if (saSplunkContextSave(newContextPtr, root, newScope, p->app, p->user) == false)
    {
        fprintf(stderr, "xsCloneConcept-F-111: Can't save context: %s\n", newContextName);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Concept cloned successfully\n");
    exit(0);
}
