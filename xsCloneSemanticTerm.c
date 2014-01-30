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
#include "saLicensing.h"
#include "saSignal.h"
#include "saSplunk.h"

#define MAXROWSIZE (8192*11)

extern char *optarg;
extern int optind, optopt;

extern saContextTypePtr saSplunkContextLoad(char *, int *, char *, char *);
extern bool saSplunkContextSave(saContextTypePtr, int, char *, char *);
extern int saSplunkGetScope(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    saContextTypePtr newContextPtr, oldContextPtr;
    bool argError;

    char newContextName[512];
    char oldContextName[512];
    char semanticTermName[512];
    int i, c;
    int newScope = saSplunkGetScope(NULL);
    int oldScope = saSplunkGetScope(NULL);

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    semanticTermName[0] = '\0';
    newContextName[0] ='\0';
    oldContextName[0] ='\0';
    while ((c = getopt(argc, argv, "f:n:o:t:")) != -1) 
    {
        switch (c)
        {
            case 'N':
                newScope = saSplunkGetScope(optarg);
                break;
            case 'O':
                oldScope = saSplunkGetScope(optarg);
                break;
            case 'n':
                strcpy(newContextName, optarg);
                break;
            case 'o':
                strcpy(oldContextName, optarg);
                break;
            case 's':
                strcpy(semanticTermName, optarg);
                break;
            case '?':
                fprintf(stderr, "xsCloneSemanticTerm-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsCloneSemanticTerm-F-103: Usage: xsCloneSemanticTerm semanticTerm FROM oldContext [ IN scope ] TO newContext [ IN scope ]\n");
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
        fprintf(stderr, "xsCloneSemanticTerm-F-105: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    oldContextPtr = saSplunkContextLoad(oldContextName, &oldScope, p->app, p->user);
    if (oldContextPtr == NULL)
    {
        fprintf(stderr, "xsCloneSemanticTerm-F-107: Can't load context: %s\n", oldContextName);
        exit(EXIT_FAILURE);
    }

    if (newScope == SA_SPLUNK_SCOPE_NONE)
        newScope = oldScope;

    newContextPtr = saSplunkContextLoad(newContextName, &newScope, p->app, p->user);
    if (newContextPtr == NULL)
    {
        fprintf(stderr, "xsCloneSemanticTerm-F-109: Can't load context: %s\n", newContextName);
        exit(EXIT_FAILURE);
    }

    bool found = false;
    i = 0;
    while(!found && i<oldContextPtr->numSemanticTerms)
    {
        if (!strcmp(semanticTermName, oldContextPtr->semanticTerms[i]->name))
            found = true;
        else
            i++;
    }
    if (!found)
    {
        fprintf(stderr, 
                "xsCloneSemanticTerm-F-115: semantic term %s does not exist in context %s\n",
                semanticTermName, oldContextPtr->name);
        exit(0);
    }

    newContextPtr->semanticTerms[newContextPtr->numSemanticTerms++] = oldContextPtr->semanticTerms[i];

    if (saSplunkContextSave(newContextPtr, newScope, p->app, p->user) == false)
    {
        fprintf(stderr, "xsCloneSemanticTerm-F-111: Can't save context: %s\n", newContextName);
        exit(EXIT_FAILURE);
    }
    exit(0);
}
