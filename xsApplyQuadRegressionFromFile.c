/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software
 contains proprietary trade and business secrets.

 Program: xsApplyQuadRegressionFromFile

 Usage: xsApplyQuadRegressionFromFile -f input_file

 Description:
        Apply quad regression against a set of events containing the specified fields. The algorithms are
        contained in a specified lookup file which contains one or more rows, each having an X field, a
        BY field (bf), a BY value (bv) and a series of 3 coefficients (coef0, coef1, coef2).

        The algorithm is coef0*fieldX*fieldX + coef1*fieldX + coef2

        The flag -f specifies the file to read.  This file is located in
        saSplunkGetRoot(argv[0])/apps/<app>/lookups/output_file.csv.
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
#include "saSplunk.h"

static char inbuf[SA_CONSTANTS_MAXROWSIZE];
static char tempbuf[SA_CONSTANTS_MAXROWSIZE];
static char fileInBuf[SA_CONSTANTS_MAXAXIS+1][SA_CONSTANTS_MAXROWSIZE];
static char *fieldList[SA_CONSTANTS_MAXROWSIZE / 32];
static double c0List[SA_CONSTANTS_MAXAXIS];
static double c1List[SA_CONSTANTS_MAXAXIS];
static double c2List[SA_CONSTANTS_MAXAXIS];
static char *xList[SA_CONSTANTS_MAXAXIS];
static char *byFieldList[SA_CONSTANTS_MAXAXIS];
static char *byValueList[SA_CONSTANTS_MAXAXIS];

extern char *optarg;
extern int optind, optopt;

extern char *saSplunkGetRoot(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

int main(int argc, char* argv[]) 
{
    bool argError;
    char fieldX[SA_CONSTANTS_MAXSTRING];
    char *fileDataList[32];
    char *fileFieldList[32];
    char fileName[SA_CONSTANTS_MAXSTRING];
    int c;
    int i;
    int numFields;
    int numFileHeader = 0;
    int numFileRows = 0;
    int regressIndex = -1;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));  
    argError = false;
    while ((c = getopt(argc, argv, "f:")) != -1) 
    {
        switch(c)
        {
            case 'f':
                strcpy(fileName, optarg);
                break;
            case '?':
                fprintf(stderr, "Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsApplyQuadRegressionFromFile-F-101: usage xsApplyQuadRegressionFromFile -f fileName");
        exit(EXIT_FAILURE);
    }

    saSplunkInfoPtr p = saSplunkLoadHeader();
    if (p == NULL)
    {
        fprintf(stderr, "xsApplyQuadRegressionFromFile-F-105: Can't get info header\n");
        exit(EXIT_FAILURE);
    } 
    if (saSplunkReadInfoPathFile(p) == false)
    {
        fprintf(stderr, "xsApplyQuadRegressionFromFile-F-107: Can't read search results file %s\n",
                p->infoPath == NULL ? "NULL" : p->infoPath);
        exit(EXIT_FAILURE);
    }

    char tempDir[512];
    sprintf(tempDir, "%s/apps/%s/lookups/%s.csv", saSplunkGetRoot(argv[0]), p->app, fileName);
    FILE *f = fopen(tempDir, "r");
    if (f == NULL)
    {
        fprintf(stderr, "xsApplyQuadRegressionFromFile-F-103: can't open file %s", fileName);
        exit(EXIT_FAILURE);
    }
    numFileHeader = saCSVFGetLine(f, fileInBuf[numFileRows++], fileFieldList);
    bool done = false;
    while (!done)
    {
        saCSVFGetLine(f, fileInBuf[numFileRows], fileDataList);
        if (!feof(f))
        {
            int j;
            for(j=0; j<numFileHeader; j++)
            {
                if (!strcmp(fileFieldList[j], "coef0"))
                    c0List[numFileRows] = atof(fileDataList[j]);
                else if (!strcmp(fileFieldList[j], "coef1"))
                    c1List[numFileRows] = atof(fileDataList[j]);
                else if (!strcmp(fileFieldList[j], "coef2"))
                    c2List[numFileRows] = atof(fileDataList[j]);
                else if (!strcmp(fileFieldList[j], "x"))
                    xList[numFileRows] = fileDataList[j];
                else if (!strcmp(fileFieldList[j], "bf"))
                    byFieldList[numFileRows] = fileDataList[j];
                else if (!strcmp(fileFieldList[j], "bv"))
                    byValueList[numFileRows] = fileDataList[j];
            }
            numFileRows++;
        }
        else
            done = true;
    }
    fclose(f);

    // Parse the first (header) line of input
    numFields = saCSVGetLine(inbuf, fieldList);

    // Find the Regress column, if it already exists
    i = 0;
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

    // find the column we wish to run regression against
    int fieldIndex = 0;
    int foundX = -1;
    int foundBy = -1;
    while(((foundX == -1) || (foundBy == -1)) && (fieldIndex < numFields))
    {
        if (!saCSVCompareField(fieldList[fieldIndex], xList[1]))
            foundX = fieldIndex;

        if (!saCSVCompareField(fieldList[fieldIndex], byFieldList[1]))
            foundBy = fieldIndex;

        fieldIndex++;
    }

    // Initialize the results array and read all of the input 
    done = false;
    while(!done)
    {
        saCSVGetLine(inbuf, fieldList);
        if (!feof(stdin))
        {
            // initialize the regression value
            double regress = 0.00;
            // If the X and By columns exist
            //    extract the X value
            //    find the right regression algorithm based on the ByValue
            //    run the regression
            if (foundX != -1)
            {
                double xValue;
                char *field = fieldList[foundX];
                if (*field == '"')
                {
                    strncpy(tempbuf, field+1, strlen(field)-2);
                    xValue = atof(tempbuf);
                }
                else           
                    xValue = atof(fieldList[foundX]);
                    
                i=1;
                bool found = false;
                while(!found && i<numFileRows)
                {
                    if (!saCSVCompareField(byFieldList[i], "*"))
                    {   
                        regress = c0List[i]*xValue*xValue + c1List[i]*xValue + c2List[i];
                        found = true;
                    }
                    else if (!saCSVCompareField(fieldList[foundBy], byValueList[i]))
                    {
                        regress = c0List[i]*xValue*xValue + c1List[i]*xValue + c2List[i];
                        found = true;
                    }
                    else
                        i++;
                }
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
