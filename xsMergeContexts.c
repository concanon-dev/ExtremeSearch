/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsMergeContext

 Usage: xsMergeContext -a context -b context -c context [-A scope] [-B scope] [-C scope]
    -a the first context to merge
    -b the second context to merge
    -c the merged context
    -A the scope (private, app, global) to find the first context (default is none)
    -B the scope (private, app, global) to find the second context (default is none)
    -C the scope (private, app, global) to store the merged context (default is global)

 Description:
    Merge two contexts, using weighted methods, into a new context.  If the counts are both 0, then they
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

extern void saContextDisplay(saContextTypePtr);
extern saContextTypePtr saSplunkContextLoad(char *, char *, int *, char *, char *);
extern saContextTypePtr saContextMerge(saContextTypePtr, saContextTypePtr, char *);
extern bool saSplunkContextSave(saContextTypePtr, char *, int, char *, char *);
extern char *saSplunkGetRoot(char *);
extern int saSplunkGetScope(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    saContextTypePtr contextPtrA;
    saContextTypePtr contextPtrB;
    saContextTypePtr contextPtrC;
    bool argError;
    char contextNameA[256];
    char contextNameB[256];
    char contextNameC[256];
    char *root = saSplunkGetRoot(argv[0]);
    int c;
    int scopeA = saSplunkGetScope(NULL);
    int scopeB = saSplunkGetScope(NULL);
    int scopeC = saSplunkGetScope("private");

    initSignalHandler(basename(argv[0]));

    argError = false;
    contextNameA[0] ='\0';
    contextNameB[0] ='\0';
    contextNameC[0] ='\0';
    while ((c = getopt(argc, argv, "A:B:C:a:b:c:")) != -1) 
    {
        switch(c)
        {
            case 'A':
                scopeA = saSplunkGetScope(optarg);
                break;
            case 'B':
                scopeB = saSplunkGetScope(optarg);
                break;
            case 'C':
                scopeC = saSplunkGetScope(optarg);
                break;
            case 'a':
	        strcpy(contextNameA, optarg);
                break;
            case 'b':
	        strcpy(contextNameB, optarg);
                break;
            case 'c':
	        strcpy(contextNameC, optarg);
                break;
            case '?':
                fprintf(stderr, "xsMergeContext-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }

    if (argError)
    {
        fprintf(stderr, "xsMergeContext-F-103: Usage: xsMergeContext -a context -b context -c context [-A scope] [-B scope] [-C scope]");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsMergeContext-F-105: Can't get info header\n");
        exit(EXIT_FAILURE);
    }
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsMergeContext-F-105: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    contextPtrA = saSplunkContextLoad(contextNameA, root, &scopeA, p->app, p->user);
    if (contextPtrA == NULL)
    {
        fprintf(stderr, "xsMergeContext-F-107: Can't open context: %s\n", contextNameA);
        exit(EXIT_FAILURE);
    }
    contextPtrB= saSplunkContextLoad(contextNameB, root, &scopeB, p->app, p->user);
    if (contextPtrB == NULL)
    {
        fprintf(stderr, "xsMergeContext-F-109: Can't open context: %s\n", contextNameB);
        exit(EXIT_FAILURE);
    }
    contextPtrC = saContextMerge(contextPtrA, contextPtrB, contextNameC);
    if (contextPtrC == NULL)
    {
        fprintf(stderr, "xsMergeContext-F-111: failed to merge contexts\n");
        exit(EXIT_FAILURE);
    }
    if (saSplunkContextSave(contextPtrC, root, scopeC, p->app, p->user) == false)
    {
        fprintf(stderr, "xsMergeContext-F-113: Can't save context: %s\n", contextNameC);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Contexts merged successfully\n");
    exit(0);
}
