/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsDeleteConcept

 Usage: xsDeleteConcept -c contextList [-s scope] -t concept
    -c a comma separated list of Contexts corresponding to the field(s) to be analyzed
    -y a comma separated list of fields to be analyzed

    The number of elements in y must be the same as the number of elements in c.

 Description:
    Discover any trend of y_field(s) over x_field(s) using linear regression. The trend is determined
    across the context associated with the field. This context of the y_field is a prerequisite for the
    command to run.  Trend is calculated as follows: Take the lowest X value and calculate the Y value,
    take the the highest X value and calcuate the Y value. For each concept in the Y Context, calculate
    the CIX for the low and high Y. The trend for each concept is (hi_Y_CIX - low_Y_CIX) / numRows.
    For each concept, the following columns are written: x,y,bf,bv,concept,trend.

    This command is similar to xsPerformLinearRegression in it's form and function.  However, it adds on
    the trend analysis functionality as described above.
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
#include "saCSV3.h"
#include "saIndex.h"
#include "saSignal.h"
#include "saSplunk.h"
#include "strsep.h"

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

static saCSVType csv;

extern double saConceptLookup(saConceptTypePtr, double);
extern inline char *saCSVExtractField(char *);
extern saContextTypePtr saSplunkContextLoad(char *, char *, int *, char *, char *);
extern inline char *saSplunkGetRoot(char *);
extern inline saSplunkInfoPtr saSplunkLoadHeader();
extern inline bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

inline char *findElementInList(char *, char *[], char *[], int listLength);

int main(int argc, char* argv[]) 
{
    char *contextList[32];
    char contextListString[512];
    char *root = saSplunkGetRoot(argv[0]);
    char *yList[32];
    char yListString[512];
    int c;

    initSignalHandler(basename(argv[0]));
    contextListString[0] = '\0';
    bool argError = false;
    while ((c = getopt(argc, argv, "c:y:")) != -1) 
    {
        switch(c)
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
        fprintf(stderr, "xsDiscoverTrend-F-103: Usage: xsDiscoverTrend -c contextList -y fieldList\n");
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

    // open stream for CSV
    saCSVOpen(&csv, stdin);
 
    // Get the header
    numFields = saCSV3GetLine(&csv, inbuf, fieldList);
    if (numFields == 0)
        exit(0);

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
    while(saCSVEOF(&csv) == false)
    {
        int index = -1;
        numFields = saCSV3GetLine(&csv, inbuf, fieldList);
        if (saCSVEOF(&csv) == false)
        {
            index = getIndex(fieldList[xIndex], fieldList[yIndex], fieldList[byFIndex], fieldList[byVIndex]);
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
                if (strcmp(saCSVExtractField(fieldList[r2Index]), "nan") &&
                    strcmp(saCSVExtractField(fieldList[r2Index]), "-nan")) 
                    thisR = atof(saCSVExtractField(fieldList[r2Index]));
                double thisA = 0.0;
                if (strcmp(saCSVExtractField(fieldList[aIndex]), "nan") &&
                    strcmp(saCSVExtractField(fieldList[aIndex]), "-nan")) 
                    thisA = atof(saCSVExtractField(fieldList[aIndex]));
                double thisB = 0.0;
                if (strcmp(saCSVExtractField(fieldList[bIndex]), "nan") &&
                    strcmp(saCSVExtractField(fieldList[bIndex]), "-nan")) 
                    thisB = atof(saCSVExtractField(fieldList[bIndex]));
                double thisErrA = 0.0;
                if (strcmp(saCSVExtractField(fieldList[errAIndex]), "nan") &&
                    strcmp(saCSVExtractField(fieldList[errAIndex]), "-nan")) 
                    thisErrA = atof(saCSVExtractField(fieldList[errAIndex]));
                double thisErrB = 0.0;
                if (strcmp(saCSVExtractField(fieldList[errBIndex]), "nan") &&
                    strcmp(saCSVExtractField(fieldList[errBIndex]), "-nan")) 
                    thisErrB = atof(saCSVExtractField(fieldList[errBIndex]));
                double thisXLow = 0.0;
                if (strcmp(saCSVExtractField(fieldList[xLowIndex]), "nan") &&
                    strcmp(saCSVExtractField(fieldList[xLowIndex]), "-nan")) 
                    thisXLow = atof(saCSVExtractField(fieldList[xLowIndex]));
                double thisXHigh = 0.0;
                if (strcmp(saCSVExtractField(fieldList[xHighIndex]), "nan") &&
                    strcmp(saCSVExtractField(fieldList[xHighIndex]), "-nan")) 
                    thisXHigh = atof(saCSVExtractField(fieldList[xHighIndex]));
                double thisYLow = 0.0;
                if (strcmp(saCSVExtractField(fieldList[yLowIndex]), "nan") &&
                    strcmp(saCSVExtractField(fieldList[yLowIndex]), "-nan")) 
                    thisYLow = atof(saCSVExtractField(fieldList[yLowIndex]));
                double thisYHigh = 0.0;
                if (strcmp(saCSVExtractField(fieldList[yHighIndex]), "nan") &&
                    strcmp(saCSVExtractField(fieldList[yHighIndex]), "-nan")) 
                    thisYHigh = atof(saCSVExtractField(fieldList[yHighIndex]));
 
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
        // for each concept in the Context
        //     lookup the the lowest and highest Y in the concept
        //     subtract the lowcix from the highcix
        //     write out the results (example):
        //     _time,ArrDelay,low,-0.5
        char *cName = findElementInList(Y[i], yList, contextList, numContexts);
        if (cName != NULL)
        {
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
}

inline char *findElementInList(char *str, char *list1[], char *list2[], int listLength)
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
 
