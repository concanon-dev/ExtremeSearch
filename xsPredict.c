/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
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
#include "saConstants.h"
#include "saCSV.h"

#include "saSignal.h"

static char inbuf[SA_CONSTANTS_MAXNUMCOLS];
static char tempbuf[SA_CONSTANTS_MAXNUMCOLS];
static char *fieldList[SA_CONSTANTS_MAXNUMCOLS];

static char *byF[SA_CONSTANTS_MAXNUMCOLS];
static char *byV[SA_CONSTANTS_MAXNUMCOLS];
static double A[SA_CONSTANTS_MAXNUMCOLS];
static double B[SA_CONSTANTS_MAXNUMCOLS];
static double errA[SA_CONSTANTS_MAXNUMCOLS];
static double errB[SA_CONSTANTS_MAXNUMCOLS];
static double guess[SA_CONSTANTS_MAXNUMCOLS];
static int numRows[SA_CONSTANTS_MAXNUMCOLS];
static double R[SA_CONSTANTS_MAXNUMCOLS];
static char *X[SA_CONSTANTS_MAXNUMCOLS];
static char *Y[SA_CONSTANTS_MAXNUMCOLS];

static char *indexString[SA_CONSTANTS_MAXNUMCOLS];
static int numIndexes = 0;

int getIndex(int, int, int, int);
inline void printLine(char *[], int);
inline char *getField(char *);

int main(int argc, char* argv[]) 
{
    double p;

    initSignalHandler(basename(argv[0]));
    int c;
    bool argError = false;
    while ((c = getopt(argc, argv, "p:")) != -1) 
    {
        switch(c)
        {
            case 'p':
                p = atof(optarg);
                break;
            case '?':
                fprintf(stderr, "xsPredict-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError == true)
    {
        fprintf(stderr, "xsPredict-F-103: Usage: xsPredict -p value\n");
        exit(0);
    }

   int numFields;
   int i;
   for(i=0; i<SA_CONSTANTS_MAXNUMCOLS; i++)
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

   // Get the header
   numFields = saCSVGetLine(inbuf, fieldList);
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

   int maxIndex = 0;
   while(!feof(stdin))
   {
       int index = -1;
       numFields = saCSVGetLine(inbuf, fieldList);
       if (!feof(stdin))
       {
           index = getIndex(xIndex, yIndex, byFIndex, byVIndex);
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
               double thisR = atof(getField(fieldList[r2Index]));
               double thisA = atof(getField(fieldList[aIndex]));
               double thisB = atof(getField(fieldList[bIndex]));
               double thisErrA = atof(getField(fieldList[errAIndex]));
               double thisErrB = atof(getField(fieldList[errBIndex]));
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

   // Determine the weighted avg of R
   for(i=0; i<=maxIndex; i++)
   {
       R[i] = R[i] / (float)numRows[i];
       A[i] = A[i] / (float)numRows[i];
       B[i] = B[i] / (float)numRows[i];
       errA[i] = errA[i] / (float)numRows[i];
       errB[i] = errB[i] / (float)numRows[i];
       guess[i] = A[i]*p + B[i];
   }

   // write out the results
   fprintf(stdout, "x,y,bf,bv,numRows,slope,intercept,errA,errB,R,%s\n", 
           SA_CONSTANTS_PREDICTION_COLUMN);
   for(i=0; i<=maxIndex; i++)
   {
       fprintf(stdout, "%s,%s,%s,%s,%d,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f\n", X[i], Y[i], byF[i], byV[i], numRows[i], A[i], B[i], errA[i], errB[i], R[i], guess[i]);
   }
}

inline char *getField(char *field)
{
   if (*field == '"')
   {
       strcpy(tempbuf, field+1);
       tempbuf[strlen(field)-2] = '\0';
       return(tempbuf);
   }
   else
       return(field);
}

inline int getIndex(int xIndex, int yIndex, int byFIndex, int byVIndex)
{
   sprintf(tempbuf, "%s,%s,%s,%s", fieldList[xIndex], fieldList[yIndex], fieldList[byFIndex],
           fieldList[byVIndex]);
   bool found = false;
   int i=0;
   while(i<numIndexes && !found)
   {
       if (!strcmp(tempbuf, indexString[i]))
           found = true;
       else
           i++;
   }
   if (!found)
   {
       indexString[i] = malloc(strlen(tempbuf)+1);
       strcpy(indexString[i], tempbuf);
       numIndexes++;
   }
   return(i);
}

inline void printLine(char *fieldList[], int numFields)
{
   FILE *x = fopen("./x", "a");
   int i;
   for(i=0; i<numFields; i++)
   {
       if (!i)
           fputs(fieldList[i], x);
       else
           fprintf(x, ",%s", fieldList[i]);
   }
   fputs("\n", x);
}
