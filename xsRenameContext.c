/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsRenameContext

 Usage: xsRenameContext -s srcContext [-0 srcScope] -d destContext [-1 destScope]
    -0 scope of source context
    -1 scope of destination context
    -d destination context
    -s source context

 Description:
    Renames a Context.  If a destScope is not specified, then the value defaults to the srcScope.
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

extern saContextTypePtr saSplunkContextLoad(char *, char *, int *, char *, char *);
extern bool saSplunkContextRename(char *, char *, int, char *, char *, char *, char *, int, char *, char *);
extern bool saSplunkContextSave(saContextTypePtr, char *, int, char *, char *);
extern char *saSplunkGetRoot(char *);
extern int saSplunkGetScope(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    bool argError;
    char destContextName[256];
    char *root = saSplunkGetRoot(argv[0]);
    char srcContextName[256];
    int c;
    int destScope = -1;
    int srcScope = saSplunkGetScope(NULL);

    initSignalHandler(basename(argv[0]));
    argError = false;
    destContextName[0] ='\0';
    srcContextName[0] ='\0';
    while ((c = getopt(argc, argv, "d:s:0:1:")) != -1) 
    {
        switch(c)
        {
            case 's':
	        strcpy(srcContextName, optarg);
                break;
            case 'd':
                strcpy(destContextName, optarg);
                break;
            case '0':
                if ((srcScope = saSplunkGetScope(optarg)) == SA_SPLUNK_SCOPE_UNKNOWN)
                {
                    fprintf(stderr, 
                            "xsRenameContext-F-113: Source Scope %s is not legal, try private, app, or global\n",
                            optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case '1':
                if ((destScope = saSplunkGetScope(optarg)) == SA_SPLUNK_SCOPE_UNKNOWN)
                {
                    fprintf(stderr, 
                        "xsRenameContext-F-115: Destination Scope %s is not legal, try private, app, or global\n",
                            optarg);
                    exit(EXIT_FAILURE);
                }

                break;
            case '?':
                fprintf(stderr, "xsRenameContext-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsRenameContext-F-103: Usage: xsRenameContext -d destContextName -s contextName [-0 srcSscope] [-1 destScope]\n");
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
        fprintf(stderr, "xsRenameContext-F-107: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    if(!saSplunkContextRename(srcContextName, root, srcScope, p->app, p->user,
                              destContextName, root, destScope, p->app, p->user))
    {
        fprintf(stderr, "xsRenameContext-F-109: Can't rename context: %s\n", srcContextName);
        exit(EXIT_FAILURE);
    }
    if (destScope == -1)
        destScope = srcScope;
    saContextTypePtr cPtr = saSplunkContextLoad(destContextName, root, &destScope, p->app, p->user);
    if (cPtr == NULL)
    {
        fprintf(stderr, "xsRenameContext-F-111: Can't open context: %s\n", destContextName);
        exit(EXIT_FAILURE);
    }
    cPtr->name = destContextName;
    saSplunkContextSave(cPtr, root, destScope, p->app, p->user);
    fprintf(stderr, "Context %s successfully renamed to %s\n", srcContextName, destContextName);
    exit(0);
}
