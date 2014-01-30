/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
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

static char *indexString[MAXROWSIZE];
static int numIndexes = 0;

extern FILE *saOpenFile(char *, char *);

char *getField(char *);
int getIndex(int, int, int, int);
void printLine(char *[], int);

int main(int argc, char* argv[]) 
{
    char outfile[256];
    double p;

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));
    outfile[0] = '\0';
    int c;
    bool argError = false;
    while ((c = getopt(argc, argv, "f:")) != -1) 
    {
        switch (c)
        {
            case 'f':
                strcpy(outfile, optarg);
                break;
            case '?':
                fprintf(stderr, "xsPerformSpearmanCorrelation-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError == true)
    {
        fprintf(stderr, "xsPerformSpearmanCorrelation-F-103: Usage: xsPerformSpearmanCorrelation [-f file]\n");
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
       else if (!saCSVCompareField(fieldList[i], "numRows"))
           numRowsIndex = i;
       else if (!saCSVCompareField(fieldList[i], "R"))
           r2Index = i;
       else if (!saCSVCompareField(fieldList[i], "x"))
            xIndex = i;
       else if (!saCSVCompareField(fieldList[i], "y"))
            yIndex = i;
   }
// ADD CODE TO CHECK FOR MISSING HEADER

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
               numRows[index] = numRows[index] + rowCount;

               R[index] = R[index] + (thisR * rowCount);
               
               if (index > maxIndex)
                   maxIndex = index;
           }
       }
   }

   fputs("x,y,bf,bv,numRows,R\n", stdout);

   
   // Determine the weighted avg of R
   for(i=0; i<=maxIndex; i++)
   {
       R[i] = R[i] / (float)numRows[i];
 
       fprintf(stdout, "%s,%s,%s,%s,%d,%.10f\n", X[i], Y[i], byF[i], byV[i], numRows[i], R[i]);
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

int getIndex(int xIndex, int yIndex, int byFIndex, int byVIndex)
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
