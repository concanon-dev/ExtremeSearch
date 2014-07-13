/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsDeleteContext

 Usage: xsDeleteContext -c context [-s scope] 
    -c name of the context to delete
    -s scope of the context (private, app, global)

 Description:
    Deletes a context.
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

extern char *optarg;
extern int optind, optopt;

extern bool saSplunkContextDelete(char *, char *, int, char *, char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern char *saSplunkGetRoot(char *);
extern int saSplunkGetScope(char *);
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
    while ((c = getopt(argc, argv, "c:s:")) != -1) 
    {
        switch(c)
        {
            case 'c':
	        strcpy(contextName, optarg);
                break;
            case 's':
                if ((scope = saSplunkGetScope(optarg)) == SA_SPLUNK_SCOPE_UNKNOWN)
                {
                    fprintf(stderr, 
                            "xsDeleteContext-F-111: Old Scope %s is not legal, try private, app, or global\n",
                            optarg);
                    exit(EXIT_FAILURE);
                }

                break;
            case '?':
                fprintf(stderr, "xsDeleteContext-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsDeleteContext-F-103: Usage: xsDeleteContext -c contextName -s scope\n");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsDeleteContext-F-105: Can't get info header\n");
        exit(EXIT_FAILURE);
    }

    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsDeleteContext-F-107: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    if(!saSplunkContextDelete(contextName, root, scope, p->app, p->user))
    {
        fprintf(stderr, "xsDeleteContext-F-109: Can't delete context: %s\n", contextName);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Context %s successfully deleted\n", contextName);
    exit(0);
}
