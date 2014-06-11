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

extern char *optarg;
extern int optind, optopt;

extern bool saSplunkContextRename(char *, int, char *, char *, char *, int, char *, char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern int saSplunkGetScope(char *);
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    saContextTypePtr contextPtr;
    bool argError;
    char contextName[256];
    char newContextName[256];
    int c;
    int scope = saSplunkGetScope(NULL);

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    contextName[0] ='\0';
    newContextName[0] ='\0';
    while ((c = getopt(argc, argv, "c:n:s:")) != -1) 
    {
        switch (c)
        {
            case 'c':
	        strcpy(contextName, optarg);
                break;
            case 'n':
                strcpy(newContextName, optarg);
                break;
            case 's':
                scope = saSplunkGetScope(optarg);
                break;
            case '?':
                fprintf(stderr, "xsRenameContext-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsRenameContext-F-103: Usage: xsRenameContext -c contextName -s scope\n");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsRenameContext-F-105: Can't get info header\n");
        exit(EXIT_FAILURE);
    }

    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsRenameContext-F-105: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    if(!saSplunkContextRename(contextName, scope, p->app, p->user,
                              newContextName, scope, p->app, p->user))
    {
        fprintf(stderr, "xsRenameContext-F-107: Can't rename context: %s\n", contextName);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Context %s successfully renamed to %s\n", contextName, newContextName);
    exit(0);
}
