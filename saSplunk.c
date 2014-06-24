/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
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
#include "saHedge.h"
#include "saSplunk.h"

#define MAXROWSIZE 1024*500

extern char *saCSVExtractField(char *);
extern saContextTypePtr saContextLoad(FILE *);
extern int saContextSave(FILE *, saContextTypePtr);
extern bool saHedgeLoadLookup(FILE *, saSynonymTableTypePtr);

bool saSplunkGetContextPath(char *path, char *root, int scope, char *app, char *user)
{
    if (path == NULL)
        return(false);
    *path = '\0';

    if (scope == SA_SPLUNK_SCOPE_PRIVATE) 
    {
        if (app == NULL || user == NULL)
            return(false);
        if (*app == '\0' || *user == '\0')
            return(false);
        sprintf(path, "%s/users/%s/%s/contexts", root, user, app);
    } 
    else if (scope == SA_SPLUNK_SCOPE_APP) 
    {
        if (app == NULL)
            return(false);
        if (*app == '\0')
            return(false);
        sprintf(path, "%s/apps/%s/contexts", root, app);
    } 
    else 
        sprintf(path, "%s/apps/xtreme/contexts", root);
    return(true);
}

bool saSplunkContextDelete(char *name, char *root, int scope, char *app, char *user)
{

    char file[1024];
    char path[1024];

    if (saSplunkGetContextPath(path, root, scope, app, user) == false)
        return(false);

    sprintf(file, "/%s.context", name);
    strcat(path, file);
    //if (!unlink(path)) 
    if (remove(path) == 0) 
    {
        return(true);
    }
    return(false);
}

saContextTypePtr saSplunkContextFind(char *name, char *root, char *app, char *user, int *scope)
{
    saContextTypePtr contextPtr = NULL;
    char path[1024];

    if (app == NULL || user == NULL)
        return(NULL);
    *scope = SA_SPLUNK_SCOPE_PRIVATE;
    sprintf(path, "%s/users/%s/%s/contexts/%s.context", root, user, app, name);
    if (access(path, F_OK) == -1)
    {
        *scope = SA_SPLUNK_SCOPE_APP;
        sprintf(path, "%s/apps/%s/contexts/%s.context", root, app, name);
        if (access(path, F_OK) == -1)
        {
            *scope = SA_SPLUNK_SCOPE_GLOBAL;
            sprintf(path, "%s/apps/xtreme/contexts/%s.context", root, name);
            if (access(path, F_OK) == -1)
                return(NULL);
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

saContextTypePtr saSplunkContextLoad(char *name, char *root, int *scope, char *app, char *user)
{
    char file[1024];
    char path[1024];
    saContextTypePtr contextPtr = NULL;

    if (*scope == SA_SPLUNK_SCOPE_NONE)
        return(saSplunkContextFind(name, root, app, user, scope));
    else
    {
        if (saSplunkGetContextPath(path, root, *scope, app, user) == false)
            return(NULL);

        sprintf(file, "/%s.context", name);
        strcat(path, file);
        FILE *f = fopen(path, "r");
        if (f == NULL)
            return(NULL);
        contextPtr = saContextLoad(f);
        fclose(f);
    }
    return(contextPtr);
}


bool saSplunkContextRename(char *name, char *root, int scope, char *app, char *user,
                           char *newName, char *newRoot, int newScope, char *newApp, char *newUser)
{
    char file[1024];
    char newFile[1024];
    char newPath[1024];
    char path[1024];

    if (saSplunkGetContextPath(path, root, scope, app, user) == false)
        return(false);

    if (saSplunkGetContextPath(newPath, newRoot, newScope, newApp, newUser) == false)
        return(false);

    sprintf(file, "/%s.context", name);
    sprintf(newFile, "/%s.context", newName);
    strcat(path, file);
    strcat(newPath, newFile);
    if (rename(path, newPath) == 0)
    {
        return(true);
    }
    return(false);
}

bool saSplunkContextSave(saContextTypePtr contextPtr, char *root, int scope, char *app, char *user)
{
    char file[1024];
    char path[1024];

    if (saSplunkGetContextPath(path, root, scope, app, user) == false)
        return(false);

#ifdef _UNICODE
    mkdir(path);
#else
    mkdir(path, 0755);
#endif

    sprintf(file, "/%s.context", contextPtr->name);
    strcat(path, file);
    FILE *f = fopen(path, "w");
    if (f == NULL)
        return(false);

    saContextSave(f, contextPtr);
    fclose(f);
    return(true);
}

char *saSplunkGetRoot(char *root)
{
    char *newRoot = root;
    int i=0;
    while(i<6)
    {
        if (newRoot == NULL)
            return(NULL);
        newRoot = dirname(newRoot);
        i++;
    }
    return(newRoot);
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
        return(false);

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
    fclose(f);
    strcpy(p->app, saCSVExtractField(fields[appIndex]));
    strcpy(p->user, saCSVExtractField(fields[userIndex]));
    if (strlen(p->app) == 0 || strlen(p->user) == 0)
        return(false);
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

bool saSplunkHedgeLoad(char *name, char *root, char *app, char *user, int *scope, saSynonymTableTypePtr synonyms)
{
    char path[1024];

    if (app == NULL || user == NULL)
        return(false);

    *scope = SA_SPLUNK_SCOPE_PRIVATE;
    sprintf(path, "%s/users/%s/%s/lookups/%s.csv", root, user, app, name);
    if (access(path, F_OK) == -1)
    {
        *scope = SA_SPLUNK_SCOPE_APP;
        sprintf(path, "%s/apps/%s/lookups/%s.csv", root, app, name);
        if (access(path, F_OK) == -1)
        {
            *scope = SA_SPLUNK_SCOPE_GLOBAL;
            sprintf(path, "%s/apps/xtreme/lookups/%s.csv", root, name);
            if (access(path, F_OK) == -1)
                return(false);
        }
    }
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        *scope = SA_SPLUNK_SCOPE_NONE;
        return(false);
    }
    bool worked = saHedgeLoadLookup(f, synonyms);
    fclose(f);

    return(worked);
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
