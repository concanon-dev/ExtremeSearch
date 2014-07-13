/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsAggregateCorrelation

 Usage: xsAggregateCorrelation [-f output_file]

 Description:
        Aggregates (combines) correlation algorithms that have the same X field, Y field, BY field and
        BY Value into a new algorithm. Takes input from the output of previous xsPerform commands. The 
        algorithms are matched by field and a weighted average (by count) is used to aggregate the  
        results.  This can be used to aggregate the results of minor periods into more major periods. An
        effective way to specify the input data stream is to pipe the output of inputlookup.

        If -f is specified, write the output to 'output_file' as well.  This file is located in 
        saSplunkGetRoot(argv[0])/apps/<app>/lookups/output_file.csv.
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

static char *byF[MAXROWSIZE];
static char *byV[MAXROWSIZE];
static char *X[MAXROWSIZE];
static char *Y[MAXROWSIZE];
static double R[MAXROWSIZE];
static int numRows[MAXROWSIZE];

static saCSVType csv;

extern inline char *saSplunkGetRoot(char *);
extern inline saSplunkInfoPtr saSplunkLoadHeader();
extern inline bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    char outfile[256];

    // initialize system
    initSignalHandler(basename(argv[0]));
    outfile[0] = '\0';
    int c;
    bool argError = false;

    // get arguments
    while ((c = getopt(argc, argv, "f:")) != -1) 
    {
        switch(c)
        {
            case 'f':
                strcpy(outfile, optarg);
                break;
            case '?':
                fprintf(stderr, "xsAggregateCorrelation-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError == true)
    {
        fprintf(stderr, "xsAggregateCorrelation-F-103: Usage: xsAggregateCorrelation [-f output_file]\n");
        exit(0);
    }

    // Load the Splunk Header information to determine app and user
    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsAggregateCorrelation-F-105: Can't get info header\n");
        exit(EXIT_FAILURE);
    } 
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsAggregateCorrelation-F-105: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    // Initialize Arrays
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
            // See if there is already a reference to the class.  A class is the tuple
            // formed by the values of the fields "x", "bf" and "bv".  This is used to
            // make sure that the correct rows are added together correctly in a weighted
            // fashion.  The weight is the count, the number of events that contribute to
            // the algorithm.
            index = getIndex(fieldList[xIndex], fieldList[yIndex], fieldList[byFIndex], 
                             fieldList[byVIndex]);
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
                double thisR = atof(saCSVExtractField(fieldList[r2Index]));
                numRows[index] = numRows[index] + rowCount;
 
                R[index] = R[index] + (thisR * rowCount);
                
                if (index > maxIndex)
                    maxIndex = index;
            }
        }
    }

    // if the outfile has been specified, open the output file 
    char tempDir[512];
    FILE *f = NULL;
    if (outfile[0] != '\0')
    {
        sprintf(tempDir, "%s/apps/%s/lookups/%s.csv", saSplunkGetRoot(argv[0]), p->app, outfile);
        f = fopen(tempDir, "w");
    } 

    // write the headers
    if (f != NULL)
        fputs("x,y,bf,bv,numRows,R\n", f);
    fputs("x,y,bf,bv,numRows,R\n", stdout);
    
    // Determine the weighted avg of R
    for(i=0; i<=maxIndex; i++)
    {
        R[i] = R[i] / (float)numRows[i];
  
        if (f != NULL)
            fprintf(f, "%s,%s,%s,%s,%d,%.10f\n", X[i], Y[i], byF[i], byV[i], numRows[i], R[i]);
        fprintf(stdout, "%s,%s,%s,%s,%d,%.10f\n", X[i], Y[i], byF[i], byV[i], numRows[i], R[i]);
    }
    if (f != NULL)
        fclose(f);
}
 
