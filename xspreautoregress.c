/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xspreautoregress

 Usage: xspreautoregress [-b fieldList] [-c numCoefs] [-i] [-m method] [-r] -x fieldList
    -b the list of BY fields, separated by commas (defaults to none)
    -c the number of coefficients to generate (defaults to SA_AUTOREGRESSION_DEFAULTNUMCOEFS)
    -i don't exit if any BY column doesn't exist (default is to exit if BY column does not exist)    
    -m the method of auto regression to perform (maxentropy or leastsquares, defaults to maxentropy)
    -r write out the lowest and highest value for each X field in a BY group
    -x the list of X fields to perform auto regression against

 Description:
        For each X field and BY field grouping, generate an autoregression algorithm.

        The algorithm is coef0*fieldX*fieldX + coef1*fieldX + coef2
*/
#include <libgen.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "saAutoRegression.h"
#include "saConstants.h"
#include "saCSV.h"
#include "saLicensing.h"
#include "saSignal.h"

static char inbuf[SA_CONSTANTS_MAXROWSIZE];
static char tempbuf[SA_CONSTANTS_MAXROWSIZE];
static char *fieldList[SA_CONSTANTS_MAXNUMCOLS];

static int totalRows[SA_CONSTANTS_MAXAXIS];
static int bFieldIndex[SA_CONSTANTS_MAXAXIS];
static int xFieldIndex[SA_CONSTANTS_MAXAXIS];
static char *bList[SA_CONSTANTS_MAXAXIS];
static char *xList[SA_CONSTANTS_MAXAXIS];
static char *yList[SA_CONSTANTS_MAXAXIS];

static char *bAxis[SA_CONSTANTS_MAXROWSIZE];
static char *bValues[SA_CONSTANTS_MAXROWSIZE];
static double tempXDoubles[SA_CONSTANTS_MAXROWSIZE];
static int numTempDoubles;
static int numUniqueBValues;
static double *xAxis[SA_CONSTANTS_MAXAXIS];
static double xAxisHigh[SA_CONSTANTS_MAXAXIS];
static double xAxisLow[SA_CONSTANTS_MAXAXIS];


extern int saCSVGetLine(char [], char *[]);
extern char *insertUniqueValue(char *[], char *, int *);
extern int saCSVParseFieldList(char *[], char *);

extern double *autoRegression(double *, int, int, int);

extern char *optarg;
extern int optind, optopt;

int main(int argc, char* argv[]) 
{
    bool argError;
    bool hasByClause;
    bool mustMatchFields;
    bool showRange;
    char fieldB[SA_CONSTANTS_MAXSTRING];
    char fieldX[SA_CONSTANTS_MAXSTRING];
    char fieldY[SA_CONSTANTS_MAXSTRING];
    char method[SA_CONSTANTS_MAXSTRING];
    int c;
    int mode;
    int numCoefs = SA_AUTOREGRESSION_DEFAULTNUMCOEFS;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    fieldY[0] = '\0';
    argError = false;
    hasByClause = false;
    mustMatchFields = true;
    showRange = false;
    strcpy(method, "maxentropy");
    while ((c = getopt(argc, argv, "b:c:im:x:y:")) != -1) 
    {
        switch(c)
        {
            case 'b':
                strcpy(fieldB, optarg);
                hasByClause = true;
                break;
            case 'c':
                numCoefs = atoi(optarg);
                break;
            case 'i':
                mustMatchFields = false;
                break;
            case 'm':
                strcpy(method, optarg);
                break;
            case 'r':
                showRange = true;
                break;
            case 'x':
                strcpy(fieldX, optarg);
                break;
            case 'y':
                strcpy(fieldY, optarg);
                break;
            case '?':
                fprintf(stderr, "xspreautoregress-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, 
                "xspreautoregress-F-103: Uage: xspreautoregress [-b field] [-c numCoefs] [-i] [-m method] -x fieldList [-y fieldList]");
        exit(EXIT_FAILURE);
    }

    if (!strcmp(method, "leastsquares"))
        mode = SA_AUTOREGRESSION_LEASTSQUARES;
    else
        mode = SA_AUTOREGRESSION_MAXENTROPY;

    int numBAxis = -1;
    if (hasByClause)
        numBAxis = saCSVParseFieldList(bList, fieldB);
    int numXAxis = saCSVParseFieldList(xList, fieldX);
    saCSVParseFieldList(yList, fieldY);
    if ((numBAxis != -1) && (numXAxis != numBAxis))
    {
        fprintf(stderr,
                "xspreautoregress-F-111: found %d X and %d B parameters but the number of values must be the same\n",
                numXAxis, numBAxis);
        exit(EXIT_FAILURE);
    }

    // Initialize index pointers
    int foundCount = 0;
    int i;
    for(i=0; i<SA_CONSTANTS_MAXAXIS; i++)
    {
        totalRows[i] = 0;
        bFieldIndex[i] = -1;
        xFieldIndex[i] = -1;
        xAxisLow[i] = 1.79769e+308;
        xAxisHigh[i] = 2.22507e-308;
    }

    // Get Header line
    int numFields = saCSVGetLine(inbuf, fieldList);

    // Find the x and by fields in the header list
    for(i=0; i<numXAxis; i++)
    {
        int j = 0;

        // if no BY clause, then set foundB == true so the search will run correctly
        bool foundB = (hasByClause == false);
        bool foundX = false;
        while(j<numFields && (foundX == false || foundB == false))
        {
              if (hasByClause)
                  if (!saCSVCompareField(fieldList[j], bList[i]))
                  {
                      bFieldIndex[i] = j;
                      foundB = true;
                  }

              if (!saCSVCompareField(fieldList[j], xList[i]))
              {
                  xFieldIndex[i] = j;
                  foundX = true;
              }
              j++;
        }
    }

    // if fields must exist in the stream, then return an error if missing
    bool missedOne = false;
    for(i=0; i<numXAxis; i++)
    {
        if (hasByClause && bFieldIndex[i] == -1)
        {
           if (mustMatchFields == true)
               fprintf(stderr, "xpreautoregress-F-105: BY clause not found: %s\n", bList[i]);
           missedOne = true;
        }
        if (xFieldIndex[i] == -1)
        {
            if (mustMatchFields == true)
                fprintf(stderr, "xspreautoregress-F-107: x not found: %s\n", xList[i]);
            missedOne = true;
        }
    }
    if (missedOne == true && mustMatchFields == true)
        exit(EXIT_FAILURE);

    // only walk the rows if all fields exist
    if (missedOne == false)
    {
        // allocate data arrays
        for(i=0; i<numXAxis; i++)
        {
            xAxis[i] = malloc(sizeof(double)*SA_CONSTANTS_MAXNUMROWS);
        }

        numUniqueBValues = 0;
        bool done = false;
        while(done == false)
        {
            saCSVGetLine(inbuf, fieldList);
            if (!feof(stdin))
            {
                for(i=0; i<numXAxis; i++)
                {
                    bool badField = false;

                    // get the latest row to use for this index
                    int numRows = totalRows[i];

                    // Extract the value of fieldX and insert them into the array
                    // first for X
                    char *field = fieldList[xFieldIndex[i]];
                    double *xPtr = xAxis[i];
                    if (field == NULL || *field == '\0')
                        badField = true;
                    else if (*field == '"')
                    {
                        strncpy(tempbuf, field+1, strlen(field)-2);
                        xPtr[numRows] = atof(tempbuf);
                    }
                    else           
                        xPtr[numRows] = atof(field);

                    // see if this value is lowest or highest (so far) in range
                    if (showRange == true && badField == false)
                    {
                        if (xPtr[numRows] < xAxisLow[i])
                            xAxisLow[i] = xPtr[numRows];
                        if (xPtr[numRows] > xAxisHigh[i])
                            xAxisHigh[i] = xPtr[numRows];
                    }

                    if (badField == false && hasByClause == true)
                    {
                        char *bField = fieldList[bFieldIndex[0]];
                        if (*bField == '"')
                        {
                            strncpy(tempbuf, bField+1, strlen(bField)-2);
                            tempbuf[strlen(bField)-2] = '\0';
                            bField = tempbuf;
                        }
                        bAxis[numRows] = insertUniqueValue(bValues, bField, &numUniqueBValues);
                    }

                    if (badField == false)
                        totalRows[i] = totalRows[i] + 1;
                }

            }
            else
                done = true;
        }
    }
    if (strlen(fieldY) != 0)
        fputs("numRows,x,y,bf,bv,coef0,coef1,coef2", stdout);
    else
        fputs("numRows,x,bf,bv,coef0,coef1,coef2", stdout);
    if (showRange == true)
        fputs(",xLow,xHigh", stdout);
    fputs("\n", stdout);

    double r2;

    if (hasByClause == true)
    {
        for(i=0; i<numXAxis; i++)
        {
            int j, k;
            for(j=0; j<numUniqueBValues; j++)
            {
                numTempDoubles = 0;
                k=0; 
                while(k<totalRows[i])
                {
                    if (!strcmp(bAxis[k], bValues[j]))
                    {
                        tempXDoubles[numTempDoubles] = xAxis[i][k];
                        numTempDoubles++;
                    }
                    k++;
                }
                if (strlen(fieldY) != 0)
                    fprintf(stdout, "%d,%s,%s,%s,%s", numTempDoubles, xList[i], yList[i], bList[i],
                            bValues[j]);
                else
                    fprintf(stdout, "%d,%s,%s,%s", numTempDoubles, xList[i], bList[i], bValues[j]);
                double *coef = autoRegression(tempXDoubles, numTempDoubles, numCoefs, mode);
                if (totalRows[i] > 0 && coef != NULL)
                {
                    fprintf(stdout, ",%.10f,%.10f,%.10f", coef[0], coef[1], coef[2]);
                    if (showRange == true)
                        fprintf(stdout, ",%.10f,%.10f", xAxisLow[i], xAxisHigh[i]);
                }
                else
                {
                    fprintf(stdout, ",0.0,0.0,0.0");
                    if (showRange == true)
                        fprintf(stdout, ",0.0,0.0");
                }
                fputs("\n", stdout);
            }
        }
    }
    else
    {
        for(i=0; i<numXAxis; i++)
        {
            double *coef = autoRegression(xAxis[i], totalRows[i], numCoefs, mode);
            if (strlen(fieldY) != 0)
                fprintf(stdout, "%d,%s,%s,*,*", totalRows[i], xList[i], yList[i]);
            else
                fprintf(stdout, "%d,%s,*,*", totalRows[i], xList[i]);
            if (totalRows[i] > 0)
            {
                fprintf(stdout, ",%.10f,%.10f,%.10f", coef[0], coef[1], coef[2]);
                if (showRange == true)
                    fprintf(stdout, ",%.10f,%.10f", xAxisLow[i], xAxisHigh[i]);
            }
            else
            {
                fprintf(stdout, ",0.0,0.0,0.0");
                if (showRange == true)
                    fprintf(stdout, ",0.0,0.0");
            }
            fputs("\n", stdout);
        }
    }
    exit(0);
}

