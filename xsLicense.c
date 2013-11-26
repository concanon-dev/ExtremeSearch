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

#define MAXROWSIZE 256
#define MAXSTRING 1024
static char inbuf[MAXSTRING*1000];
static char *fieldList[MAXROWSIZE*4];

extern writeHeader;

int main(int argc, char* argv[]) 
{
   while(!feof(stdin))
   {
       int numFields = saCSVGetLine(inbuf, fieldList);
       if (!feof(stdin))
       {
           int i;
           for(i=0; i<numFields; i++)
               if (!i)
                   fputs(fieldList[i], stdout);
               else
                   fprintf(stdout, ",%s", fieldList[i]);
               fputs("\n", stdout);
       }
   }
   writeHeader = false;
   doMain(argc, argv, false);
}

