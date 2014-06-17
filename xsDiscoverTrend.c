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
#include "saLicensing.h"
#include "saSignal.h"
#include "saSplunk.h"
#include "saUtils.h"

#define MAXROWSIZE 256

static char *byF[MAXROWSIZE];
static char *byV[MAXROWSIZE];
static char inbuf[MAXROWSIZE];
static char tempbuf[MAXROWSIZE];
static char *fieldList[MAXROWSIZE];

static double A[MAXROWSIZE];
static double B[MAXROWSIZE];
static double end[MAXROWSIZE];
static double errA[MAXROWSIZE];
static double errB[MAXROWSIZE];
static double R[MAXROWSIZE];
static double start[MAXROWSIZE];
static double xLow[MAXROWSIZE];
static double xHigh[MAXROWSIZE];
static double yLow[MAXROWSIZE];
static double yHigh[MAXROWSIZE];
static char *X[MAXROWSIZE];
static char *Y[MAXROWSIZE];
static int numRows[MAXROWSIZE];

static char *indexString[MAXROWSIZE];
static int numIndexes = 0;

extern double saConceptLookup(saConceptTypePtr, double);
extern saContextTypePtr saSplunkContextLoad(char *, char *, int *, char *, char *);
extern char *saSplunkGetRoot(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

char *findElementInList(char *, char *[], char *[], int listLength);
char *getField(char *);
int getIndex(int, int, int, int);
void printLine(char *[], int);

int main(int argc, char* argv[]) 
{
    char *contextList[32];
    char contextListString[512];
    char *root = saSplunkGetRoot(argv[0]);
    char *yList[32];
    char yListString[512];
    int c;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    contextListString[0] = '\0';
    bool argError = false;
    while ((c = getopt(argc, argv, "c:y:")) != -1) 
    {
        switch (c)
        {
            case 'c':
                strcpy(contextListString, optarg);
                break;
            case 'y':
                strcpy(yListString, optarg);
                break;
            case '?':
                fprintf(stderr, "xsDiscoverTrend-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError == true)
    {
        fprintf(stderr, "xsDiscoverTrend-F-103: Usage: xsDiscoverTrend -c contextList\n");
        exit(0);
    }

    if (contextListString[0] == '\0')
    {
        fprintf(stderr, "xsDiscoverTrend-F-113: no contexts specified\n");
        exit(0);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsDiscoverTrend-F-115: Can't open header\n");
        exit(0);
    }
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsDiscoverTrend-F-117: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    int numContexts = 0;
    char *t = contextListString;
    char *u = yListString;
    while ((contextList[numContexts] = strsep(&t, ",")) != NULL)
    {
            yList[numContexts] = strsep(&u, ",");
            numContexts++;
    }

    int i;
    int numFields;
    for(i=0; i<MAXROWSIZE; i++)
    {
        numRows[i] = 0;
        R[i] = 0.0;
        X[i] = NULL;
        Y[i] = NULL;
        xLow[i] = 1.79769e+308;
        xHigh[i] = 2.22507e-308;
        yLow[i] = 0.0;
        yHigh[i] = 0.0;
    }
 
    int aIndex = -1;
    int bIndex = -1;
    int byFIndex = -1;
    int byVIndex = -1;
    int errAIndex = -1;
    int errBIndex = -1;
    int numRowsIndex = -1;
    int r2Index = -1;
    int xIndex = -1;
    int xLowIndex = -1;
    int xHighIndex = -1;
    int yIndex = -1;
    int yLowIndex = -1;
    int yHighIndex = -1;
 
    // Get the header
    numFields = saCSVGetLine(inbuf, fieldList);
    for(i=0; i<numFields; i++)
    {
        if (!saCSVCompareField(fieldList[i], "bf"))
            byFIndex = i;
        else if (!saCSVCompareField(fieldList[i], "bv"))
            byVIndex = i;
        else if (!saCSVCompareField(fieldList[i], "slope"))
            aIndex = i;
        else if (!saCSVCompareField(fieldList[i], "intercept"))
            bIndex = i;
        else if (!saCSVCompareField(fieldList[i], "errA"))
            errAIndex = i;
        else if (!saCSVCompareField(fieldList[i], "errB"))
            errBIndex = i;
        else if (!saCSVCompareField(fieldList[i], "numRows"))
            numRowsIndex = i;
        else if (!saCSVCompareField(fieldList[i], "R"))
            r2Index = i;
        else if (!saCSVCompareField(fieldList[i], "x"))
             xIndex = i;
        else if (!saCSVCompareField(fieldList[i], "y"))
             yIndex = i;
        else if (!saCSVCompareField(fieldList[i], "xLow"))
             xLowIndex = i;
        else if (!saCSVCompareField(fieldList[i], "xHigh"))
             xHighIndex = i;
        else if (!saCSVCompareField(fieldList[i], "yLow"))
             yLowIndex = i;
        else if (!saCSVCompareField(fieldList[i], "yHigh"))
             yHighIndex = i;
    }
 
    int maxIndex = 0;
    while(!feof(stdin))
    {
        int index = -1;
        numFields = saCSVGetLine(inbuf, fieldList);
        if (!feof(stdin))
        {
            index = getIndex(xIndex, yIndex, byFIndex, byVIndex);
            if (byF[index] == NULL)
            {
                byF[index] = malloc(strlen(fieldList[byFIndex]));
                strcpy(byF[index], fieldList[byFIndex]);
            }
            if (byV[index] == NULL)
            {
                byV[index] = malloc(strlen(fieldList[byVIndex]));
                strcpy(byV[index], fieldList[byVIndex]);
            }
            if (X[index] == NULL)
            {
                X[index] = malloc(strlen(fieldList[xIndex]));
                strcpy(X[index], fieldList[xIndex]);
            }
            if (Y[index] == NULL)
            {
                Y[index] = malloc(strlen(fieldList[yIndex]));
                strcpy(Y[index], fieldList[yIndex]);
            }
 
            int rowCount = atoi(fieldList[numRowsIndex]);
            if (rowCount > 0)
            {
                double thisR = 0.0;
                if (strcmp(getField(fieldList[r2Index]), "nan") &&
                    strcmp(getField(fieldList[r2Index]), "-nan")) 
                    thisR = atof(getField(fieldList[r2Index]));
                double thisA = 0.0;
                if (strcmp(getField(fieldList[aIndex]), "nan") &&
                    strcmp(getField(fieldList[aIndex]), "-nan")) 
                    thisA = atof(getField(fieldList[aIndex]));
                double thisB = 0.0;
                if (strcmp(getField(fieldList[bIndex]), "nan") &&
                    strcmp(getField(fieldList[bIndex]), "-nan")) 
                    thisB = atof(getField(fieldList[bIndex]));
                double thisErrA = 0.0;
                if (strcmp(getField(fieldList[errAIndex]), "nan") &&
                    strcmp(getField(fieldList[errAIndex]), "-nan")) 
                    thisErrA = atof(getField(fieldList[errAIndex]));
                double thisErrB = 0.0;
                if (strcmp(getField(fieldList[errBIndex]), "nan") &&
                    strcmp(getField(fieldList[errBIndex]), "-nan")) 
                    thisErrB = atof(getField(fieldList[errBIndex]));
                double thisXLow = 0.0;
                if (strcmp(getField(fieldList[xLowIndex]), "nan") &&
                    strcmp(getField(fieldList[xLowIndex]), "-nan")) 
                    thisXLow = atof(getField(fieldList[xLowIndex]));
                double thisXHigh = 0.0;
                if (strcmp(getField(fieldList[xHighIndex]), "nan") &&
                    strcmp(getField(fieldList[xHighIndex]), "-nan")) 
                    thisXHigh = atof(getField(fieldList[xHighIndex]));
                double thisYLow = 0.0;
                if (strcmp(getField(fieldList[yLowIndex]), "nan") &&
                    strcmp(getField(fieldList[yLowIndex]), "-nan")) 
                    thisYLow = atof(getField(fieldList[yLowIndex]));
                double thisYHigh = 0.0;
                if (strcmp(getField(fieldList[yHighIndex]), "nan") &&
                    strcmp(getField(fieldList[yHighIndex]), "-nan")) 
                    thisYHigh = atof(getField(fieldList[yHighIndex]));
 
                numRows[index] = numRows[index] + rowCount;
                R[index] = R[index] + (thisR * rowCount);
                A[index] = A[index] + (thisA * rowCount);
                B[index] = B[index] + (thisB * rowCount);
                errA[index] = errA[index] + (thisErrA * rowCount);
                errB[index] = errB[index] + (thisErrB * rowCount);
 
                if (xLow[index] > thisXLow)
                    xLow[index] = thisXLow;
                if (xHigh[index] < thisXHigh)
                    xHigh[index] = thisXHigh;
                if (yLow[index] > thisYLow)
                    yLow[index] = thisYLow;
                if (yHigh[index] < thisYHigh)
                    yHigh[index] = thisYHigh;
 
                if (index > maxIndex)
                    maxIndex = index;
            }
        }
    }
    // write out the field headers
    fprintf(stdout, "x,y,bf,bv,concept,trend\n");
 
    // Determine the weighted avg of R
    for(i=0; i<=maxIndex; i++)
    {
        R[i] = R[i] / (float)numRows[i];
        A[i] = A[i] / (float)numRows[i];
        B[i] = B[i] / (float)numRows[i];
        errA[i] = errA[i] / (float)numRows[i];
        errB[i] = errB[i] / (float)numRows[i];
        double lY = A[i]*xLow[i] + B[i];
        double hY = A[i]*xHigh[i] + B[i];
 
        // load the Context for Y
        // using linear regression, calculate the lowest Y from lowest X
        // using linear regression, calculate the highest Y from highest X
        // for each set in the fuzzy Term Set
        //     lookup the the lowest and highest Y in the set
        //     subtract the lowcix from the highcix
        //     write out the results (example):
        //     _time,ArrDelay,low,-0.5
        char *cName = findElementInList(Y[i], yList, contextList, numContexts);
        int scope = SA_SPLUNK_SCOPE_NONE;
        saContextTypePtr contextPtr = saSplunkContextLoad(cName, root, &scope, p->app, p->user);
        if (contextPtr == NULL)
        {
            fprintf(stderr, "xsDiscoverTrend-F-111: can't open context %s\n", cName);
            exit(0);
        }
        int j;
        for(j=0; j<contextPtr->numConcepts; j++)
        {
            double lCix = saConceptLookup(contextPtr->concepts[j], lY);
            double hCix = saConceptLookup(contextPtr->concepts[j], hY);
            //
            // Modified trend to give a much less diluted value
            // double trend = (hCix-lCix)/(float)numRows[i];
            double trend = (hCix-lCix);
            fprintf(stdout, "%s,%s,%s,%s,%s,%.10f\n", X[i], Y[i], byF[i], byV[i], 
                    contextPtr->concepts[j]->name, trend);
        }
    }
}

char *findElementInList(char *str, char *list1[], char *list2[], int listLength)
{
    int i=0;
    while(i<listLength)
    {
        if(!strcmp(str, list1[i]))
           return(list2[i]);
        i++;
    }
    return(NULL);
}
 
char *getField(char *field)
{
   if (*field == '"')
   {
       strcpy(tempbuf, field+1);
       tempbuf[strlen(field)-2] = '\0';
       return(tempbuf);
   }
   else
       return(field);
}

int getIndex(int xIndex, int yIndex, int byFIndex, int byVIndex)
{

   sprintf(tempbuf, "%s,%s,%s,%s", fieldList[xIndex], fieldList[yIndex], fieldList[byFIndex],
           fieldList[byVIndex]);
   bool found = false;
   int i=0;
   while(i<numIndexes && !found)
   {
       if (!strcmp(tempbuf, indexString[i]))
           found = true;
       else
           i++;
   }
   if (!found)
   {
       indexString[i] = malloc(strlen(tempbuf)+1);
       strcpy(indexString[i], tempbuf);
       numIndexes++;
   }
   return(i);
}

void printLine(char *fieldList[], int numFields)
{
   FILE *x = fopen("./x", "a");
   int i;
   for(i=0; i<numFields; i++)
   {
       if (!i)
           fputs(fieldList[i], x);
       else
           fprintf(x, ",%s", fieldList[i]);
   }
   fputs("\n", x);
}
