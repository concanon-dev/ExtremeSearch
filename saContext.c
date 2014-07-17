/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: saContext

Description:
    All of the functions necessary to manipulate the "context" data structure, including reading from/writing
    to a file.

Functions:
    External:
    saContextDisplay
    saContextDisplayWithHeader
    saContextInit
    saContextLoad
    saContextLookup
    saContextMerge
    saContextSave

    Internal:
    eatTimestamp
    getAttribute
    getLine
    getTimestamp
    getValue
    trim

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

inline char *eatTimestamp(char **);
inline char *getAttribute(char **, char);
inline char *getLine(FILE *);
inline char *getValue(char **);
inline char *trim(char *);
inline time_t getTimestamp(char *);

extern saContextTypePtr saContextCreateAvgCentered(char *, double, double, char *[], int, char *,
                                                   char *, int, char *, char *);
extern saContextTypePtr saContextCreateDomain(char *, double, double, char *[], int, char *,
                                              char *, int, char *, char *);
extern saContextTypePtr saContextCreateMedianCentered(char *, double, double, char *[], int, char *,
                                                      char *, int, char *, char *);

inline saContextTypePtr saContextInit(char *name, double domainMin, double domainMax, double avg, 
                               double sdev, int count, int numConcepts, char *type, 
                               char *notes, char *uom)
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
    p->numConcepts = numConcepts;
    p->type = malloc(strlen(type)+1);
    strcpy(p->type, type);
    p->notes = malloc(strlen(notes)+1);
    strcpy(p->notes, notes);
    p->uom = uom; 
    int i;
    for(i=0; i<SA_CONTEXT_MAXTERMS; i++)
        p->concepts[i] = NULL;
    return(p);
}

inline saContextTypePtr saContextLoad(FILE *infile)
{
    saContextTypePtr contextPtr;
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
    contextPtr = malloc(sizeof(saContextType));
    memset(contextPtr, 0, sizeof(saContextType));
    contextPtr->uom = "";
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
                contextPtr->avg = (double)atof(value);
            }
            else if (!strcmp(attribute, "count"))
            {
                contextPtr->count = atoi(value);
            }
            else if (!strcmp(attribute, "domainMax"))
            {
                contextPtr->domainMax = (double)atof(value);
            }
            else if (!strcmp(attribute, "domainMin"))
            {
                contextPtr->domainMin = (double)atof(value);
            }
            else if (!strcmp(attribute, "numConcepts") || !strcmp(attribute, "numSemanticTerms"))
            {
                contextPtr->numConcepts = atoi(value);
            }
            else if (!strcmp(attribute, "sdev"))
            {
                contextPtr->sdev = (double)atof(value);
            }
            else if (!strcmp(attribute, "context"))
            {
                contextPtr->name = malloc(sizeof(char)*(strlen(value)+1));
                strcpy(contextPtr->name, value);
            }
            else if (!strcmp(attribute, "type"))
            {
                contextPtr->type = malloc(sizeof(char)*(strlen(value)+1));
                strcpy(contextPtr->type, value);
            }
            else if (!strcmp(attribute, "notes"))
            {
                contextPtr->notes = malloc(sizeof(char)*(strlen(value)+1));
                strcpy(contextPtr->notes, value);
            }
            else if (!strcmp(attribute, "format"))
            {
                contextPtr->format = malloc(sizeof(char)*(strlen(value)+1));
                strcpy(contextPtr->format, value);
            }
            else if (!strcmp(attribute, "uom"))
            {
                contextPtr->uom = malloc(sizeof(char)*(strlen(value)+1));
                strcpy(contextPtr->uom, value);
            }
        }
    }
    free(line);
    int numConcepts = 0;
    done = false;
    while(numConcepts < contextPtr->numConcepts && !done)
    {
        line = getLine(infile);
        if (line != NULL)
        {
            cursor = line;
            cursor = eatTimestamp(&cursor);
            saConceptTypePtr p = malloc(sizeof(saConceptType));
            memset(p, 0, sizeof(saConceptType));
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
                {
                    p->name = malloc(strlen(value)+1);
                    strcpy(p->name, value);
                }
                else if (!strcmp(attribute, "shape"))
                {
                    p->shape = malloc(strlen(value)+1);
                    strcpy(p->shape, value);
                }
                else if (!strcmp(attribute, "format"))
                {
                    p->format = malloc(sizeof(char)*(strlen(value)+1));
                    strcpy(p->format, value);
                }
                else if (!strncmp(attribute, "truth", 5))
                {
                    int idx = atoi(attribute+5);
                    char *tuple = value;
                    p->indexVector[idx] = atof(getAttribute(&tuple, ':'));
                    p->vector[idx] = atof(getValue(&tuple));
                }
            }
            contextPtr->concepts[numConcepts++] = p;
            free(line);
        }
        else
            done = true;
    }
    return(contextPtr);
}

inline saContextTypePtr saContextMerge(saContextTypePtr c1, saContextTypePtr c2, char *c3Name)
{
    int i;

    // confirm that the types of contexts are the same
    if (strcmp(c1->type, c2->type))
        return(NULL);

    // To merge two contexts, the concepts must match exactly (number and name)
    if (c1->numConcepts != c2->numConcepts)
        return(NULL);

    // make sure the concept names are the same
    for(i=0; i<c1->numConcepts; i++)
        if (strcmp(c1->concepts[i]->name, c2->concepts[i]->name))
            return(NULL);

    // Get the weights
    // If the counts are both 0, then make them both 1
    // If either count is 0, make them both equal to the non-zero amount
    int c1Count = c1->count;
    int c2Count = c2->count;
    int c3Count = c1->count + c2Count;
    if (c1Count == 0 && c2Count == 0)
    {
        c1Count = 1;
        c2Count = 1;
    }
    else if (c1Count == 0)
    {
        c1Count = c2Count;
        c3Count = 0;
    }
    else if (c2Count == 0)
    {
        c2Count = c1Count;
        c3Count = 0;
    }
    double c1Weight = (double)c1Count/(double)(c1Count + c2Count);
    double c2Weight = (double)c2Count/(double)(c1Count + c2Count);

    // get the names
    char *conceptNames[SA_CONTEXT_MAXTERMS];
    for(i=0; i<c1->numConcepts; i++)
    {
        conceptNames[i] = (char *)malloc(strlen(c1->concepts[i]->name)+1);
        strcpy(conceptNames[i], c1->concepts[i]->name);
    }

    // check the number of concepts and assign shapes/endshapes based on that number
    char *shapeStr = c1->concepts[0]->shape;
    char *endShapeStr = c1->concepts[0]->shape;
    if (!strncmp(endShapeStr, "linear", 6))
        endShapeStr = SA_CONCEPT_SHAPE_LINEAR;
    else
        endShapeStr = SA_CONCEPT_SHAPE_CURVE;
    if (c1->numConcepts > 2)
        shapeStr = c1->concepts[1]->shape;

    // Go out and create the context based on type
    if (!strcmp(c1->type, SA_CONTEXT_TYPE_AVERAGE_CENTERED))
    {
        // adjust the fields by weight
        double avg = c1->avg * c1Weight + c2->avg * c2Weight;
        double sdev = c1->sdev * c1Weight + c2->sdev * c2Weight;
        return(saContextCreateAvgCentered(c3Name, avg, sdev, conceptNames, c1->numConcepts,
                                          shapeStr, endShapeStr, c3Count, c1->notes, c1->uom));
    }
    else if (!strcmp(c1->type, SA_CONTEXT_TYPE_DOMAIN))
    {
        // adjust the fields by weight
        double domainMin = c1->domainMin * c1Weight + c2->domainMin * c2Weight;
        double domainMax = c1->domainMax * c1Weight + c2->domainMax * c2Weight;
        return(saContextCreateDomain(c3Name, domainMin, domainMax, conceptNames, c1->numConcepts, 
                                     shapeStr, endShapeStr, c3Count, c1->notes, c1->uom));
    }
    else if (!strcmp(c1->type, SA_CONTEXT_TYPE_MEDIAN_CENTERED))
    {
        // adjust the fields by weight
        double avg = c1->avg * c1Weight + c2->avg * c2Weight;
        double sdev = c1->sdev * c1Weight + c2->sdev * c2Weight;
        return(saContextCreateMedianCentered(c3Name, avg, sdev, conceptNames, c1->numConcepts,
                                             shapeStr, endShapeStr, c3Count, c1->notes, c1->uom));
    }

    return(NULL);
}

inline char *eatTimestamp(char **cursor)
{

    char *s = strstr(*cursor, SA_CONTEXT_TIMESTAMP_SEPARATOR);
    if (*s != '\0')
        s += strlen(SA_CONTEXT_TIMESTAMP_SEPARATOR)+1;
    *cursor = s;
    return(s);
}

inline char *getLine(FILE *infile)
{
    if (feof(infile))
        return(NULL);
    char *inBuf = malloc(sizeof(char)*MAXBUFSIZE);
    fgets(inBuf, MAXBUFSIZE, infile);
    if (inBuf[strlen(inBuf)-1] == '\n')
        inBuf[strlen(inBuf)-1] = '\0';
    return(inBuf);
}

inline char *getAttribute(char **cursor, char separator)
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

inline char *getValue(char **cursor)
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


inline bool saContextSave(FILE *outfile, saContextTypePtr contextPtr)
{
    char timeStr[MAXBUFLEN+1];
    time_t ct = getTimestamp(timeStr);
    if (contextPtr->numConcepts == 0)
        return(false);

    fprintf(outfile, "%s %s format=%s record=header version=%d context=%s", timeStr, 
            SA_CONTEXT_TIMESTAMP_SEPARATOR, SA_CONTEXT_FORMAT, (int)ct, contextPtr->name);
    fprintf(outfile, " count=%d", contextPtr->count);
    fprintf(outfile, " domainMin=%.10f", contextPtr->domainMin);
    fprintf(outfile, " domainMax=%.10f", contextPtr->domainMax);
    fprintf(outfile, " avg=%.10f", contextPtr->avg);
    fprintf(outfile, " sdev=%.10f", contextPtr->sdev);
    fprintf(outfile, " type=%s", contextPtr->type);
    fprintf(outfile, " numConcepts=%d", contextPtr->numConcepts);
    fprintf(outfile, " notes=\"%s\"", contextPtr->notes);
    fprintf(outfile, " uom=\"%s\"", contextPtr->uom);
    fprintf(outfile, "\n");

    int i, j;
    for(i=0; i< contextPtr->numConcepts; i++)
    {
        saConceptTypePtr p = contextPtr->concepts[i];
        fprintf(outfile, "%s %s format=%s record=set version=%d context=%s", timeStr, 
                SA_CONTEXT_TIMESTAMP_SEPARATOR, SA_CONTEXT_FORMAT, (int)ct, contextPtr->name);
        fprintf(outfile, " name=%s", p->name);
        fprintf(outfile, " shape=%s", p->shape);
        fprintf(outfile, " domainMin=%.4f", p->domainMin);
        fprintf(outfile, " domainMax=%.4f", p->domainMax);
        fprintf(outfile, " alfacut=%.3f", p->alfacut);
        fprintf(outfile, " numpoints=%d", p->numPoints);
        j=0;
        while(j < p->numPoints)
        {
	      fprintf(outfile, " point%d=%.10f", j, p->points[j]);
              j++;
        }
        for(j=0; j<SA_CONCEPT_VECTORSIZE; j++)
            fprintf(outfile, " truth%d=%1.10f:%.10f", j, p->indexVector[j], p->vector[j]);
        
        fprintf(outfile, "\n");
        fflush(outfile);
    }
    return(true);
}

inline time_t getTimestamp(char *buf)
{
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buf, MAXBUFLEN, "%a %b %d %Y %T", timeinfo);
    return(rawtime);
}

inline char *trim(char *str)
{
    while(*str == ' ' && *str != '\0')
        str++;
    return(str);
}

inline void saContextDisplayWithHeader(saContextTypePtr contextPtr, char *name)
{
    char output[2048];
    char val[32];
    int i, j;

    fputs(name, stdout);
    for(i=0; i<contextPtr->numConcepts; i++)
        fprintf(stdout, ",%s", contextPtr->concepts[i]->name);
    fputs("\n", stdout);

    for(i=0; i<SA_CONCEPT_VECTORSIZE; i++)
    {
        fprintf(stdout, "%1.5f", contextPtr->concepts[0]->indexVector[i]);
        for(j=0; j<contextPtr->numConcepts; j++)
            fprintf(stdout, ",%1.10f", contextPtr->concepts[j]->vector[i]);
        fputs("\n", stdout);
    }
    return;
}

inline void saContextDisplay(saContextTypePtr contextPtr)
{
    saContextDisplayWithHeader(contextPtr, contextPtr->name);
}


inline void saContextLookup(saContextTypePtr contextPtr, double value, double *results)
{
    double bucket_size = (contextPtr->domainMax - contextPtr->domainMin) 
                          / (float) SA_CONCEPT_VECTORSIZE;
    int vector_index = (int)round((value - contextPtr->domainMin) / bucket_size);
    if (vector_index > (SA_CONCEPT_VECTORSIZE - 1))
        vector_index = SA_CONCEPT_VECTORSIZE - 1;
    else if (vector_index < 0)
        vector_index = 0;

    int k;
    for(k=0; k<contextPtr->numConcepts; k++)
        results[k] = contextPtr->concepts[k]->vector[vector_index];
}

