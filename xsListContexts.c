/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsListContexts

 Usage: xsListContexts [-s scope]
    -s the scope containing the contexts to list (defaults to global)

 Description:
    List the contexts in a scope (private, app or global)
*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "saLicensing.h"
#include "saSplunk.h"

extern char *optarg;
extern int optind, optopt;

extern bool saSplunkGetContextPath(char *, char *, int, char *, char *);
extern char *saSplunkGetRoot(char *);
extern int saSplunkGetScope(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);
extern void saListDir(char *, char *, bool, FILE *, char *);

int main(int argc, char *argv[])
{
    bool argError;
    int c;
    int scope = SA_SPLUNK_SCOPE_GLOBAL;
    
    if (!isLicensed())
        exit(EXIT_FAILURE);

    while ((c = getopt(argc, argv, "s:")) != -1) 
    {
        switch(c)
        {
            case 's':
                scope = saSplunkGetScope(optarg);
                break;
            case '?':
                fprintf(stderr, "xsListContexts-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsListContexts-F-103: Usage: xsListContexts [-s scope]");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsListContexts-F-105: Can't get Header record\n");
        exit(EXIT_FAILURE);
    }

    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsListContexts-F-107: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    char path[1024];
    char *root = saSplunkGetRoot(argv[0]);
    if (saSplunkGetContextPath(path, root, scope, p->app, p->user) == false)
    {
        fprintf(stderr, "xsListContexts-F-109: failed to get context path\n");
        exit(EXIT_FAILURE);
    }
    saListDir(path, ".context", true, stdout, "context");
}
