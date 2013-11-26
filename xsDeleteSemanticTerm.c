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

extern saContextTypePtr saSplunkContextLoad(char *, int, char *, char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);
extern bool saSplunkContextSave(saContextTypePtr, int, char *, char *);

FILE *saOpenFile(char *, char *);

int main(int argc, char* argv[]) 
{
    saContextTypePtr contextPtr;
    bool argError;

    char contextName[512];
    char semanticTermName[512];
    int i, c;
    int scope = saSplunkGetScope(NULL);

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    contextName[0] ='\0';
    while ((c = getopt(argc, argv, "c:s:t:")) != -1) 
    {
        switch (c)
        {
            case 'c':
	        strcpy(contextName, optarg); 
                break;
            case 's':
                scope = saSplunkGetScope(optarg);
                break;
            case 't':
                strcpy(semanticTermName, optarg);
                break;
            case '?':
                fprintf(stderr, "xsDeleteSemanticTerm-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr,
                "xsDeleteSemanticTerm-F-103: Usage: xsDeleteSemanticTerm -c contextName [-s scope] -t semanticTerm\n");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsDeleteSemanticTerm-F-111: can't load header\n");
        exit(EXIT_FAILURE);
    }

    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsDeleteSemanticTerm-F-119: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    contextPtr = saSplunkContextLoad(contextName, scope, p->app, p->user);
    if (contextPtr == NULL)
    {
        fprintf(stderr, "xsDeleteSemanticTerm-F-113: Can't open context: %s\n", contextName);
        exit(EXIT_FAILURE);
    }

    bool found = false;
    i = 0;
    while(!found && i<contextPtr->numSemanticTerms)
    {
        if (!strcmp(semanticTermName, contextPtr->semanticTerms[i]->name))
            found = true;
        else
            i++;
    }
    if (!found)
    {
        fprintf(stderr, 
                "xsDeleteSemanticTerm-F-115: semantic term %s does not exist in context %s\n",
                semanticTermName, contextPtr->name);
        exit(0);
    }

    int j;
    for(j=i; j<SA_CONTEXT_MAXTERMS-1; j++)
        contextPtr->semanticTerms[j] = contextPtr->semanticTerms[j+1];
    contextPtr->numSemanticTerms--;

    if (saSplunkContextSave(contextPtr, scope, p->app, p->user) == false)
    {
        fprintf(stderr, "xsDeleteSemanticTerm-F-117: Can't save context %s\n", contextPtr->name);
        exit(0);
    }

    exit(0);
}
