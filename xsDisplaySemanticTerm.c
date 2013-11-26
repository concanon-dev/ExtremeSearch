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
#include "saHedge.h"
#include "saSplunk.h"
#include "saUtils.h"

#define MAXROWSIZE (8192*11)

extern char *optarg;
extern int optind, optopt;

extern saSemanticTermTypePtr saHedgeApply(int, saSemanticTermTypePtr);
extern bool saHedgeLoadLookup(FILE *, saSynonymTableTypePtr);
extern char *saHedgeLookup(saSynonymTableTypePtr, char *);

extern saContextTypePtr saSplunkContextLoad(char *, int, char *, char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

void saContextDisplay(saContextTypePtr);
FILE *saOpenFile(char *, char *);

static saSynonymTableType synonyms;

int main(int argc, char* argv[]) 
{
    saContextTypePtr contextPtr;
    saSemanticTermTypePtr semanticTerm[32];
    bool argError;
    char contextFileName[512];
    char *contextList[32];
    char contextString[128];
    char filePath[128];
    char *finalSetName[32];
    char *scopeList[32];
    char scopeString[256];
    char *setArray[64];
    char setString[256];
    char *setStringList[32];
    char synonymFileName[128];
    int i, c;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    contextFileName[0] ='\0';
    strcpy(scopeString, "none");
    setString[0] = '\0';
    synonymFileName[0] = '\0';
    synonyms.numWords = 0;
    while ((c = getopt(argc, argv, "c:l:p:s:")) != -1) 
    {
        switch (c)
        {

            case 'c':
                strcpy(contextString, optarg);
                break;
            case 'l':
                strcpy(setString, optarg);
                break;
            case 'p':
                strcpy(scopeString, optarg);
                break;
            case 's':
                strcpy(synonymFileName, optarg);
                break;
            case '?':
                fprintf(stderr, "xsDisplaySemanticTerm-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsDisplaySemanticTerm-F-103: Usage: xsContext -f contextFileName");
        exit(EXIT_FAILURE);
    }

    // load the synonym file if it exists
    if (strlen(synonymFileName)>0)
    {
        FILE *sFile = saOpenFile(synonymFileName, "r");
        if (sFile == NULL)
            synonymFileName[0] = '\0';
        else
        {
            bool loaded = saHedgeLoadLookup(sFile, &synonyms);
            fclose(sFile);
            if (loaded == false)
            {
                fprintf(stderr, "xsDisplaySemanticTerm-F-117: Error in loading synonyms file: %s\n",
                        synonymFileName);
                exit(0);
            }
        }
    }
    
    if (strlen(setString) == 0)
    {
        fprintf(stderr, "xsDisplayContext-F-113: No semantic term specified\n");
        exit(0);
    }
    int numSets = 0;
    char *s = setString;
    char *t = contextString;
    char *u = scopeString;
    while ((setStringList[numSets] = strsep(&s, ";")) != NULL)
    {
            contextList[numSets] = strsep(&t, ";");
            scopeList[numSets] = strsep(&u, ";");
            numSets++;
    }

    if (numSets == 0)
    {
        fprintf(stderr, "xsDisplayContext-F-121: No semantic terms found\n");
        exit(0);
    }

    // get the header information
    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsDisplaySemanticTerm-F-123: can't get header\n");
        exit(0);
    }
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsDisplaySemanticTerm-F-125: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    // for each specified set, generate the fuzzy set
    int thisSet = 0;
    for(thisSet=0; thisSet<numSets; thisSet++)
    {
        s = setStringList[thisSet];
        char *s2 = strtok(s, ",");
        int numSemanticTerms = 0;
        while(s2 != NULL)
        {
            setArray[numSemanticTerms++] = s2;
            s2 = strtok(NULL, ",");
        } 

        int scope = saSplunkGetScope(scopeList[thisSet]);
        char *cName = NULL;
        if (contextList[thisSet] != NULL && *contextList[thisSet] != '\0')
            cName = contextList[thisSet];
        else
            cName = setArray[numSemanticTerms-1];

        contextPtr = saSplunkContextLoad(cName, scope, p->app, p->user);
        if (contextPtr == NULL)
        {
            fprintf(stderr, "xsDisplaySemanticTerm-F-111: can't open context %s\n", cName);
            exit(0);
        }

        bool found = false;
        i = 0;
        while(!found && i<contextPtr->numSemanticTerms)
        {
            if (!strcmp(setArray[numSemanticTerms-1], contextPtr->semanticTerms[i]->name))
            {
                found = true;
                semanticTerm[thisSet] = contextPtr->semanticTerms[i];
            }
            else
                i++;
        }
        if (!found)
        {
            fprintf(stderr, "xsDisplaySemanticTerm-F-115: semantic term %s does not exist in context %s\n",
                    setArray[numSemanticTerms-1], contextPtr->name);
            exit(0);
        }

        i = numSemanticTerms - 2;
        while(i>=0)
        {
            int numHedges = 0;
            char *hedgeArray[128];
            char *hedgeStr = malloc(sizeof(char) * 128);
            hedgeStr[0] = '\0';
            char *hedgeWord = saHedgeLookup(&synonyms, setArray[i]);
            char *s = strtok(hedgeWord, " ");
            while(s != NULL)
            {
                hedgeArray[numHedges++] = s;
                strcat(hedgeStr, s);
                s = strtok(NULL, " ");
                if (s != NULL)
                    strcat(hedgeStr, " ");
            }
            setArray[i] = hedgeStr; 
            int j;
            for(j=numHedges-1; j>=0; j--)
            {
                int hedgeIndex = SA_HEDGE_ERROR;
                if (!strcmp(hedgeArray[j], "_noise"))
                    hedgeIndex = SA_HEDGE__NOISE;
                else if (!strcmp(hedgeArray[j], "about"))
                    hedgeIndex = SA_HEDGE_ABOUT;
                else if (!strcmp(hedgeArray[j], "above"))
                    hedgeIndex = SA_HEDGE_ABOVE;
                else if (!strcmp(hedgeArray[j], "after"))
                    hedgeIndex = SA_HEDGE_AFTER;
                else if (!strcmp(hedgeArray[j], "before"))
                    hedgeIndex = SA_HEDGE_BEFORE;
                else if (!strcmp(hedgeArray[j], "below"))
                    hedgeIndex = SA_HEDGE_BELOW;
                else if (!strcmp(hedgeArray[j], "close"))
                    hedgeIndex = SA_HEDGE_CLOSE;
                else if (!strcmp(hedgeArray[j], "extremely"))
                    hedgeIndex = SA_HEDGE_EXTREMELY;
                else if (!strcmp(hedgeArray[j], "generally"))
                    hedgeIndex = SA_HEDGE_GENERALLY;
                else if (!strcmp(hedgeArray[j], "not"))
                    hedgeIndex = SA_HEDGE_NOT;
                else if (!strcmp(hedgeArray[j], "positively"))
                    hedgeIndex = SA_HEDGE_POSITIVE;
                else if (!strcmp(hedgeArray[j], "slightly"))
                    hedgeIndex = SA_HEDGE_SLIGHTLY;
                else if (!strcmp(hedgeArray[j], "somewhat"))
                    hedgeIndex = SA_HEDGE_SOMEWHAT;
                else if (!strcmp(hedgeArray[j], "very"))
                    hedgeIndex = SA_HEDGE_VERY;
                else if (!strcmp(hedgeArray[j], "vicinity"))
                    hedgeIndex = SA_HEDGE_VICINITY;

                if (hedgeIndex != SA_HEDGE_ERROR)
                    semanticTerm[thisSet] = saHedgeApply(hedgeIndex, semanticTerm[thisSet]);
                else
                {
                    fprintf(stderr, "xsDisplaySemanticTerm-F-119: No such hedge %s\n", hedgeArray[j]);
                    exit(0);
                }
            }
            i--;
        }
        finalSetName[thisSet] = malloc(512);
        char *tmp = finalSetName[thisSet];
        strcpy(tmp, "\"");
        for(i=0; i<numSemanticTerms; i++)    
        {
            if (*setArray[i] != '_')
            {
                strcat(tmp, setArray[i]);
                strcat(tmp, " ");
            }
        }
        tmp[strlen(tmp)-1] = '"';
        
    }
    fprintf(stdout, "domain");
    for(i=0; i<numSets; i++)
        fprintf(stdout, ",%s", finalSetName[i], stdout);
    fputs("\n", stdout);
    for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
    {
        fprintf(stdout, "%d", i);
        int j;
        for(j=0; j<numSets; j++)
            fprintf(stdout, ",%1.10f", semanticTerm[j]->vector[i]);
        fputs("\n", stdout);
    }

    exit(0);
}
