/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "saContext.h"
#include "saCSV.h"
#include "saSplunk.h"

#define MAXROWSIZE 1024*500

extern saContextTypePtr saContextLoad(FILE *);
extern int saContextSave(FILE *, saContextTypePtr);
extern FILE *saOpenFile(char *, char *);

void saSplunkGetContextPath(char *path, int scope, char *app, char *user)
{
    char *splunkHome = getenv("SPLUNK_HOME");

    if (scope == SA_SPLUNK_SCOPE_PRIVATE) 
    {
        sprintf(path, "%s/users/%s/%s/contexts", splunkHome, user, app);
    } 
    else if (scope == SA_SPLUNK_SCOPE_APP) 
    {
        char *appPath = "/etc/apps";
        sprintf(path, "%s%s/%s/contexts", splunkHome, appPath, app);
    } 
    else 
    {
        char *xtremePath = "/etc/apps/xtreme";
        sprintf(path, "%s%s/contexts", splunkHome, xtremePath);
    }
}
bool saSplunkContextDelete(char *name, int scope, char *app, char *user)
{

    char file[1024];
    char path[1024];

    saSplunkGetContextPath(path, scope, app, user);

    sprintf(file, "/%s.context", name);
    strcat(path, file);
    if (!unlink(path)) 
    {
        return(true);
    }
    return(false);
}

saContextTypePtr saSplunkContextFind(char *name, char *app, char *user, int *scope)
{
    saContextTypePtr contextPtr = NULL;
    char path[1024];
    char *splunkHome = getenv("SPLUNK_HOME");

    *scope = SA_SPLUNK_SCOPE_PRIVATE;
    sprintf(path, "%susers/%s/%s/contexts/%s.context", splunkHome, user, app, name);
    if (access(path, F_OK) == -1)
    {
        *scope = SA_SPLUNK_SCOPE_APP;
        char *appPath = "/etc/apps";
        sprintf(path, "%s%s/%s/contexts/%s.context", splunkHome, appPath, app, name);
        if (access(path, F_OK) == -1)
        {
            *scope = SA_SPLUNK_SCOPE_GLOBAL;
            char *xtremePath = "/etc/apps/xtreme";
            sprintf(path, "%s%s/contexts/%s.context", splunkHome, xtremePath, name);
            if (access(path, F_OK) == -1)
            {
                return(NULL);
            }
        }
    }
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        *scope = SA_SPLUNK_SCOPE_NONE;
        return(NULL);
    }
    contextPtr = saContextLoad(f);
    fclose(f);

    return(contextPtr);    
}

saContextTypePtr saSplunkContextLoad(char *name, int *scope, char *app, char *user)
{
    char file[1024];
    char path[1024];
    saContextTypePtr contextPtr = NULL;

    if (*scope == SA_SPLUNK_SCOPE_NONE)
    {
        return(saSplunkContextFind(name, app, user, scope));
    }
    else
    {
        saSplunkGetContextPath(path, *scope, app, user);

        sprintf(file, "/%s.context", name);
        strcat(path, file);
        FILE *f = fopen(path, "r");
        if (f == NULL)
	{
            return(NULL);
	}
        contextPtr = saContextLoad(f);
        fclose(f);
    }
    return(contextPtr);
}

bool saSplunkContextSave(saContextTypePtr contextPtr, int scope, char *app, char *user)
{
    char file[1024];
    char path[1024];

    saSplunkGetContextPath(path, scope, app, user);

#ifdef _UNICODE
    mkdir(path);
#else
    mkdir(path, 0755);
#endif

    sprintf(file, "/%s.context", contextPtr->name);
    strcat(path, file);
    // FILE *f = saOpenFile(path, "w");
    FILE *f = fopen(path, "w");
    if (f == NULL)
    {
        return(false);
    }

    saContextSave(f, contextPtr);
    fclose(f);
    return(true);
}

int saSplunkGetScope(char *scopeStr)
{
    if (scopeStr == NULL)
        return(SA_SPLUNK_SCOPE_NONE);
    else if (scopeStr[0] == '\0')
        return(SA_SPLUNK_SCOPE_NONE);

    char *s = scopeStr;
    while(*s != '\0')
    {
          *s = (char)tolower(*s);
          s++;
    }
    if (!strcmp(scopeStr, "none") || !strcmp(scopeStr, "0"))
        return(SA_SPLUNK_SCOPE_NONE);
    else if (!strcmp(scopeStr, "private") || !strcmp(scopeStr, "1"))
        return(SA_SPLUNK_SCOPE_PRIVATE);
    else if (!strcmp(scopeStr, "app") || !strcmp(scopeStr, "2"))
        return(SA_SPLUNK_SCOPE_APP);
    else
        return(SA_SPLUNK_SCOPE_GLOBAL);
}

saSplunkInfoPtr saSplunkLoadHeader()
{
    char *fields[128];
    char inbuf[MAXROWSIZE];
    saSplunkInfoPtr p = NULL;

    bool done = false;
    while(done == false)
    {
        int numFields = saCSVGetHeaderLine(inbuf, fields);
        if (numFields == 0)
            done = true;
        else if (numFields == 2)
        {
            if (!strcmp(fields[0], "infoPath"))
            {
                if (p == NULL)
                    p = (saSplunkInfoPtr)malloc(sizeof(saSplunkInfoType));
                strcpy(p->infoPath, fields[1]);
            }
        }
    }
    return(p);
}

bool saSplunkReadInfoPathFile(saSplunkInfoPtr p)
{
    char *fields[128];
    char inbuf[MAXROWSIZE];
    int appIndex = -1;
    int userIndex = -1;

    if (p == NULL)
        return(false);

    FILE *f = fopen(p->infoPath, "r");
    if (f == NULL)
    {
        return(false);
    }

    int numHeaderFields = saCSVFGetLine(f, inbuf, fields);
    if (numHeaderFields == 0)
    {
        fclose(f);
        return(false);
    }

    int i;
    for(i=0; i<numHeaderFields; i++)
    {
        if (!saCSVCompareField(fields[i], "_ppc.user"))
            userIndex = i;
        else if (!saCSVCompareField(fields[i], "_ppc.app"))
            appIndex = i;
    }
    if (userIndex == -1 || appIndex == -1)
    {
        fclose(f);
        return(false);
    }
    int numFields = saCSVFGetLine(f, inbuf, fields);
    if (numFields < userIndex || numFields < appIndex)
    {
        fclose(f);
        return(false);
    }
    strcpy(p->app, fields[appIndex]);
    strcpy(p->user, fields[userIndex]);
    fclose(f);
    return(true);
}

saSplunkInfoPtr saSplunkLoadInfo(char *infoPath)
{
    saSplunkInfoPtr p = (saSplunkInfoPtr)malloc(sizeof(saSplunkInfoType));
    strcpy(p->infoPath, infoPath);
    if (saSplunkReadInfoPathFile(p) == false)
        return(NULL);
    return(p);
}

/*
int main(int argc, char *argv[])
{
    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "FALSE\n");
        exit(0);
    }
}
*/
