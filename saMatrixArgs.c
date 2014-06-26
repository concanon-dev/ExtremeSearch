/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: saMatrixArgs

Description:
    Build a matrix of X by X arguments  and return them as two lists, x and y.  For example, if x is passed in 
    as the string a,b,c then set x to "a,b,c,a,b,c,a,b,c" and set y to "a,b,c,b,c,a,c,a,b"

Functions:
    
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void saMatrixArgs(char *x, char *y, int size)
{
    char *tmp;
    char *xList[size];
    char z[size];
    int numFields=0;

    // save the original list
    strcpy(z, x);

    // parse the list into discrete strings
    xList[numFields++] = strtok(z, ",");
    while((tmp = strtok(NULL, ",")) != NULL)
       xList[numFields++] = tmp;

    // initialize the array
    int i, j;
    for(i=0; i<size; i++)
    {
        x[i] = '\0';
        y[i] = '\0';
    }

    // build the X list as a comma-delimited string
    for(i=0; i<numFields; i++)
    {
        for(j=0; j<numFields; j++)
        {
            strcat(x, xList[j]);
            strcat(x, ",");
        }
    }

    // build the Y list as a comma-delimited string
    for(i=0; i<numFields; i++)
    {
        j=i;
        int k=0;
        while(k<numFields)
        {
              strcat(y, xList[j++]);
              strcat(y, ",");
              if (j==numFields)
                  j=0;
              k++;
        }
    }
    x[strlen(x)-1] = '\0';
    y[strlen(y)-1] = '\0';
}
