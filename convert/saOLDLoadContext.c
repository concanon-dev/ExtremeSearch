#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "OLDsaSemanticTerm.h"
#include "OLDsaContext.h"

#define MAXBUFSIZE (1024 * 32)

char *OLDgetAttribute(char **, char);
char *OLDgetLine(FILE *);
char *OLDeatTimestamp(char **);
char *OLDgetValue(char **);

OLDsaContextPtr saFuzzyLoadContext(FILE *infile)
{
    OLDsaContextPtr termSetPtr;
    bool done;
    char *attribute;
    char *cursor;
    char *value;
 /*
  * get the first line
  * build the termSet
  * for each fuzzy set
  *    get the line
  *    build the fuzzy set
  *    insert the fuzzy set in termSet
  */   
    termSetPtr = malloc(sizeof(OLDsaContextType));
    memset(termSetPtr, 0, sizeof(OLDsaContextType));
    char *line = OLDgetLine(infile);
    cursor = line;
    cursor = OLDeatTimestamp(&cursor);
    while(*cursor != '\0')
    {
        attribute = OLDgetAttribute(&cursor, '=');
        value = OLDgetValue(&cursor);
        if (attribute != NULL && value != NULL)
        {
            if (!strcmp(attribute, "avg"))
            {
                termSetPtr->avg = (double)atof(value);
            }
            else if (!strcmp(attribute, "count"))
            {
                termSetPtr->count = atoi(value);
            }
            else if (!strcmp(attribute, "max"))
            {
                termSetPtr->max = (double)atof(value);
            }
            else if (!strcmp(attribute, "min"))
            {
                termSetPtr->min = (double)atof(value);
            }
            else if (!strcmp(attribute, "numsemanticterms"))
            {
                termSetPtr->numSemanticTerms = atoi(value);
            }
            else if (!strcmp(attribute, "sdev"))
            {
                termSetPtr->sdev = (double)atof(value);
            }
            else if (!strcmp(attribute, "context"))
            {
                termSetPtr->name = malloc(sizeof(char)*(strlen(value)+1));
                strcpy(termSetPtr->name, value);
            }
            else if (!strcmp(attribute, "type"))
            {
                termSetPtr->type = malloc(sizeof(char)*(strlen(value)+1));
                strcpy(termSetPtr->type, value);
            }
            else if (!strcmp(attribute, "notes"))
            {
                termSetPtr->notes = malloc(sizeof(char)*(strlen(value)+1));
                strcpy(termSetPtr->notes, value);
            }

        }
    }
    free(line);
    int numSemanticTerms = 0;
    done = false;
    while(numSemanticTerms<termSetPtr->numSemanticTerms && !done)
    {
        line = OLDgetLine(infile);
        if (line != NULL)
        {
            cursor = line;
            cursor = OLDeatTimestamp(&cursor);
            OLDsaSemanticTermTypePtr f = malloc(sizeof(OLDsaSemanticTermType));
            memset(f, 0, sizeof(OLDsaSemanticTermType));
            char *termSetName;
            while(*cursor != '\0')
            {
                attribute = OLDgetAttribute(&cursor, '=');
                value = OLDgetValue(&cursor);
                // ADD IN ALL OF THE ATTR/VALUE ASSIGNMENTS!!!
                if (!strcmp(attribute, "alfacut"))
                    f->alfacut = atof(value);
                else if (!strcmp(attribute, "ctlswitch"))
                    f->type = atoi(value);
                else if (!strcmp(attribute, "description"))
                    strcpy(f->description, value);
                else if (!strcmp(attribute, "high"))
                    f->domain[1] = atof(value);
                else if (!strcmp(attribute, "low"))
                    f->domain[0] = atof(value);
                else if (!strcmp(attribute, "order"))
                    f->setOrder = atoi(value);
                else if (!strcmp(attribute, "paramcount"))
                    f->numParameters = atoi(value);
                else if (!strncmp(attribute, "parm", 4))
                {
                    int index = atoi(attribute+4)-1;
                    if (index >= 0)
                        f->parameters[index] = atof(value);
                } 
                else if (!strcmp(attribute, "term"))
                    strcpy(f->name, value);
                else if (!strncmp(attribute, "truth", 5))
                {
                    int idx = atoi(attribute+5);
                    char *tuple = value;
                    f->indexVector[idx] = atof(OLDgetAttribute(&tuple, ':'));
                    f->vector[idx] = atof(OLDgetValue(&tuple));
                }
            }
            termSetPtr->semanticTerms[numSemanticTerms++] = f;
            free(line);
        }
        else
            done = true;
    }
    return(termSetPtr);
}

char *OLDeatTimestamp(char **cursor)
{

    char *s = strstr(*cursor, TIMESTAMP_SEPARATOR);
    if (*s != '\0')
        s += strlen(TIMESTAMP_SEPARATOR)+1;
    *cursor = s;
    return(s);
}

char *OLDgetLine(FILE *infile)
{
    if (feof(infile))
        return(NULL);
    char *inBuf = malloc(sizeof(char)*MAXBUFSIZE);
    fgets(inBuf, MAXBUFSIZE, infile);
    if (inBuf[strlen(inBuf)-1] == '\n')
        inBuf[strlen(inBuf)-1] = '\0';
    return(inBuf);
}

char *OLDgetAttribute(char **cursor, char separator)
{
    char *ptr = *cursor;
    char *s = *cursor;

    while(*ptr == ' ')
    {
          ptr++;
          s++;
    }

    while(*ptr != separator && *ptr != '\0')
          ptr++;

    if (*ptr == separator)
    {
        *ptr = '\0';
        *cursor = ptr + 1;
    }
    else
        *cursor = ptr;
    return(s);
}

char *OLDgetValue(char **cursor)
{
    char *ptr = *cursor;
    char *s = *cursor;
    char termChar = ' ';

    if (*s == '"')
    {
        termChar = '"';
        s++;
        ptr++;
    }

    while(*ptr != termChar && *ptr != '\0')
          ptr++;

    if (*ptr == termChar)
    {
        *ptr = '\0';
        *cursor = ptr+1;
    }
    else
        *cursor = ptr;
    return(s);
}

