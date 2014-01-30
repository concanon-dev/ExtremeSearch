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
#include "saConstants.h"
#include "saCSV.h"
#include "saLicensing.h"
#include "saSignal.h"

static char inbuf[SA_CONSTANTS_MAXROWSIZE];
static char tempbuf[SA_CONSTANTS_MAXROWSIZE];
static char fileInBuf[SA_CONSTANTS_MAXAXIS+1][SA_CONSTANTS_MAXROWSIZE];
static char *fieldList[SA_CONSTANTS_MAXROWSIZE / 32];
static char *aList[SA_CONSTANTS_MAXAXIS];
static char *bList[SA_CONSTANTS_MAXAXIS];
static char *xList[SA_CONSTANTS_MAXAXIS];

extern char *optarg;
extern int optind, optopt;

extern FILE *saOpenFile(char *, char *);

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

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));  
    argError = false;
    while ((c = getopt(argc, argv, "0:1:2:x:")) != -1) 
    {
        switch (c)
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
        fprintf(stderr, "xsApplyAutoRegression-F-101: usage xsApplyAutoRegression -0 double -1 double -2 double -x fieldList)");
        exit(EXIT_FAILURE);
    }

    int numXAxis = saCSVParseFieldList(xList, fieldX);

    // Parse the first (header) line of input
    numFields = saCSVGetLine(inbuf, fieldList);

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
        saCSVGetLine(inbuf, fieldList);
        if (!feof(stdin))
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


