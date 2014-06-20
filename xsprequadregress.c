/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

 Program: xsprequadregress

 Usage: xsprequadregress [-b fieldList] [-i] [-m method] [-r] -x fieldList -y fieldList
    -b the list of BY fields, separated by commas (defaults to none)
    -i don't exit if any BY column doesn't exist (default is to exit if BY column does not exist)    
    -m the method of auto regression to perform (maxentropy or leastsquares, defaults to maxentropy)
    -r write out the lowest and highest values for each X and Y field in each BY group
    -x the list of X fields to perform quad regression against
    -y the list of Y fields to perform quad regression against

 Description:
        For each X/Y field pair and BY field grouping, generate a quadregression algorithm.

        The algorithm is coef0*fieldX*fieldX + coef1*fieldX + coef2
*/
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
#include "saQuadRegression.h"
#include "saSignal.h"

static char inbuf[SA_CONSTANTS_MAXROWSIZE];
static char tempbuf[SA_CONSTANTS_MAXROWSIZE];
static char *fieldList[SA_CONSTANTS_MAXNUMCOLS];

static int totalRows[SA_CONSTANTS_MAXAXIS];
static int bFieldIndex[SA_CONSTANTS_MAXAXIS];
static int xFieldIndex[SA_CONSTANTS_MAXAXIS];
static int yFieldIndex[SA_CONSTANTS_MAXAXIS];
static char *bList[SA_CONSTANTS_MAXAXIS];
static char *xList[SA_CONSTANTS_MAXAXIS];
static char *yList[SA_CONSTANTS_MAXAXIS];

static char *bAxis[SA_CONSTANTS_MAXROWSIZE];
static char *bValues[SA_CONSTANTS_MAXROWSIZE];
static double tempXDoubles[SA_CONSTANTS_MAXROWSIZE];
static double tempYDoubles[SA_CONSTANTS_MAXROWSIZE];
static int numTempDoubles;
static int numUniqueBValues;
static double *xAxis[SA_CONSTANTS_MAXAXIS];
static double *yAxis[SA_CONSTANTS_MAXAXIS];
static double xAxisHigh[SA_CONSTANTS_MAXAXIS];
static double xAxisLow[SA_CONSTANTS_MAXAXIS];
static double yAxisHigh[SA_CONSTANTS_MAXAXIS];
static double yAxisLow[SA_CONSTANTS_MAXAXIS];


extern int saCSVGetLine(char [], char *[]);
extern char *insertUniqueValue(char *[], char *, int *);
extern int saCSVParseFieldList(char *[], char *);

extern dataRecordTypePtr initDataRecord(int, double *, double *, double *);
extern int quadRegression(dataRecordTypePtr, int, int, int);

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
    int c;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    argError = false;
    hasByClause = false;
    mustMatchFields = true;
    showRange = false;
    while ((c = getopt(argc, argv, "b:irx:y:")) != -1) 
    {
        switch Copyright
        {
            case 'b':
                strcpy(fieldB, optarg);
                hasByClause = true;
                break;
            case 'i':
                mustMatchFields = false;
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
                fprintf(stderr, "xsprequadregress-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, 
                "xsprequadregress-F-103: Uage: xsprequadregress [-d] [-i] [-r] -x fieldList -y fieldList");
        exit(EXIT_FAILURE);
    }

    int numBAxis = -1;
    if (hasByClause)
        numBAxis = saCSVParseFieldList(bList, fieldB);
    int numXAxis = saCSVParseFieldList(xList, fieldX);
    int numYAxis = saCSVParseFieldList(yList, fieldY);
    if (numXAxis != numYAxis)
    {
        fprintf(stderr, 
                "xsprequadregress-F-105: found %d X and %d Y parameters.  Values must be the same\n",
                numXAxis, numYAxis);
        exit(EXIT_FAILURE);
    }

    if ((numBAxis != -1) && (numXAxis != numBAxis))
    {
        fprintf(stderr,
                "xsprequadregress-F-107: found %d X and %d B parameters.  Values must be the same\n",
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
        yFieldIndex[i] = -1;
        xAxisLow[i] = 1.79769e+308;
        xAxisHigh[i] = 2.22507e-308;
        yAxisLow[i] = 1.79769e+308;
        yAxisHigh[i] = 2.22507e-308;
    }

    // Get Header line
    int numFields = saCSVGetLine(inbuf, fieldList);

    // Find the x and y fields in the header list
    for(i=0; i<numXAxis; i++)
    {
        int j = 0;

        // if no BY clause, then set foundB == true so the search will run correctly
        bool foundB = (hasByClause == false);
        bool foundX = false;
        bool foundY = false;
        while(j<numFields && (foundX == false || foundY == false || foundB == false))
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
              if (!saCSVCompareField(fieldList[j], yList[i]))
              {
                  yFieldIndex[i] = j;
                  foundY = true;
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
               fprintf(stderr, "xprequadregress-F-109: BY clause not found: %s\n", bList[i]);
           missedOne = true;
        }
        if (xFieldIndex[i] == -1)
        {
            if (mustMatchFields == true)
                fprintf(stderr, "xsprequadregress-F-111: x not found: %s\n", xList[i]);
            missedOne = true;
        }
        if (yFieldIndex[i] == -1)
        {
            if (mustMatchFields == true)
                fprintf(stderr, "xsprequadregress-F-113: y not found: %s\n", yList[i]);
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
            yAxis[i] = malloc(sizeof(double)*SA_CONSTANTS_MAXNUMROWS);
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

                    // Extract the value of fieldX and fieldY and insert them into the array
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

                    // Now for Y
                    field = fieldList[yFieldIndex[i]];
                    double *yPtr = yAxis[i];
                    if (badField == true || field == NULL || *field == '\0')
                        badField = true;
                    else if (*field == '"')
                    {
                        strncpy(tempbuf, field+1, strlen(field)-2);
                        yPtr[numRows] = atof(tempbuf);
                    }
                    else    
                        yPtr[numRows] = atof(field);

                    // see if this value is lowest or highest (so far) in range
                    if (showRange == true && badField == false)
                    {
                        if (yPtr[numRows] < yAxisLow[i])
                            yAxisLow[i] = yPtr[numRows];
                        if (yPtr[numRows] > yAxisHigh[i])
                            yAxisHigh[i] = yPtr[numRows];
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
    fputs("numRows,x,y,bf,bv,coef0,coef1,coef2", stdout);
    if (showRange == true)
        fputs(",xLow,xHigh,yLow,yHigh", stdout);
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
                        tempYDoubles[numTempDoubles] = yAxis[i][k];
                        numTempDoubles++;
                    }
                    k++;
                }
                fprintf(stdout, "%d,%s,%s,%s,%s", numTempDoubles, xList[i], yList[i], 
                        bList[i], bValues[j]);
                dataRecordTypePtr p = initDataRecord(numTempDoubles, tempXDoubles, tempYDoubles, 
                                                     NULL);
                int status = quadRegression(p, numTempDoubles, 0, numTempDoubles); 
                                                                         // length, offset, periods
                if (totalRows[i] > 0)
                {
                    fprintf(stdout, ",%.10f,%.10f,%.10f", p->coef[0], p->coef[1], p->coef[2]);
                    if (showRange == true)
                        fprintf(stdout, ",%.10f,%.10f,%.10f,%.10f", xAxisLow[i], xAxisHigh[i],
                                yAxisLow[i], yAxisHigh[i]);
                }
                else
                {
                    fprintf(stdout, ",0.0,0.0,0.0");
                    if (showRange == true)
                        fprintf(stdout, ",0.0,0.0,0.0,0.0");
                }
                fputs("\n", stdout);
            }
        }
    }
    else
    {
        for(i=0; i<numXAxis; i++)
        {
            dataRecordTypePtr p = initDataRecord(totalRows[i], xAxis[i], yAxis[i], NULL);
            int status = quadRegression(p, totalRows[i], 0, totalRows[i]);
                                                                         // length, offset, periods
            fprintf(stdout, "%d,%s,%s,*,*", totalRows[i], xList[i], yList[i]);
            if (totalRows[i] > 0)
            {
                fprintf(stdout, ",%.10f,%.10f,%.10f", p->coef[0], p->coef[1], p->coef[2]);
                if (showRange == true)
                    fprintf(stdout, ",%.10f,%.10f,%.10f,%.10f", xAxisLow[i], xAxisHigh[i],
                            yAxisLow[i], yAxisHigh[i]);
            }
            else
            {
                fprintf(stdout, ",0.0,0.0,0.0");
                if (showRange == true)
                    fprintf(stdout, ",0.0,0.0,0.0,0.0");
            }
            fputs("\n", stdout);
        }
    }
    exit(0);
}

