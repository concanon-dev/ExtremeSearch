/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsCloneContext

 Usage: xsCloneContext -o oldContext [-O oldScope] -n newContext [-N newScope]
    -N scope of new context
    -O scope of old context
    -n new context
    -o old context

 Description:
    Makes a full copy of a Context, including all concepts.
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

int main(int argc, char* argv[]) 
{
    saContextTypePtr contextPtr;
    bool argError;
    char newName[256];
    char oldName[256];
    char *root = saSplunkGetRoot(argv[0]);
    int c;
    int newScope = saSplunkGetScope(NULL);
    int oldScope = saSplunkGetScope(NULL);

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    newName[0] ='\0';
    oldName[0] ='\0';
    while ((c = getopt(argc, argv, "N:O:n:o:")) != -1) 
    {
        switch (c)
        {
            case 'n':
	        strcpy(newName, optarg);
                break;
            case 'N':
                newScope = saSplunkGetScope(optarg);
                break;
            case 'o':
                strcpy(oldName, optarg);
                break;
            case 'O':
                oldScope = saSplunkGetScope(optarg);
                break;
            case '?':
                fprintf(stderr, "xsCloneContext-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsCloneContext-F-103: Usage: xsCloneContext -N newScope -O oldScope -n newContext -o oldContext");
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
        fprintf(stderr, "xsCloneContext-F-107: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    contextPtr = saSplunkContextLoad(oldName, root, &oldScope, p->app, p->user);
    if (contextPtr == NULL)
    {
        fprintf(stderr, "xsCloneContext-F-109: Can't load context: %s\n", oldName);
        exit(EXIT_FAILURE);
    }

    strcpy(contextPtr->name, newName);
    if (saSplunkContextSave(contextPtr, root, newScope, p->app, p->user) == false)
    {
        fprintf(stderr, "xsCloneContext-F-111: Can't save context: %s\n", newName);
        exit(EXIT_FAILURE);
    }
    exit(0);
}
