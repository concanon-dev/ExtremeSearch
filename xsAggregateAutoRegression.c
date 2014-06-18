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

static char *indexString[MAXROWSIZE];
static int numIndexes = 0;

extern FILE *saOpenFile(char *, char *);
extern char *saSplunkGetRoot(char *);
extern saSplunkInfoPtr saSplunkLoadHeader();
extern bool saSplunkReadInfoPathFile(saSplunkInfoPtr);

char *getField(char *);
int getIndex(int, int, int);

int main(int argc, char* argv[]) 
{
    char outfile[256];

    // confirm the license
    if (!isLicensed())
        exit(EXIT_FAILURE);

    // initialize system
    initSignalHandler(basename(argv[0]));
    outfile[0] = '\0';
    int c;
    bool argError = false;

    // get arguments
    while ((c = getopt(argc, argv, "f:")) != -1) 
    {
        switch (c)
        {
            case 'f':
                strcpy(outfile, optarg);
                break;
            case '?':
                fprintf(stderr, "xsAggregateAutoRegression-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError == true)
    {
        fprintf(stderr, "xsAggregateAutoRegression-F-103: Usage: xsAggregateAutoRegression\n");
        exit(0);
    }

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

   // initialize array
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

   // Get the csv header
   numFields = saCSVGetLine(inbuf, fieldList);
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

   // load the data
   int maxIndex = 0;
   while(!feof(stdin))
   {
       int index = -1;
       numFields = saCSVGetLine(inbuf, fieldList);
       if (!feof(stdin))
       {
           // See if there is already a reference to the class.  A class is the tuple 
           // formed by the values of the fields "x", "bf" and "bv".  This is used to
           // make sure that the correct rows are added together correctly in a weighted
           // fashion.  The weight is the count, the number of events that contribute to
           // the algorithm.
           index = getIndex(xIndex, byFIndex, byVIndex);
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

           // increment the coefficients
           int rowCount = atoi(fieldList[numRowsIndex]);
           if (rowCount > 0)
           {
               double thisCoef0 = atof(getField(fieldList[coef0Index]));
               double thisCoef1 = atof(getField(fieldList[coef1Index]));
               double thisCoef2 = atof(getField(fieldList[coef2Index]));
               numRows[index] = numRows[index] + rowCount;

               coef0[index] = coef0[index] + (thisCoef0 * rowCount);
               coef1[index] = coef1[index] + (thisCoef1 * rowCount);
               coef2[index] = coef2[index] + (thisCoef2 * rowCount);
               
               if (index > maxIndex)
                   maxIndex = index;
           }
       }
   }

   char tempDir[512];
   sprintf(tempDir, "%s/apps/%s/lookups/%s.csv", saSplunkGetRoot(argv[0]), p->app, outfile);
   FILE *f = saOpenFile(tempDir, "w");

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
 
       if (f != NULL) 
           fprintf(f, "%s,%s,%s,%d,%.10f,%.10f,%.10f\n", X[i], byF[i], byV[i],
                   numRows[i], coef0[i], coef1[i], coef2[i]);
       fprintf(stdout, "%s,%s,%s,%d,%.10f,%.10f,%.10f\n", X[i], byF[i], byV[i],
               numRows[i], coef0[i], coef1[i], coef2[i]);
   }
   if (f != NULL)
       fclose(f);
}

// return the contents of a field, without quotes if they exist
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

// find the row that corresponds to the x,bf,bv tuple
int getIndex(int xIndex, int byFIndex, int byVIndex)
{
   sprintf(tempbuf, "%s,%s,%s", fieldList[xIndex], fieldList[byFIndex], fieldList[byVIndex]);
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

