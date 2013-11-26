/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
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
#include "saSplunk.h"

extern char *optarg;
extern int optind, optopt;

extern bool saSplunkContextDelete(char *, int, char *, char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    saContextTypePtr contextPtr;
    bool argError;
    char contextName[256];
    int c;
    int scope = saSplunkGetScope(NULL);

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    contextName[0] ='\0';
    while ((c = getopt(argc, argv, "c:s:")) != -1) 
    {
        switch (c)
        {
            case 'c':
	        strcpy(contextName, optarg);
                break;
            case 's':
                scope = saSplunkGetScope(optarg);
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
        fprintf(stderr, "xsDeleteContext-F-105: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    if(!saSplunkContextDelete(contextName, scope, p->app, p->user))
    {
        fprintf(stderr, "xsDeleteContext-F-107: Can't delete context: %s\n", contextName);
        exit(EXIT_FAILURE);
    }
    exit(0);
}
