/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsPerformAutoRegression

 Usage: xsPerformAutoRegression [-f output_file]
        -f the name of the file to write the output

 Description:
        Take the results from xspreautoregress to generate an auto regression algorithm.

        The algorithm is coef0*fieldX*fieldX + coef1*fieldX + coef2
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
static double coef0[MAXROWSIZE];
static double coef1[MAXROWSIZE];
static double coef2[MAXROWSIZE];
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
                fprintf(stderr, "xsPerformAutoRegression-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError == true)
    {
        fprintf(stderr, "xsPerformAutoRegression-F-103: Usage: xsPerformAutoRegression [-f output_file]\n");
        exit(0);
    }

    // Load the Splunk Header information to get app & user
    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsPerformAutoRegression-F-105: Can't get info header\n");
        exit(EXIT_FAILURE);
    }
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsPerformAutoRegression-F-105: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    // Initialize the arrays
    int numFields;
    int i;
    for(i=0; i<MAXROWSIZE; i++)
    {
        numRows[i] = 0;
        X[i] = NULL;
    }
 
    int byFIndex = -1;
    int byVIndex = -1;
    int coef0Index = -1;
    int coef1Index = -1;
    int coef2Index = -1;
    int numRowsIndex = -1;
    int xIndex = -1;
 
    // open stream for CSV
    saCSVOpen(&csv, stdin);
 
    // Get the header
    numFields = saCSV3GetLine(&csv, inbuf, fieldList);
    for(i=0; i<numFields; i++)
    {
        if (!saCSVCompareField(fieldList[i], "bf"))
            byFIndex = i;
        else if (!saCSVCompareField(fieldList[i], "bv"))
            byVIndex = i;
        else if (!saCSVCompareField(fieldList[i], "coef0"))
            coef0Index = i;
        else if (!saCSVCompareField(fieldList[i], "coef1"))
            coef1Index = i;
        else if (!saCSVCompareField(fieldList[i], "coef2"))
            coef2Index = i;
        else if (!saCSVCompareField(fieldList[i], "numRows"))
            numRowsIndex = i;
        else if (!saCSVCompareField(fieldList[i], "x"))
             xIndex = i;
    }
 
    // Read the data
    int maxIndex = 0;
    while(saCSVEOF(&csv) == false)
    {
        int index = -1;
        numFields = saCSV3GetLine(&csv, inbuf, fieldList);
        if (saCSVEOF(&csv) == false)
        {
            // Determine if the BY index already exists.  If not, then create it
            index = getIndex(fieldList[xIndex], "", fieldList[byFIndex], fieldList[byVIndex]);
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
 
            // Load the data for coefs into the correct BY index
            int rowCount = atoi(fieldList[numRowsIndex]);
            if (rowCount > 0)
            {
                double thisCoef0 = 0.0;
                if (strcmp(saCSVExtractField(fieldList[coef0Index]), "nan") &&
                    strcmp(saCSVExtractField(fieldList[coef0Index]), "-nan"))
                    thisCoef0 = atof(saCSVExtractField(fieldList[coef0Index]));
 
                double thisCoef1 = 0.0;
                if (strcmp(saCSVExtractField(fieldList[coef1Index]), "nan") &&
                    strcmp(saCSVExtractField(fieldList[coef1Index]), "-nan"))
                thisCoef1 = atof(saCSVExtractField(fieldList[coef1Index]));
 
                double thisCoef2 = 0.0;
                if (strcmp(saCSVExtractField(fieldList[coef2Index]), "nan") &&
                    strcmp(saCSVExtractField(fieldList[coef2Index]), "-nan"))
                thisCoef2 = atof(saCSVExtractField(fieldList[coef2Index]));
                numRows[index] = numRows[index] + rowCount;
 
                coef0[index] = coef0[index] + (thisCoef0 * rowCount);
                coef1[index] = coef1[index] + (thisCoef1 * rowCount);
                coef2[index] = coef2[index] + (thisCoef2 * rowCount);
                
                if (index > maxIndex)
                    maxIndex = index;
            }
        }
    }
 
    // Write out the file if specified
    char tempDir[512];
    FILE *f = NULL;
    if (outfile[0] != '\0')
    {
        sprintf(tempDir, "%s/apps/%s/lookups/%s.csv", saSplunkGetRoot(argv[0]), p->app, outfile);
        f = fopen(tempDir, "w");
    }
 
    // write out the results
    if (f != NULL)
        fputs("x,bf,bv,numRows,coef0,coef1,coef2\n", f);
    fputs("x,bf,bv,numRows,coef0,coef1,coef2\n", stdout);
 
    
    // Determine the weighted avg of R2
    for(i=0; i<=maxIndex; i++)
    {
        coef0[i] = coef0[i] / (float)numRows[i];
        coef1[i] = coef1[i] / (float)numRows[i];
        coef2[i] = coef2[i] / (float)numRows[i];
 
        // Write data to output stream 
        if (f != NULL) 
            fprintf(f, "%s,%s,%s,%d,%.10f,%.10f,%.10f\n", X[i], byF[i], byV[i],
                    numRows[i], coef0[i], coef1[i], coef2[i]);
        fprintf(stdout, "%s,%s,%s,%d,%.10f,%.10f,%.10f\n", X[i], byF[i], byV[i],
                numRows[i], coef0[i], coef1[i], coef2[i]);
    }
    if (f != NULL)
        fclose(f);
}
  
