/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsDisplayContext

 Usage: xsDisplayContext -n context [-s scope]
    -n the name of the context to display
    -s the scope (private, app, global) to find the context (default is none)

 Description:
    Generates a table with the bucket values and membership values of every concept in a context
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
extern char *saSplunkGetRoot(char *);
extern int saSplunkGetScope(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    saContextTypePtr contextPtr;
    bool argError;
    char contextName[256];
    char *root = saSplunkGetRoot(argv[0]);
    int c;
    int scope = saSplunkGetScope(NULL);

    initSignalHandler(basename(argv[0]));

    argError = false;
    contextName[0] ='\0';
    while ((c = getopt(argc, argv, "n:s:")) != -1) 
    {
        switch(c)
        {
            case 'n':
	        strcpy(contextName, optarg);
                break;
            case 's':
                if ((scope = saSplunkGetScope(optarg)) == SA_SPLUNK_SCOPE_UNKNOWN)
                {
                    fprintf(stderr, 
                            "xsDisplayContext-F-111: Scope %s is not legal, try private, app, or global\n",
                            optarg);
                    exit(EXIT_FAILURE);
                }

                break;
            case '?':
                fprintf(stderr, "xsDisplayContext-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsDisplayContext-F-103: Usage: xsDisplayContext -f contextFileName");
        exit(EXIT_FAILURE);
    }
    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsDisplayContext-F-105: Can't get info header\n");
        exit(EXIT_FAILURE);
    }
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsDisplayContext-F-107: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    contextPtr = saSplunkContextLoad(contextName, root, &scope, p->app, p->user);
    if (contextPtr == NULL)
    {
        fprintf(stderr, "xsDisplayContext-F-109: Can't open context: %s\n", contextName);
        exit(EXIT_FAILURE);
    }
    saContextDisplay(contextPtr);
    exit(0);
}
