/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software
 contains proprietary trade and business secrets.

 Program: xsPerformLinearRegression

 Usage: xsPerformLinearRegression [-f output_file]
        -f the name of the file to write the output

 Description:
        Take the results from xspreregress to generate a linear regression algorithm.

        The algorithm is a*fieldX + b
*/
#include <libgen.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "saCSV3.h"
#include "saCSV.h"
#include "saIndex.h"
#include "saSignal.h"
#include "saSplunk.h"

#define MAXROWSIZE 256
#define MAXSTRING 1024
static char inbuf[MAXSTRING*1000];
static char tempbuf[MAXSTRING*10];
static char *fieldList[MAXROWSIZE*4];

static char *byF[MAXROWSIZE*100];
static char *byV[MAXROWSIZE*100];
static char *X[MAXROWSIZE];
static char *Y[MAXROWSIZE];
static double A[MAXROWSIZE];
static double B[MAXROWSIZE];
static double errA[MAXROWSIZE];
static double errB[MAXROWSIZE];
static double R[MAXROWSIZE];
static int numRows[MAXROWSIZE];

static saCSVType csv;

extern inline char *saSplunkGetRoot(char *);
extern inline saSplunkInfoPtr saSplunkLoadHeader();
extern inline bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    char outfile[256];

    // Initialize the system
    initSignalHandler(basename(argv[0]));
    outfile[0] = '\0';
    int c;
    bool argError = false;

    // Get the arguments
    while ((c = getopt(argc, argv, "f:")) != -1) 
    {
        switch(c)
        {
            case 'f':
                strcpy(outfile, optarg);
                break;
            case '?':
                fprintf(stderr, "xsPerformLinearRegression-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError == true)
    {
        fprintf(stderr, "xsPerformLinearRegression-F-103: Usage: xsPerformLinearRegression [-f file]\n");
        exit(0);
    }

    // Load the Splunk Header information to get app and user
    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsDisplayContext-F-105: Can't get info header\n");
        exit(EXIT_FAILURE);
    }
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsDisplayContext-F-105: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    // Initialize the array
    int numFields;
    int i;
    for(i=0; i<MAXROWSIZE; i++)
    {
        numRows[i] = 0;
        R[i] = 0.0;
        X[i] = NULL;
        Y[i] = NULL;
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
    int yIndex = -1;
 
    // open stream to read CSV
    saCSVOpen(&csv, stdin);
 
    // Get the header
    numFields = saCSV3GetLine(&csv, inbuf, fieldList);
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
    }

    // load the data 
    int maxIndex = 0;
    while(saCSVEOF(&csv) == false)
    {
        int index = -1;
        numFields = saCSV3GetLine(&csv, inbuf, fieldList);
        if (saCSVEOF(&csv) == false)
        {
            // Determine if the BY index already exists.  If not, then create it
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

            // If there's actual row values associated with the BY index, then aggregate the information in
            // a wieghted fashion 
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
 
                numRows[index] = numRows[index] + rowCount;
                R[index] = R[index] + (thisR * rowCount);
                A[index] = A[index] + (thisA * rowCount);
                B[index] = B[index] + (thisB * rowCount);
                errA[index] = errA[index] + (thisErrA * rowCount);
                errB[index] = errB[index] + (thisErrB * rowCount);
                
                if (index > maxIndex)
                    maxIndex = index;
            }
        }
    }
 
    // if the outfile is specified then write results
    char tempDir[512];
    FILE *f = NULL;
    if (outfile[0] != '\0')
    {
        sprintf(tempDir, "%s/apps/%s/lookups/%s.csv", saSplunkGetRoot(argv[0]), p->app, outfile);
        f = fopen(tempDir, "w");
    }

    // Write the header
    if (f != NULL)
        fputs("x,y,bf,bv,numRows,slope,intercept,errA,errB,R\n", f);
    fputs("x,y,bf,bv,numRows,slope,intercept,errA,errB,R\n", stdout);
    
    // Determine the weighted avg of R
    for(i=0; i<=maxIndex; i++)
    {
        R[i] = R[i] / (float)numRows[i];
        A[i] = A[i] / (float)numRows[i];
        B[i] = B[i] / (float)numRows[i];
        errA[i] = errA[i] / (float)numRows[i];
        errB[i] = errB[i] / (float)numRows[i];

        // Write the data 
        if (f != NULL) 
            fprintf(f, "%s,%s,%s,%s,%d,%.10f,%.10f,%.10f,%.10f,%.10f\n", X[i], Y[i], byF[i], byV[i],
                    numRows[i], A[i], B[i], errA[i], errB[i], R[i]);
        fprintf(stdout, "%s,%s,%s,%s,%d,%.10f,%.10f,%.10f,%.10f,%.10f\n", X[i], Y[i], byF[i], byV[i],
                numRows[i], A[i], B[i], errA[i], errB[i], R[i]);
    }
    if (f != NULL)
        fclose(f);
}

