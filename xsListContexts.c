#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "saLicensing.h"
#include "saSplunk.h"

extern char *optarg;
extern int optind, optopt;

extern void saSplunkGetContextPath(char *, char *, int, char *, char *);
extern char *saSplunkGetRoot(char *);
extern int saSplunkGetScope(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);
extern void saListDir(char *, char *, bool, FILE *, char *);

int main(int argc, char *argv[])
{
    bool argError;
    int c;
    int scope = -1;
    
    if (!isLicensed())
        exit(EXIT_FAILURE);

    while ((c = getopt(argc, argv, "s:")) != -1) 
    {
        switch (c)
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
        fprintf(stderr, "xsListContexts-F-103: Usage: xsListContexts -s scope");
        exit(EXIT_FAILURE);
    }

    if (scope == -1)
        scope = saSplunkGetScope(NULL);
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
    saSplunkGetContextPath(path, root, scope, p->app, p->user);
    saListDir(path, ".context", true, stdout, "context");
}
