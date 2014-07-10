/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: saCSV3

Description:
    Provide all functions necessary to read in CSV-formatted data.

Functions:
    External:

    Internal:
*/
#define _CSV3_C
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "saCSV3.h"

//static saCSVType csv;

#define SA_CSV_MAXSTRING 1024

#define FIRST_CHAR 0
#define NORMAL_FIELD 1
#define QUOTED_FIELD 2


inline int get(saCSVTypePtr csvPtr)
{
    int c;

    if (csvPtr->pointer < csvPtr->size)
    {
        c = csvPtr->buffer[csvPtr->pointer++];
    }
    else
    {
        csvPtr->size = fread((void *)csvPtr->buffer, (size_t) sizeof(char), (size_t)MAXSIZE, csvPtr->csvFile);
        if (csvPtr->size <= 0)
            return(-1);
        csvPtr->pointer = 1;
        c = csvPtr->buffer[0];
    }
    return(c);
}

inline bool saCSVEOF(saCSVTypePtr csvPtr)
{
    if (csvPtr->size <= 0)
        return(true);
    return(false);
}

inline void unget(saCSVTypePtr csvPtr)
{
    csvPtr->pointer--;
}


inline int saCSV3GetLine(saCSVTypePtr csvPtr, char inbuf[], char *fields[])
{
    char separator = ',';

    int i = 0;
    int numFields = 0;
    int state = FIRST_CHAR;
    int done = 0;
    while(!done)
    {
          inbuf[i] = get(csvPtr);
          if (saCSVEOF(csvPtr))
          {
              done = 1;
              inbuf[i] = '\0';
          }
          else if (state == FIRST_CHAR)
          {
              fields[numFields++] = &(inbuf[i]);
              if (inbuf[i] == '"')
                  state = QUOTED_FIELD;
              else if (inbuf[i] == separator) 
              {
                  inbuf[i] ='\0';
              }
              else if (inbuf[i] == 13)
              {
                  /* must be dos so eat the 'nl' */
                  char c = get(csvPtr);
                  inbuf[i] = '\0';
                  done = 1;
              }
              else if (inbuf[i] == '\n')
              {
                  inbuf[i] ='\0';
                  done = 1;
              }
              else if (inbuf[i] == '\\')
              {
                  char c = get(csvPtr);
                  if (!saCSVEOF(csvPtr))
                  {
                      inbuf[i] = c;
                      state = NORMAL_FIELD;
                  }
              }
              else 
                  state = NORMAL_FIELD;
              i++;
          }
          else if (state == NORMAL_FIELD)
          {
              if (inbuf[i] == separator)
              {
                  state = FIRST_CHAR;
                  inbuf[i] = '\0';
                  i++;
              }
              else if (inbuf[i] == 13)
              {
                  /* must be dos so eat the 'n' */
                  char c = get(csvPtr);
                  inbuf[i] = '\0';
                  done = 1;
              }
              else if (inbuf[i] == '\n')
              {
                  inbuf[i] = '\0';
                  done = 1;
              }
              else if (inbuf[i] == '\\')
              {
                  char c = get(csvPtr);
                  if (!saCSVEOF(csvPtr))
                      inbuf[i] = c;
                  i++;
              }
              else
                  i++;
          }
          else if (state == QUOTED_FIELD)
          {
              if (inbuf[i] == '"')
              {
                  char c = get(csvPtr);
                  if (c == separator || c == '\n' || c == 13)
                  {
                      state = FIRST_CHAR; 
                      inbuf[i+1] = '\0';
                      i += 2;
                      if (c == 13)
                          c = get(csvPtr);
                      done = (c == '\n');
                  }
                  else if (c == '"')
                  {
                      inbuf[i+1] = c;
                      i += 2;
                  }
                  else 
                  {
                      unget(csvPtr);
                      i++;
                  }
              } 
              else if (inbuf[i] == '\\')
              {
                  char c = get(csvPtr);
                  if (!saCSVEOF(csvPtr))
                      inbuf[i] = c;
                  i++;
              }
              else
                  i++;
          }
    }
    return(numFields);
}

inline void saCSVOpen(saCSVTypePtr csvPtr, FILE *f)
{
    csvPtr->csvFile = f;
    csvPtr->pointer = 0;
    csvPtr->size = 0;
}


/*
int main(int argc, char *argv[])
{
    char inbuf[MAXSIZE];
    char *fields[256];

    // FILE *f = fopen(argv[1], "r");
    saCSVOpen(&csv, stdin);
    // saCSVOpen(&csv, f);
    bool done = false;
    while(done == false)
    {
        int numFields = saCSV3GetLine(&csv, inbuf, fields);
        if (numFields != 0)
        {
            int i;
            for(i=0; i<numFields; i++)
            {
                if (i == 0)
                    fputs(fields[0], stdout);
                else
                    fprintf(stdout, ",%s", fields[i]);
            }
            fputs("\n", stdout);
        }
        else
            done = true;
    }
}
*/
