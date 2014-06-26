/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsListUOM

 Usage: xsListUOM -c context [-s scope]
    -c the name of the context
    -s the scope containing the contexts to list (defaults to global)

 Description:
    List the Unit of Measurement (UOM) from a context in a scope (private, app or global)
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

#include "saSignal.h"
#include "saSplunk.h"
#include "saUtils.h"

#define MAXROWSIZE (8192*11)

extern char *optarg;
extern int optind, optopt;

extern saContextTypePtr saSplunkContextLoad(char *, char *, int *, char *, char *);
extern char *saSplunkGetRoot(char *);
extern int saSplunkGetScope(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    saContextTypePtr contextPtr;
    bool argError;
    char contextString[128];
    char *root = saSplunkGetRoot(argv[0]);
    char scopeString[256];
    int i, c;

    initSignalHandler(basename(argv[0]));
    argError = false;
    contextString[0] ='\0';
    strcpy(scopeString, "global");
    while ((c = getopt(argc, argv, "c:s:")) != -1) 
    {
        switch(c)
        {

            case 'c':
                strcpy(contextString, optarg);
                break;
            case 's':
                strcpy(scopeString, optarg);
                break;
            case '?':
                fprintf(stderr, "xsListUOM-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsListUOM-F-103: Usage: xsListUOM -c context [-s scope]");
        exit(EXIT_FAILURE);
    }

    // get the header information
    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsListUOM-F-123: can't get header\n");
        exit(0);
    }

    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsListUOM-F-125: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    int scope = saSplunkGetScope(scopeString);
    contextPtr = saSplunkContextLoad(contextString, root, &scope, p->app, p->user);
    if (contextPtr == NULL)
    {
        fprintf(stderr, "xsListUOM-F-111: can't open context %s\n", contextString);
        exit(0);
    }
    fputs("\"UOM\"\n", stdout);
    fprintf(stdout, "\"%s\"\n", contextPtr->uom);
    exit(0);
}
