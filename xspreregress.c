/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <libgen.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "saLinearRegression.h"
#include "xconstants.h"

static char inbuf[MAXROWSIZE];
static char tempbuf[MAXROWSIZE];
static char *fieldList[MAXNUMCOLS];

static int totalRows[MAXAXIS];
static int bFieldIndex[MAXAXIS];
static int xFieldIndex[MAXAXIS];
static int yFieldIndex[MAXAXIS];
static char *bList[MAXAXIS];
static char *xList[MAXAXIS];
static char *yList[MAXAXIS];

static char *bAxis[MAXROWSIZE];
static char *bValues[MAXROWSIZE];
static double tempXDoubles[MAXROWSIZE];
static double tempYDoubles[MAXROWSIZE];
static int numTempDoubles;
static int numUniqueBValues;
static double *xAxis[MAXAXIS];
static double *yAxis[MAXAXIS];
static double xAxisHigh[MAXAXIS];
static double xAxisLow[MAXAXIS];
static double yAxisHigh[MAXAXIS];
static double yAxisLow[MAXAXIS];

extern FILE *saDebugOpenFile(int);

extern int getCSVLine(char [], char *[]);
extern char *insertUniqueValue(char *[], char *, int *);
extern int parseFieldList(char *[], char *);

extern saLinearRegressionTypePtr createLR(double [], double [], int numPoints);
extern void getRegressionLine(saLinearRegressionTypePtr);

extern char *optarg;
extern int optind, optopt;


int main(int argc, char* argv[]) 
{
    bool argError;
    bool hasByClause;
    bool mustMatchFields;
    bool showRange;
    char fieldB[MAXSTRING];
    char fieldX[MAXSTRING];
    char fieldY[MAXSTRING];
    int c;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    saDebugBegin();

    argError = false;
    hasByClause = false;
    mustMatchFields = true;
    showRange = false;
    while ((c = getopt(argc, argv, "b:irx:y:")) != -1) 
    {
        switch (c)
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
                fprintf(stderr, "xspreregress-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, 
                "xspreregress-F-103: Uage: xspreregress [-d] [-i] [-r] -x fieldList -y fieldList");
        exit(EXIT_FAILURE);
    }

    int numBAxis = -1;
    if (hasByClause)
        numBAxis = parseFieldList(bList, fieldB);
    int numXAxis = parseFieldList(xList, fieldX);
    int numYAxis = parseFieldList(yList, fieldY);
    if (numXAxis != numYAxis)
    {
        fprintf(stderr, 
                "xspreregress-F-105: found %d X and %d Y parameters.  Values must be the same\n",
                numXAxis, numYAxis);
        exit(EXIT_FAILURE);
    }

    if ((numBAxis != -1) && (numXAxis != numBAxis))
    {
        fprintf(stderr,
                "xspreregress-F-111: found %d X and %d B parameters.  Values must be the same\n",
                numXAxis, numBAxis);
        exit(EXIT_FAILURE);
    }

    // Initialize index pointers
    int foundCount = 0;
    int i;
    for(i=0; i<MAXAXIS; i++)
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
    int numFields = getCSVLine(inbuf, fieldList);
    if (debug_levels[DEBUG_DATA] > 1)
    {   
        FILE *d = saDebugOpenFile(DEBUG_DATA);
        for(i=0; i<numFields; i++)
        {
            if (i>0)
                fputs(",", d);
            fputs(fieldList[i], d);
        }
        fputs("\n", d);
        fclose(d);
    }

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
                  if (!compareField(fieldList[j], bList[i]))
                  {
                      bFieldIndex[i] = j;
                      foundB = true;
                  }

              if (!compareField(fieldList[j], xList[i]))
              {
                  xFieldIndex[i] = j;
                  foundX = true;
              }
              if (!compareField(fieldList[j], yList[i]))
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
               fprintf(stderr, "xspreregress-F-105: BY clause not found: %s\n", bList[i]);
           missedOne = true;
        }
        if (xFieldIndex[i] == -1)
        {
            if (mustMatchFields == true)
                fprintf(stderr, "xspreregress-F-107: x not found: %s\n", xList[i]);
            missedOne = true;
        }
        if (yFieldIndex[i] == -1)
        {
            if (mustMatchFields == true)
                fprintf(stderr, "xspreregress-F-109: y not found: %s\n", yList[i]);
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
            xAxis[i] = malloc(sizeof(double)*MAXNUMROWS);
            yAxis[i] = malloc(sizeof(double)*MAXNUMROWS);
        }

        numUniqueBValues = 0;
        bool done = false;
        while(done == false)
        {
            int numCols = getCSVLine(inbuf, fieldList);
            if (!feof(stdin))
            {
                if (debug_levels[DEBUG_DATA] > 1)
                {
                    FILE *d = saDebugOpenFile(DEBUG_DATA);
                    for(i=0; i<numCols; i++)
                    {
                        if (i>0)
                            fputs(",", d);
                        fputs(fieldList[i], d);
                    }
                    fputs("\n", d);
                    fclose(d);
                }
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

    if (debug_levels[DEBUG_DATA] > 0)
    {
        FILE *d = saDebugOpenFile(DEBUG_DATA);
        for(i=0; i<totalRows[0]; i++)
            fprintf(d, "%.10f,%.10f\n", xAxis[0][i], yAxis[0][i]);
        fclose(d);
    }

    FILE *d = saDebugOpenFile(DEBUG_OUTPUT);
    if (debug_levels[DEBUG_OUTPUT] > 0)
        fputs("numRows,x,y,bf,bv,slope,intercept,errA,errB,R", d);
    fputs("numRows,x,y,bf,bv,slope,intercept,errA,errB,R", stdout);

    if (showRange == true)
    {
        if (debug_levels[DEBUG_OUTPUT] > 0)
            fputs(",xLow,xHigh,yLow,yHigh", d);
        fputs(",xLow,xHigh,yLow,yHigh", stdout);
    }
    if (debug_levels[DEBUG_OUTPUT] > 0)
        fputs("\n", d);
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
                if (debug_levels[DEBUG_LOG] > 0)
                    fprintf(d, "%d,%s,%s,%s,%s", numTempDoubles, xList[i], yList[i], 
                            bList[i], bValues[j]);
                fprintf(stdout, "%d,%s,%s,%s,%s", numTempDoubles, xList[i], yList[i], 
                        bList[i], bValues[j]);

                saLinearRegressionTypePtr lrPtr = createLR(tempXDoubles, tempYDoubles, 
                                                           numTempDoubles);
                getRegressionLine(lrPtr);
                if (totalRows[i] > 0)
                {
                    if (debug_levels[DEBUG_LOG] > 0)
                        fprintf(d, ",%.10f,%.10f,%.10f,%.10f,%.10f", lrPtr->coefA, lrPtr->coefB,
                                lrPtr->stdError_CoefA, lrPtr->stdError_CoefB, lrPtr->R);
                    fprintf(stdout, ",%.10f,%.10f,%.10f,%.10f,%.10f", lrPtr->coefA, lrPtr->coefB,
                            lrPtr->stdError_CoefA, lrPtr->stdError_CoefB, lrPtr->R);

                    if (showRange == true)
                    {
                        if (debug_levels[DEBUG_OUTPUT] > 0)
                            fprintf(d, ",%.10f,%.10f,%.10f,%.10f", xAxisLow[i], xAxisHigh[i],
                                    yAxisLow[i], yAxisHigh[i]);
                        fprintf(stdout, ",%.10f,%.10f,%.10f,%.10f", xAxisLow[i], xAxisHigh[i],
                                yAxisLow[i], yAxisHigh[i]);
                    }
                }
                else
                {
                    if (debug_levels[DEBUG_OUTPUT] > 0)
                        fprintf(d, ",0.0,0.0,0.0,0.0,0.0");
                    fprintf(stdout, ",0.0,0.0,0.0,0.0,0.0");
                    if (showRange == true)
                    {
                        if (debug_levels[DEBUG_OUTPUT] > 0)
                            fprintf(d, ",0.0,0.0,0.0,0.0");
                        fprintf(stdout, ",0.0,0.0,0.0,0.0");
                    }
                }
                if (debug_levels[DEBUG_OUTPUT] > 0)
                    fputs("\n", d);
                fputs("\n", stdout);
            }
        }
    }
    else
    {
        for(i=0; i<numXAxis; i++)
        {
            saLinearRegressionTypePtr lrPtr = createLR(xAxis[i], yAxis[i], totalRows[i]);
            getRegressionLine(lrPtr);

            if (debug_levels[DEBUG_OUTPUT] > 0)
                fprintf(d, "%d,%s,%s,*,*", totalRows[i], xList[i], yList[i]);
            fprintf(stdout, "%d,%s,%s,*,*", totalRows[i], xList[i], yList[i]);

            if (totalRows[i] > 0)
            {
                if (debug_levels[DEBUG_OUTPUT] > 0)
                    fprintf(d, ",%.10f,%.10f,%.10f,%.10f,%.10f", lrPtr->coefA, lrPtr->coefB, 
                            lrPtr->stdError_CoefA, lrPtr->stdError_CoefB, lrPtr->R);
                fprintf(stdout, ",%.10f,%.10f,%.10f,%.10f,%.10f", lrPtr->coefA, lrPtr->coefB, 
                        lrPtr->stdError_CoefA, lrPtr->stdError_CoefB, lrPtr->R);

                if (showRange == true)
                {
                    if (debug_levels[DEBUG_OUTPUT] > 0)
                        fprintf(d, ",%.10f,%.10f,%.10f,%.10f", xAxisLow[i], xAxisHigh[i],
                                yAxisLow[i], yAxisHigh[i]);
                    fprintf(stdout, ",%.10f,%.10f,%.10f,%.10f", xAxisLow[i], xAxisHigh[i],
                            yAxisLow[i], yAxisHigh[i]);
                }
            }
            else
            {
                if (debug_levels[DEBUG_OUTPUT] > 0)
                    fprintf(d, ",0.0,0.0,0.0,0.0,0.0");
                fprintf(stdout, ",0.0,0.0,0.0,0.0,0.0");

                if (showRange == true)
                {
                    if (debug_levels[DEBUG_OUTPUT] > 0)
                        fprintf(d, ",0.0,0.0,0.0,0.0");
                    fprintf(stdout, ",0.0,0.0,0.0,0.0");
                }
            }
            if (debug_levels[DEBUG_OUTPUT] > 0)
                fputs("\n", d);
            fputs("\n", stdout);
        }
    }
    exit(0);
}

