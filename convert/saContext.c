/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "saContext.h"

#define MAXBUFLEN 80
#define MAXBUFSIZE (1024 * 32)

char *eatTimestamp(char **);
char *getAttribute(char **, char);
char *getLine(FILE *);
char *getValue(char **);
char *trim(char *);
time_t getTimestamp(char *);

saContextTypePtr saContextInit(char *name, double domainMin, double domainMax, double avg, 
                               double sdev, int count, int numTerms, char *type, char *notes)
{
    saContextTypePtr p = (saContextTypePtr)malloc(sizeof(saContextType));
    if (p == NULL)
        return(NULL);
    p->name = malloc(strlen(name)+1);
    strcpy(p->name, name);
    p->domainMin = domainMin;
    p->domainMax = domainMax;
    p->avg = avg;
    p->sdev = sdev;
    p->count = count;
    p->numSemanticTerms = numTerms;
    p->type = malloc(strlen(type)+1);
    strcpy(p->type, type);
    p->notes = malloc(strlen(notes)+1);
    strcpy(p->notes, notes);
    int i;
    for(i=0; i<SA_CONTEXT_MAXTERMS; i++)
        p->semanticTerms[i] = NULL;
    return(p);
}

saContextTypePtr saContextLoad(FILE *infile)
{
    saContextTypePtr termSetPtr;
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
    termSetPtr = malloc(sizeof(saContextType));
    memset(termSetPtr, 0, sizeof(saContextType));
    char *line = getLine(infile);
    cursor = line;
    cursor = eatTimestamp(&cursor);
    while(*cursor != '\0')
    {
        attribute = getAttribute(&cursor, '=');
        value = getValue(&cursor);
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
            else if (!strcmp(attribute, "domainMax"))
            {
                termSetPtr->domainMax = (double)atof(value);
            }
            else if (!strcmp(attribute, "domainMin"))
            {
                termSetPtr->domainMin = (double)atof(value);
            }
            else if (!strcmp(attribute, "numSemanticTerms"))
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
        line = getLine(infile);
        if (line != NULL)
        {
            cursor = line;
            cursor = eatTimestamp(&cursor);
            saSemanticTermTypePtr p = malloc(sizeof(saSemanticTermType));
            memset(p, 0, sizeof(saSemanticTermType));
            char *termSetName;
            while(*cursor != '\0')
            {
                attribute = getAttribute(&cursor, '=');
                value = getValue(&cursor);
                // ADD IN ALL OF THE ATTR/VALUE ASSIGNMENTS!!!
                if (!strcmp(attribute, "alfacut"))
                    p->alfacut = atof(value);
                else if (!strcmp(attribute, "domainMax"))
                    p->domainMax = atof(value);
                else if (!strcmp(attribute, "domainMin"))
                    p->domainMin = atof(value);
                else if (!strcmp(attribute, "numPoints"))
                    p->numPoints = atoi(value);
                else if (!strncmp(attribute, "point", 5))
                {
                    int index = atoi(attribute+5)-1;
                    if (index >= 0)
                        p->points[index] = atof(value);
                } 
                else if (!strcmp(attribute, "name"))
                    strcpy(p->name, value);
                else if (!strncmp(attribute, "truth", 5))
                {
                    int idx = atoi(attribute+5);
                    char *tuple = value;
                    p->indexVector[idx] = atof(getAttribute(&tuple, ':'));
                    p->vector[idx] = atof(getValue(&tuple));
                }
            }
            termSetPtr->semanticTerms[numSemanticTerms++] = p;
            free(line);
        }
        else
            done = true;
    }
    return(termSetPtr);
}

char *eatTimestamp(char **cursor)
{

    char *s = strstr(*cursor, SA_CONTEXT_TIMESTAMP_SEPARATOR);
    if (*s != '\0')
        s += strlen(SA_CONTEXT_TIMESTAMP_SEPARATOR)+1;
    *cursor = s;
    return(s);
}

char *getLine(FILE *infile)
{
    if (feof(infile))
        return(NULL);
    char *inBuf = malloc(sizeof(char)*MAXBUFSIZE);
    fgets(inBuf, MAXBUFSIZE, infile);
    if (inBuf[strlen(inBuf)-1] == '\n')
        inBuf[strlen(inBuf)-1] = '\0';
    return(inBuf);
}

char *getAttribute(char **cursor, char separator)
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

char *getValue(char **cursor)
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


bool saContextSave(FILE *outfile, saContextTypePtr contextPtr)
{
    char timeStr[MAXBUFLEN+1];
    time_t ct = getTimestamp(timeStr);
    if (contextPtr->numSemanticTerms == 0)
        return(false);
    
    fprintf(outfile, "%s %s record=header version=%d context=%s", timeStr, 
            SA_CONTEXT_TIMESTAMP_SEPARATOR, (int)ct, contextPtr->name);
    fprintf(outfile, " count=%d", contextPtr->count);
    fprintf(outfile, " domainMin=%.10f", contextPtr->domainMin);
    fprintf(outfile, " domainMax=%.10f", contextPtr->domainMax);
    fprintf(outfile, " avg=%.10f", contextPtr->avg);
    fprintf(outfile, " sdev=%.10f", contextPtr->sdev);
    fprintf(outfile, " type=%s", contextPtr->type);
    fprintf(outfile, " numSemanticTerms=%d", contextPtr->numSemanticTerms);
    fprintf(outfile, " notes=\"%s\"", contextPtr->notes);
    fprintf(outfile, "\n");

    int i, j;
    for(i=0; i< contextPtr->numSemanticTerms; i++)
    {
        saSemanticTermTypePtr p = contextPtr->semanticTerms[i];
        fprintf(outfile, "%s %s record=set version=%d context=%s", timeStr, 
                SA_CONTEXT_TIMESTAMP_SEPARATOR, (int)ct, contextPtr->name);
        fprintf(outfile, " name=%s", p->name);
        fprintf(outfile, " numpoints=%d", p->numPoints);
        fprintf(outfile, " domainMin=%.4f", p->domainMin);
        fprintf(outfile, " domainMax=%.4f", p->domainMax);
        fprintf(outfile, " alfacut=%.3f", p->alfacut);
        j=0;
        while(j < p->numPoints)
	      fprintf(outfile, " point%d=%.10f", j, p->points[j++]);
        
        for(j=0; j<SA_SEMANTICTERM_VECTORSIZE; j++)
            fprintf(outfile, " truth%d=%1.10f:%.10f", j, p->indexVector[j], p->vector[j]);
        fprintf(outfile, "\n");
        fflush(outfile);
    }
    return(true);
}

time_t getTimestamp(char *buf)
{
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buf, MAXBUFLEN, "%a %b %d %Y %T", timeinfo);
    return(rawtime);
}

char *trim(char *str)
{
    while(*str == ' ' && *str != '\0')
        str++;
    return(str);
}

void saContextDisplayWithHeader(saContextTypePtr termSetPtr, char *name)
{
    char output[2048];
    char val[32];
    int i, j;

    fputs(name, stdout);
    for(i=0; i<termSetPtr->numSemanticTerms; i++)
        fprintf(stdout, ",%s", termSetPtr->semanticTerms[i]->name);
    fputs("\n", stdout);

    for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
    {
        fprintf(stdout, "%1.5f", termSetPtr->semanticTerms[0]->indexVector[i]);
        for(j=0; j<termSetPtr->numSemanticTerms; j++)
            fprintf(stdout, ",%1.10f", termSetPtr->semanticTerms[j]->vector[i]);
        fputs("\n", stdout);
    }
    return;
}

void saContextDisplay(saContextTypePtr termSetPtr)
{
    saContextDisplayWithHeader(termSetPtr, termSetPtr->name);
}

