/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SA_CSV_MAXSTRING 1024

#define FIRST_CHAR 0
#define NORMAL_FIELD 1
#define QUOTED_FIELD 2

int saCSVFGetLine(FILE *f, char inbuf[], char *fields[])
{
    char separator = ',';

    int i = 0;
    int numFields = 0;
    int state = FIRST_CHAR;
    int done = 0;
    while(!done)
    {
          inbuf[i] = fgetc(f);
          if (feof(f))
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
                  char c = fgetc(f);
                  inbuf[i] = '\0';
                  done = 1;
              }
              else if (inbuf[i] == '\n')
              {
                  inbuf[i] ='\0';
                  done = 1;
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
                  char c = fgetc(f);
                  inbuf[i] = '\0';
                  done = 1;
              }
              else if (inbuf[i] == '\n')
              {
                  inbuf[i] = '\0';
                  done = 1;
              }
              else
                  i++;
          }
          else if (state == QUOTED_FIELD)
          {
              if (inbuf[i] == '"')
              {
                  char c = fgetc(f);
                  if (c == separator || c == '\n' || c == 13)
                  {
                      state = FIRST_CHAR; 
                      inbuf[i+1] = '\0';
                      i += 2;
                      if (c == 13)
                          c = fgetc(f);
                      done = (c == '\n');
                  }
                  else if (c == '"')
                  {
                      inbuf[i+1] = c;
                      i += 2;
                  }
                  else 
                  {
                      ungetc((int)c, f);
                      i++;
                  }
              } 
              else
                  i++;
          }
    }
    return(numFields);
}

int saCSVGetLine(char inbuf[], char *fields[])
{
    return(saCSVFGetLine(stdin, inbuf, fields));
}

int saCSVFGetHeaderLine(FILE *f, char inbuf[], char *fields[])
{
    int numFields = 0;
    inbuf[0] = '\0';
    if (fgets(inbuf, SA_CSV_MAXSTRING, f) != NULL)
    {
        int len = strlen(inbuf);
        if (len > 0)
        {
            if (inbuf[len-1] == '\n')
                inbuf[len-1] = '\0';
            if (len > 1)
            {
                fields[0] = inbuf;
                numFields = 1;
                char *s = inbuf;
                bool done = false;
                while(!done)
                {
                    if (*s == ':')
                    {
                        *s = '\0';
                        fields[1] = ++s;
                        numFields = 2;
                        done = true;
                    }
                    else if (*s == '\0')
                        done = true;
                    else
                        s++;
                }
            }
        }
    }
    return(numFields);
}

int saCSVGetHeaderLine(char inbuf[], char *fields[])
{
    return(saCSVFGetHeaderLine(stdin, inbuf, fields));
}

int saCSVCompareField(char *quotedStr, char str[])
{
    if (*quotedStr != '"')
        return(strcmp(quotedStr, str));

    char temp[SA_CSV_MAXSTRING];
    temp[0] = '\0';
    strncat(temp, quotedStr+1, strlen(quotedStr)-1);
    temp[strlen(quotedStr)-2] = '\0';
    return(strcmp(temp, str));
}

bool saCSVConvertToDouble(char *s, double *d)
{
    char *endptr;

    *d = strtod(s, &endptr);
    if (*endptr != '\0')
        return(false);
    return(true);
}

char *saCSVExtractField(char *quotedStr)
{
    if (*quotedStr != '"')
        return(quotedStr);

    int len = strlen(quotedStr);
    char *newStr = malloc(len-1);
    strncpy(newStr, quotedStr+1, len-2);
    newStr[len-2] = '\0';
    return(newStr);
}

int saCSVParseFieldList(char *fieldList[], char *fields)
{
    int count = 0;
    fieldList[count++] = fields;

    int i;
    int len = strlen(fields);
    for (i=1; i<len; i++)
    {
        if (fields[i] == ',')
        {
            fieldList[count++] = fields+i+1;
            fields[i] = '\0';
        }
    }
    return(count);
}
