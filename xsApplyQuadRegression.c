/*
 Copyrigh 2012-2014 Scianta Analytics LLC   All Rights Reserved.
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software
 contains proprietary trade and business secrets.

 Program: xsApplyQuadRegression

 Usage: xsApplyQuadRegression -0 double -1 double -2 double -x fieldX

        -0 is coefficient 0
        -1 is coefficient 1
        -2 is coefficient 2
        -x is the X field to apply Quad Regression algorithm

 Description:
        Apply quad regression against a set of events containing the specified fields. The algorithm is
        derived from a series of 3 coefficients and one (X) field.

        The algorithm is coef0*fieldX*fieldX + coef1*fieldX + coef2
*/
#include <errno.h>
#include <libgen.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "csv3.h"
#include "saConstants.h"
#include "saCSV.h"

#include "saSignal.h"
#include "saSplunk.h"

static char inbuf[SA_CONSTANTS_MAXROWSIZE];
static char tempbuf[SA_CONSTANTS_MAXROWSIZE];
static char fileInBuf[SA_CONSTANTS_MAXAXIS+1][SA_CONSTANTS_MAXROWSIZE];
static char *fieldList[SA_CONSTANTS_MAXROWSIZE / 32];
static char *aList[SA_CONSTANTS_MAXAXIS];
static char *bList[SA_CONSTANTS_MAXAXIS];
static char *xList[SA_CONSTANTS_MAXAXIS];

static saCSVType csv;

extern char *optarg;
extern int optind, optopt;

extern inline saSplunkInfoPtr saSplunkLoadHeader();
extern inline bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    bool argError;
    char fieldX[SA_CONSTANTS_MAXSTRING];
    double coef0;
    double coef1;
    double coef2;
    int c;
    int numFields;
    int numFileHeader = 0;
    int numFileRows = 0;
    int regressIndex = -1;

    initSignalHandler(basename(argv[0]));  
    argError = false;
    while ((c = getopt(argc, argv, "0:1:2:x:")) != -1) 
    {
        switch(c)
        {
            case '0':
                coef0 = atof(optarg);
                break;
            case '1':
                coef1 = atof(optarg);
                break;
            case '2':
                coef2 = atof(optarg);
                break;
            case 'x':
                strcpy(fieldX, optarg);
                break;
            case '?':
                fprintf(stderr, "Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsApplyQuadRegression-F-101: usage xsApplyQuadRegression -0 double -1 double -2 double -x fieldX)");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsApplyQuadRegression-F-105: Can't get info header\n");
        exit(EXIT_FAILURE);
    }
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsApplyQuadRegression-F-107: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    int numXAxis = saCSVParseFieldList(xList, fieldX);

    // open stream to read CSV
    saCSVOpen(&csv, stdin);

    // Parse the first (header) line of input
    numFields = saCSV3GetLine(&csv, inbuf, fieldList);

    // Find the Regress column, if it already exists
    int i = 0;
    bool found = false;
    while(i<numFields && !found)
    {
        if (!saCSVCompareField(fieldList[i], SA_CONSTANTS_PREDICTION_COLUMN))
            found = true;
        else
            i++;
    }
    if (found)
        regressIndex = i;
    else
        fprintf(stdout, "%s,", SA_CONSTANTS_PREDICTION_COLUMN);
        
    for(i=0; i<numFields;i++)
    {
        if (i)
            fputs(",", stdout);
        fputs(fieldList[i], stdout);
    }
    fputs("\n", stdout);

    // find the column we wish to run regression
    int fieldIndex = 0;
    found = false;
    while(!found && fieldIndex < numFields)
    {
        if (!saCSVCompareField(fieldList[fieldIndex], xList[0]))
            found = true;
        else
            fieldIndex++;
    }

    // Initialize the results array and read all of the input 
    double regress;
    bool done = false;
    while(!done)
    {
        saCSV3GetLine(&csv, inbuf, fieldList);
        if (saCSVEOF(&csv) == false)
        {
            // initialize regression value
            regress = 0.00;
            
            // If the column that we are extracting exists, 
            //    extract the value and run regression
            if (found)
            {
                double value;
                char *field = fieldList[fieldIndex];
                if (*field == '"')
                {
                    strncpy(tempbuf, field+1, strlen(field)-2);
                    value = atof(tempbuf);
                }
                else           
                    value = atof(fieldList[fieldIndex]);  
                regress = coef0*value*value + coef1*value + coef2;             
            }

            if (regressIndex == -1)
                fprintf(stdout, "%.10f,", regress);
                
            // write out the values of each field passed in
            int i;
    	    for(i=0; i<numFields; i++)
            {
                if (i)
                    fputs(",", stdout);
                if (i == regressIndex)
                {
                    // Write out the regression results
                    fprintf(stdout, "%.10f", regress);
                }
                else
                    fputs(fieldList[i], stdout);
            }
            fputs("\n", stdout);
        }
        else
            done = true;
    }
    exit(0);
}


