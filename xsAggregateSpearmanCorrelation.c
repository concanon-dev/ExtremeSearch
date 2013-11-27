/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <libgen.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXROWSIZE 256

static char inbuf[MAXROWSIZE];
static char tempbuf[MAXROWSIZE];
static char *fieldList[MAXROWSIZE];

static int numRows[MAXROWSIZE];
static double R[MAXROWSIZE];
static char *by[MAXROWSIZE];
static char *X[MAXROWSIZE];
static char *Y[MAXROWSIZE];

void printLine(char *[], int);

int main(int argc, char* argv[]) 
{
   int numFields;

   if (!isLicensed())
       exit(EXIT_FAILURE);

   initSignalHandler(basename(argv[0]));

   int i;
   for(i=0; i<MAXROWSIZE; i++)
   {
       numRows[i] = 0;
       R[i] = 0.0;
   }

   int byIndex = -1;
   int iIndex = -1;
   int numRowsIndex = -1;
   int rIndex = -1;
   int xIndex = -1;
   int yIndex = -1;

   /*
       The parallel processing routine xprecorrelate returns it's results as
       rows with 5 fields: i,numRows,x,y,R.  So, we need to determine which column
       contains which field.
   */
   // Get the header
   numFields = saCSVGetLine(inbuf, fieldList);
   for(i=0; i<numFields; i++)
   {
       if (!compareField(fieldList[i], "by"))
           byIndex = i;
       else if (!compareField(fieldList[i], "i"))
           iIndex = i;
       else if (!compareField(fieldList[i], "numRows"))
           numRowsIndex = i;
       else if (!compareField(fieldList[i], "R"))
           rIndex = i;
       else if (!compareField(fieldList[i], "x"))
            xIndex = i;
       else if (!compareField(fieldList[i], "y"))
            yIndex = i;
   }

   /*
       Walk the rows of data and populate the arrays with the correct values from
       each column.
   */
   int maxIndex = 0;
   while(!feof(stdin))
   {
       int index = -1;
       numFields = saCSVGetLine(inbuf, fieldList);
       if (!feof(stdin))
       {
           // get the index (number) associated with the x,y pair to correlate
           index = atoi(fieldList[iIndex]);

           // If the BY for the correlation index is not defined, set it
           if (by[index] == NULL || (*by[index] == '\0'))
           {
               by[index] = malloc(strlen(fieldList[byIndex]));
               strcpy(by[index], fieldList[byIndex]);
           }

           // If the X value for the correlation index is not defined, set it
           if (X[index] == NULL || (*X[index] == '\0'))
           {
               X[index] = malloc(strlen(fieldList[xIndex]));
               strcpy(X[index], fieldList[xIndex]);
           }

           // If the Y value for the correlation index is not defined, set it
           if (Y[index] == NULL || (*Y[index] == '\0'))
           {
               Y[index] = malloc(strlen(fieldList[yIndex]));
               strcpy(Y[index], fieldList[yIndex]);
           }

           // get the number of rows for this sample
           int rowCount = atoi(fieldList[numRowsIndex]);

           // get the pearsons R value
           double thisR;
           if (*fieldList[rIndex] == '"')
           {
               strcpy(tempbuf, (fieldList[rIndex])+1);
               tempbuf[strlen(fieldList[rIndex])-2] = '\0';
               thisR = atof(tempbuf);
           }
           else
               thisR = atof(fieldList[rIndex]);

           // Increment the row count and the R**2 value for this index
           if (rowCount > 0)
           {
               numRows[index] = numRows[index] + rowCount;
               R[index] = R[index] + (thisR * rowCount);
               if (index > maxIndex)
                   maxIndex = index;
           }
       }
   }

   // Determine the weighted avg of R
   for(i=0; i<=maxIndex; i++)
       R[i] = R[i] / (float)numRows[i];

   // write out the results
   fputs("x,y,by,numRows,R\n", stdout);
   for(i=0; i<=maxIndex; i++)
   {
       fprintf(stdout, "%s,%s,%s,%d,%.10f\n", X[i], Y[i], by[i], numRows[i], R[i]);
   }
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
