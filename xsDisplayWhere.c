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
#include "saCSV.h"
#include "saExpression.h"
#include "saHash.h"
#include "saHedge.h"
#include "saLicensing.h"
#include "saSignal.h"
#include "saSplunk.h"
#include "saToken.h"

#define FCIX "xsGetWhereCix"
#define FCIX_AVG "avg"
#define FCIX_WEIGHTED "weighted"

#define MAXROWSIZE (1024*500)

static bool calledXcix = false;
static bool termSetFound[32];
static char cixName[256] = FCIX;
static char *fieldList[MAXROWSIZE / 32];
static char headerbuf[MAXROWSIZE];
static char *headerList[MAXROWSIZE / 31];
static char inbuf[MAXROWSIZE];
static char tempbuf[MAXROWSIZE];
static char whereLine[MAXROWSIZE / 32];
static saHashtableTypePtr conceptTable;
static int numHeaderFields = 0;
static int termSetFieldIndex[MAXROWSIZE / 32];

static saSplunkInfoPtr p = NULL;

static saSynonymTableType synonyms;

extern FILE *saOpenFile(char *, char *);

extern char *optarg;
extern int optind, optopt;

extern saHashtableTypePtr saHashCreateDefault();
extern void *saHashGet(saHashtableTypePtr, char *);
extern void saHashSet(saHashtableTypePtr, char *, char *);

extern saConceptTypePtr saConceptCreatePI(char *, double, double, double, double, double);

extern saExpressionTypePtrArray generateExpStack(void);
static bool runExpressionStack(char *[], int, saExpressionTypePtrArray, int, float, float, char [],
                               double *);
extern void walkExpressionStack(saExpressionTypePtrArray, int);    

extern saConceptTypePtr saHedgeApply(int, saConceptTypePtr);
extern double saConceptLookup(saConceptTypePtr, double);
extern bool saHedgeLoadLookup(FILE *, saSynonymTableTypePtr);
extern char *saHedgeLookup(saSynonymTableTypePtr, char *);
extern int saParserParse(char *, char [], saExpressionTypePtrArray);
extern saContextTypePtr saSplunkContextLoad(char *, int *, char *, char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    saContextTypePtr termSetPtr;
    bool argError;
    bool foundAnyField;
    char cixFunction[80];
    char synonymFileName[80];
    float alfacut=0.2;
    float scalar_percent=0.05;
    int c;
    int cixIndex = -1;
    int numFields;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    conceptTable = saHashCreateDefault();

    /* determine if this is being called as FCIX */
    if (!strcmp(basename(argv[0]), FCIX))
        calledXcix = true;

    argError = false;
    synonymFileName[0] = '\0';
    synonyms.numWords = 0;
    strcpy(cixFunction, FCIX_AVG);
    while ((c = getopt(argc, argv, "a:c:n:p:s:w:")) != -1) 
    {
        switch (c)
        {
            case 'a':
                alfacut = atof(optarg);
                break;
            case 'c':
                strcpy(cixFunction, optarg);
                break;
            case 'n':
                strcpy(cixName, optarg);
                break;
            case 'p':
                scalar_percent = atof(optarg);
                break;
            case 's':
                strcpy(synonymFileName, optarg);
                break;
            case 'w':
                strcpy(whereLine, optarg);
                break;
            case '?':
                fprintf(stderr, "xsWhere-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsWhere-F-103: Usage: xsrWhere -a alfacut -c cixFunction -n cixName -p scalar_percent -s synonymsFile -u useAlfa -w whereLine\n");
        exit(EXIT_FAILURE);
    }

    // read the header information
    p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsWhere-F-125: Can't get header info\n");
        exit(EXIT_FAILURE);
    }

    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsWhere-F-127: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
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
                fprintf(stderr, "xsWhere-F-119: Error in loading synonyms file: %s\n",
                        synonymFileName);
                exit(0);
            }
        }
    }

    // parse the where line
    saExpressionTypePtrArray expStack = generateExpStack();
    int stackSize = saParserParse(whereLine, tempbuf, expStack);
    if (stackSize < 0)
    {
        fprintf(stderr, "xsWhere-F-105: parse error: %s\n", tempbuf);
        exit(EXIT_FAILURE);
    }
    // Parse the first (header) line of input
    foundAnyField = false;
    numHeaderFields = saCSVGetLine(headerbuf, headerList);

    int i;
    // walk the stack to load the fuzzy sets
    for(i=0; i<stackSize; i++)
    {
        if (expStack[i]->type == SA_TOKEN_FIELD)
        {
            // Walk the fields in the header and see if they are called out in the where line
            int k = 0;
            bool done = false;
            while(done == false)
            {
                if (!saCSVCompareField(headerList[k], expStack[i]->field))
                {
                    expStack[i]->intvalue = k;
                    done = true;
                    foundAnyField = true;
                }
                else
                {
                    k++;
                    if (k == numHeaderFields)
                        done = true;
                }
            }
            if (k == numHeaderFields)
            {

                fprintf(stderr, "xsWhere-F-121: Field %s does not exist\n",
                        expStack[i]->field);
                exit(EXIT_FAILURE);
            }
        }
    }
    
    // Initialize the results array and read all of the input 
    double cix_avg;
    bool done = false;
    while(done == false)
    {
        int zzz = saCSVGetLine(inbuf, fieldList);
        if (!feof(stdin))
        {
            // if any of the fields in the where_line exist in the column list
            //    process the where_line execution stack
            if (foundAnyField == true)
                runExpressionStack(fieldList, numHeaderFields, expStack, stackSize, 
                                   alfacut, scalar_percent, cixFunction, &cix_avg);
        }
        else
            done = true;
    }

    fprintf(stdout, "set,index,cix\n");
    saHashentryTypePtr bucket = NULL;
    for(i=0; i<conceptTable->size; i++)
    {
        bucket = conceptTable->table[i];
        while(bucket != NULL)
        {
            saConceptTypePtr f = (saConceptTypePtr)bucket->value;
            int k;
            for(k=0; k<SA_CONCEPT_VECTORSIZE; k++)
                fprintf(stdout, "%s,%1.5f,%1.10f\n", bucket->key, f->indexVector[k], f->vector[k]);
            bucket = bucket->next;
        }
    }

    exit(0);
}

saConceptTypePtr processHedge(saConceptTypePtr f, saSynonymTableType *synonyms, 
                                   char *word)
{
    char *setArray[32];
    int numHedges = 0;
    char *hedgeWord = saHedgeLookup(synonyms, word);
    char *s = strtok(hedgeWord, " ");
    while(s != NULL)
    {
        setArray[numHedges++] = s;
        s = strtok(NULL, " ");
    }

    int i;
    for(i=numHedges-1; i>=0; i--)
    {
        s = setArray[i];
        int hedgeIndex = SA_HEDGE_ERROR;
        if (!strcmp(s, "_noise"))
            hedgeIndex = SA_HEDGE__NOISE;
        else if (!strcmp(s, "about"))
            hedgeIndex = SA_HEDGE_ABOUT;
        else if (!strcmp(s, "above"))
            hedgeIndex = SA_HEDGE_ABOVE;
        else if (!strcmp(s, "after"))
            hedgeIndex = SA_HEDGE_AFTER;                    
        else if (!strcmp(s, "before"))
            hedgeIndex = SA_HEDGE_BEFORE;
        else if (!strcmp(s, "below"))
            hedgeIndex = SA_HEDGE_BELOW;
        else if (!strcmp(s, "close"))
            hedgeIndex = SA_HEDGE_CLOSE;
        else if (!strcmp(s, "extremely"))
            hedgeIndex = SA_HEDGE_EXTREMELY; 
        else if (!strcmp(s, "generally"))
            hedgeIndex = SA_HEDGE_GENERALLY; 
        else if (!strcmp(s, "not"))
            hedgeIndex = SA_HEDGE_NOT;
        else if (!strcmp(s, "positively"))
            hedgeIndex = SA_HEDGE_POSITIVE;
        else if (!strcmp(s, "slightly"))
            hedgeIndex = SA_HEDGE_SLIGHTLY;
        else if (!strcmp(s, "somewhat"))
            hedgeIndex = SA_HEDGE_SOMEWHAT;
        else if (!strcmp(s, "very"))
            hedgeIndex = SA_HEDGE_VERY;
        else if (!strcmp(s, "vicinity"))
            hedgeIndex = SA_HEDGE_VICINITY;

        if (hedgeIndex == SA_HEDGE_ERROR)
            return(NULL);
        f = saHedgeApply(hedgeIndex, f);
    }
    return(f);
}

int removeStackElement(saExpressionTypePtrArray expStack, int stackSize, int fieldToRemove)
{
    int i;
    for(i=fieldToRemove;i<stackSize-1; i++)
        expStack[i] = expStack[i+1];
    return(stackSize-1);
}

bool runExpressionStack(char *fieldList[], int numFields, saExpressionTypePtrArray expStack,
                        int stackSize, float alfacut, float scalar_percent, char cixFunction[],
                        double *cix_avg)
{
    bool matches = false;
    bool results[stackSize];
    char tempName[256];
    double cix_sum = 0;

    results[0] = false;
    results[1] = false;
    int cix_count = 0;
    int resultsIndex = 0;
    int stackIndex = 0;
    while(stackIndex < stackSize)
    {
        if (expStack[stackIndex]->type == SA_TOKEN_AND)
        {
            results[resultsIndex-2] = ((results[resultsIndex-2] == true) 
                                       && (results[resultsIndex-1] == true)) ? true : false;
            resultsIndex--;
            stackIndex++;
        }
        else if (expStack[stackIndex]->type == SA_TOKEN_OR)
        {
            results[resultsIndex-2] = ((results[resultsIndex-2] == true) 
                                       || (results[resultsIndex-1] == true)) ? true : false;
            resultsIndex--;
            stackIndex++;
        }
        else
        {
            saConceptTypePtr concept = NULL;
            char *contextName = NULL;
            saContextTypePtr t = NULL;
            
            //
            // stack looks like this:  
            //          field, concept, [modifier, ... ] is 
            //

            // get the value of the field
            strcpy(tempbuf, fieldList[expStack[stackIndex++]->intvalue]);
            char *field = tempbuf;
            // strip off ""
            if (*tempbuf == '"')
            {
                field++;
                tempbuf[strlen(tempbuf)-1] = '\0';
            }
            double fieldValue = atof(field);
            
            // Load the fuzzy term set
            if (expStack[stackIndex]->type == SA_TOKEN_FUZZYTERMSET)
            {
                contextName = (char *)expStack[stackIndex]->field;
                stackIndex += 2;
                int i=0;
                bool found = false;
                while(!found && i<numHeaderFields)
                {
                      if (!saCSVCompareField(headerList[i], contextName))
                      {
                          contextName = saCSVExtractField(fieldList[i]);
                          found = true;
                      }
                      else
                          i++;
                }
                sprintf(tempName, "%s_%d", contextName, stackIndex);
            }
            else
            {
                contextName = (char *)expStack[stackIndex-1]->field;
                sprintf(tempName, "%s_%d", contextName, stackIndex-1);
            }
            
            // Find the fuzzy set in the cache or load it from the file
            concept = (saConceptTypePtr)saHashGet(conceptTable, tempName);
            if (concept == NULL)
            {
/*
    if context_name is a field,
       get field value
       if value is not in conceptSet cache
          build scalar conceptSet
    else if context_name is a scalar
          build scalar conceptSet
    else
       try to open the file contexName.context
*/
                char *dStr = (char *)expStack[stackIndex]->field;
                int i=0;
                bool found = false;
                while(!found && i<numHeaderFields)
                {
                      if (!saCSVCompareField(headerList[i], dStr))
                      {
                          dStr = fieldList[i];
                          found = true;
                      }
                      else
                          i++;
                }
                double d;
                if (saCSVConvertToDouble(dStr, &d) == true)
                {
                    int status;
                    double center, domainMin, domainMax, halfTerm;
                    double parms[4];
                    double domain[2];
                    if (d == 0.0)
                    {
                        center = 0.0;
                        domainMin = -0.2;
                        domainMax = 0.2;
                    }
                    else
                    {
                        center = d;
                        halfTerm = fabs(scalar_percent * d);
                        domainMin = center - halfTerm;
                        domainMax = center + halfTerm;
                    }
                    sprintf(tempName, "%s_%d", dStr, stackIndex);
                    concept = (saConceptTypePtr)saHashGet(conceptTable, tempName);
                    if (concept == NULL)
                    {
                        concept = saConceptCreatePI(dStr, domainMin, domainMax, domainMin,
                                                              domainMax, center);
                        if (concept == NULL)
                        {
                            fprintf(stderr, "xsDisplayWhere-F-123: can't create context %s\n", 
                                    dStr);
                            exit(0);
                        }
                    }

                }
                else
                {
                    int scope = SA_SPLUNK_SCOPE_NONE;
                    t = saSplunkContextLoad(contextName, &scope, p->app, p->user);
                    if (t == NULL)
                    {
                        fprintf(stderr, "xsWhere-F-111: can't open context %s\n", contextName);
                        exit(0);
                    }
                
                    // lookup the field in the fuzzy set
                    int m;
                    for(m=0; m<t->numConcepts; m++)
                        if (!strcmp(t->concepts[m]->name, expStack[stackIndex]->field))
                            concept = t->concepts[m];
                
                    if (concept == NULL)
                    {
                        fprintf(stderr, 
                               "xsWhere-F-113: concept '%s' does not exist in context '%s'\n",
                                expStack[stackIndex]->field, contextName);
                        exit(0);
                    }
                }
                while(expStack[++stackIndex]->type != SA_TOKEN_IS)
                    concept = processHedge(concept, &synonyms, 
                                                expStack[stackIndex]->field);
                saHashSet(conceptTable, tempName, (void *)concept);
            }
            else
                while(expStack[stackIndex]->type != SA_TOKEN_IS)
                    stackIndex++;
            
            double cix = saConceptLookup(concept, fieldValue);
            if (!strcmp(cixFunction, FCIX_WEIGHTED))
            {
                cix_sum += (cix*(stackSize-stackIndex));
                cix_count+= (stackSize-stackIndex);
            }
            else
            {
                cix_sum += cix;
                cix_count++;
            }
            if (cix > alfacut)
                matches = true;
            else 
                matches = false;
            stackIndex++;
            results[resultsIndex++] = matches;
        }
    }
    *cix_avg = cix_sum / (double)cix_count;
    return(results[0]);
}

