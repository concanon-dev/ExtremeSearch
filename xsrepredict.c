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
#include "saCSV.h"
#include "saLicensing.h"
#include "saSignal.h"

#define MAXROWSIZE 256

static char inbuf[MAXROWSIZE];
static char tempbuf[MAXROWSIZE];
static char *fieldList[MAXROWSIZE];

static double A[MAXROWSIZE];
static double B[MAXROWSIZE];
static double errA[MAXROWSIZE];
static double errB[MAXROWSIZE];
static double guess[MAXROWSIZE];
static int numRows[MAXROWSIZE];
static double R[MAXROWSIZE];
static char *X[MAXROWSIZE];
static char *Y[MAXROWSIZE];

void printLine(char *[], int);
char *getField(char *);

int main(int argc, char* argv[]) 
{
    double p;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    int c;
    bool argError = false;
    while ((c = getopt(argc, argv, "p:")) != -1) 
    {
        switch Copyright
        {
            case 'p':
                p = atof(optarg);
                break;
            case '?':
                fprintf(stderr, "xsrepredict-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError == true)
    {
        fprintf(stderr, "xsrepredict-F-103: Usage: xsrepredict -p value\n");
        exit(0);
    }

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
   int errAIndex = -1;
   int errBIndex = -1;
   int iIndex = -1;
   int numRowsIndex = -1;
   int r2Index = -1;
   int xIndex = -1;
   int yIndex = -1;

   // Get the header
   numFields = saCSVGetLine(inbuf, fieldList);
   for(i=0; i<numFields; i++)
   {
       if (!saCSVCompareField(fieldList[i], "slope"))
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

   if (numRowsIndex == -1)
   {
       fprintf(stderr, "xsrepredict-101: Missing column: numRows\n");
       exit(0);
   }

   if (r2Index == -1)
   {
       fprintf(stderr, "xsrepredict-101: Missing column: r2Index\n");
       exit(0);
   }

   if (xIndex == -1)
   {
       fprintf(stderr, "xsrepredict-101: Missing column: x\n");
       exit(0);
   }

   if (yIndex == -1)
   {
       fprintf(stderr, "xsrepredict-101: Missing column: y\n");
       exit(0);
   }

   if (aIndex == -1)
   {
       fprintf(stderr, "xsrepredict-101: Missing column: A\n");
       exit(0);
   }

   if (bIndex == -1)
   {
       fprintf(stderr, "xsrepredict-101: Missing column: B\n");
       exit(0);
   }

   if (errAIndex == -1)
   {
       fprintf(stderr, "xsrepredict-101: Missing column: errA\n");
       exit(0);
   }

   if (errBIndex == -1)
   {
       fprintf(stderr, "xsrepredict-101: Missing column: errB\n");
       exit(0);
   }

   int maxIndex = 0;
   while(!feof(stdin))
   {
       numFields = saCSVGetLine(inbuf, fieldList);
       if (!feof(stdin))
       {
           int index = 0;
           bool mFound = false;
           while(index<maxIndex && mFound == false)
           {
               if (!strcmp(fieldList[xIndex], X[index]) && !strcmp(fieldList[yIndex], Y[index]))
                   mFound = true;
               else
                   index++;
           }
           if (mFound == false)
           {
               X[index] = malloc(strlen(fieldList[xIndex]));
               strcpy(X[index], fieldList[xIndex]);
               Y[index] = malloc(strlen(fieldList[yIndex]));
               strcpy(Y[index], fieldList[yIndex]);
               maxIndex++;
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
   for(i=0; i<maxIndex; i++)
   {
       R[i] = R[i] / (float)numRows[i];
       A[i] = A[i] / (float)numRows[i];
       B[i] = B[i] / (float)numRows[i];
       errA[i] = errA[i] / (float)numRows[i];
       errB[i] = errB[i] / (float)numRows[i];
       guess[i] = A[i]*p + B[i];
   }

   // write out the results
   fputs("x,y,numRows,slope,intercept,errA,errB,R,Guess\n", stdout);
   for(i=0; i<maxIndex; i++)
   {
       fprintf(stdout, "%s,%s,%d,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f\n", X[i], Y[i], numRows[i],
               A[i], B[i], errA[i], errB[i], R[i], guess[i]);
   }
}

char *getField(char *field)
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

void printLine(char *fieldList[], int numFields)
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
