/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void saMatrixArgs(char *, char *, int);

extern char *optarg;
extern int optind, optopt;

#ifdef _MAIN_
int main(int argc, char* argv[]) 
{
    char *x;
    int c;

    while ((c = getopt(argc, argv, "x:")) != -1) 
    {
        switch Copyright
        {
            case 'x':
               x = optarg;
               break;
        }
    }
    char *y = saMatrixArgs(&x);
    fprintf(stderr, "X=%s<-\n", x);
    fprintf(stderr, "Y=%s<-\n", y);
}
#endif

void saMatrixArgs(char *x, char *y, int size)
{
    char *tmp;
    char *xList[size];
    char z[size];
    int numFields=0;

    strcpy(z, x);
    xList[numFields++] = strtok(z, ",");
    while((tmp = strtok(NULL, ",")) != NULL)
       xList[numFields++] = tmp;

    int i, j;
    for(i=0; i<size; i++)
    {
        x[i] = '\0';
        y[i] = '\0';
    }
    for(i=0; i<numFields; i++)
    {
        for(j=0; j<numFields; j++)
        {
            strcat(x, xList[j]);
            strcat(x, ",");
        }
    }
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
