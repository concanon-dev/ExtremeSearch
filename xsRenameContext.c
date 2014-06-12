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

extern saContextTypePtr saSplunkContextLoad(char *, int *, char *, char *);
extern bool saSplunkContextRename(char *, int, char *, char *, char *, int, char *, char *);
extern bool saSplunkContextSave(saContextTypePtr, int, char *, char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern int saSplunkGetScope(char *);
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    saContextTypePtr contextPtr;
    bool argError;
    char destContextName[256];
    char srcContextName[256];
    int c;
    int destScope = saSplunkGetScope(NULL);
    int srcScope = saSplunkGetScope(NULL);

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    destContextName[0] ='\0';
    srcContextName[0] ='\0';
    while ((c = getopt(argc, argv, "d:s:0:1:")) != -1) 
    {
        switch (c)
        {
            case 's':
	        strcpy(srcContextName, optarg);
                break;
            case 'd':
                strcpy(destContextName, optarg);
                break;
            case '0':
                srcScope = saSplunkGetScope(optarg);
                break;
            case '1':
                destScope = saSplunkGetScope(optarg);
                break;
            case '?':
                fprintf(stderr, "xsRenameContext-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsRenameContext-F-103: Usage: xsRenameContext -d destContextName -s contextName -0 srcSscope -1 destScope\n");
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

    if(!saSplunkContextRename(srcContextName, srcScope, p->app, p->user,
                              destContextName, destScope, p->app, p->user))
    {
        fprintf(stderr, "xsRenameContext-F-107: Can't rename context: %s\n", srcContextName);
        exit(EXIT_FAILURE);
    }
    saContextTypePtr cPtr = saSplunkContextLoad(destContextName, &destScope, p->app, p->user);
    if (contextPtr == NULL)
    {
        fprintf(stderr, "xsRenameContext-F-109: Can't open context: %s\n", destContextName);
        exit(EXIT_FAILURE);
    }
    cPtr->name = destContextName;
    saSplunkContextSave(cPtr, destScope, p->app, p->user);
    fprintf(stderr, "Context %s successfully renamed to %s\n", srcContextName, destContextName);
    exit(0);
}
