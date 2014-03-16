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
#include "saDoubleField.h"
#include "saField.h"
#include "saFloatField.h"
#include "saIntField.h"
#include "saProperties.h"
#include "saStringField.h"
#include "saTimeField.h"
#include "saUtils.h"

extern saDicePtr saDiceCreate(char *, char *);

extern bool saDoubleFieldIncrement(saDoubleFieldPtr);
extern bool saFloatFieldIncrement(saFloatFieldPtr);
extern bool saIntFieldIncrement(saIntFieldPtr);

extern int saPropertiesLookup(saPropertiesPtr, char *);
extern int saPropertiesLookupNext(saPropertiesPtr, char *, int);
extern int saPropertiesMatch(saPropertiesPtr, char *);

extern int saStringFieldCountLines(saStringFieldPtr);
extern bool saStringFieldIncrement(saStringFieldPtr);

extern long saTimeFieldCalcTime(char *);
extern long saTimeFieldConvertToSeconds(char *);
extern bool saTimeFieldGetDayHour(saTimeFieldPtr, int *);
extern bool saTimeFieldIncrement(saTimeFieldPtr);

bool saFieldSetValid(saFieldPtr, char *);

bool saFieldConfigure(saPropertiesPtr p, saFieldPtr fieldArray[])
{
    char lookupStr[1024];
    int i, k;

    for(i=0; i<SAFIELD_MAX; i++)
    {
        if (fieldArray[i] != NULL)
        {
            if (!strcmp(fieldArray[i]->type, SAFIELD_DOUBLE))
            {
                char *dice = NULL;
                char *diceWeight = NULL;

                saDoubleFieldPtr fPtr = (saDoubleFieldPtr)malloc(sizeof(saDoubleFieldType));
                sprintf(lookupStr, "field.%i.dice", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    if ((dice = saPropertiesGetValue(p, k)) == NULL)
                    {
                        fprintf(stderr, "field %d: dice can not be NULL\n", i);
                        return(false);
                    }
                }

                sprintf(lookupStr, "field.%i.init", i);
                if ((k = saPropertiesMatch(p, lookupStr)) == -1)
                {
                    fprintf(stderr, "init not set for field %d\n", i);
                    return(false);
                }
                fPtr->init = atof(saPropertiesGetValue(p, k));
                fPtr->value = fPtr->init;

                sprintf(lookupStr, "field.%i.increment", i);
                if ((k = saPropertiesMatch(p, lookupStr)) == -1)
                {
                    fprintf(stderr, "increment not set for field %d\n", i);
                    return(false);
                }
                if ((fPtr->increment = atof(saPropertiesGetValue(p, k))) == 0)
                {
                    fprintf(stderr, "field %d: increment can not be 0\n", i);
                    return(false);
                }

                sprintf(lookupStr, "field.%i.dice.weight", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    if ((diceWeight = saPropertiesGetValue(p, k)) == NULL)
                    {
                        fprintf(stderr, "field %d: dice weight can not be NULL, defaulting to \"1\"\n", i);
                        diceWeight = "1";
                    }
                }
                else
                    diceWeight = "1";

                sprintf(lookupStr, "field.%i.valid", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    saFieldSetValid(fieldArray[i], saPropertiesGetValue(p, k));
                }
                else
                    saFieldSetValid(fieldArray[i], SAFIELD_VALID_ALL);

                fieldArray[i]->fieldPtr = (void *)fPtr;
            }
            else if (!strcmp(fieldArray[i]->type, SAFIELD_FLOAT))
            {
                bool increment = false;
                bool init = false;
                char *dice = NULL;
                char *diceWeight = NULL;

                saFloatFieldPtr fPtr = (saFloatFieldPtr)malloc(sizeof(saFloatFieldType));
                sprintf(lookupStr, "field.%i.dice", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    if ((dice = saPropertiesGetValue(p, k)) == NULL)
                    {
                        fprintf(stderr, "field %d: dice can not be NULL\n", i);
                        return(false);
                    }
                }

                sprintf(lookupStr, "field.%i.init", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    fPtr->init = (float)atof(saPropertiesGetValue(p, k));
                    fPtr->value = fPtr->init;
                    init = true;
                }

                sprintf(lookupStr, "field.%i.increment", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    if ((fPtr->increment = (float)atof(saPropertiesGetValue(p, k))) == 0)
                    {
                        fprintf(stderr, "field %d: increment can not be 0\n", i);
                        return(false);
                    }
                    increment = true;
                }

                sprintf(lookupStr, "field.%i.dice.weight", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    if ((diceWeight = saPropertiesGetValue(p, k)) == NULL)
                    {
                        fprintf(stderr, "field %d: dice weight can not be NULL, defaulting to \"1\"\n", i);
                        diceWeight = "1";
                    }
                }
                else
                    diceWeight = "1";

                sprintf(lookupStr, "field.%i.decimal", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    if ((fPtr->decimal = atoi(saPropertiesGetValue(p, k))) <= 0)
                        fPtr->decimal = 2;
                }
                else
                    fPtr->decimal = 4;

                sprintf(lookupStr, "field.%i.valid", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    saFieldSetValid(fieldArray[i], saPropertiesGetValue(p, k));
                }
                else
                    saFieldSetValid(fieldArray[i], SAFIELD_VALID_ALL);

                if (dice == NULL && increment == false)
                {
                    fprintf(stderr, "field %d: field.%d.dice or field.%d.increment must be set\n", i, i, i);
                    return(false);
                }
                if (dice == NULL && init == false)
                {
                    fprintf(stderr, "field %d: field.%d.init must be set\n", i, i);
                    return(false);
                }
                if (dice != NULL)
                    fPtr->dice = saDiceCreate(dice, diceWeight);
                else
                    fPtr->dice = NULL;

                fieldArray[i]->fieldPtr = (void *)fPtr;
            }
            else if (!strcmp(fieldArray[i]->type, SAFIELD_INT))
            {
                bool increment = false;
                bool init = false;
                char *dice = NULL;
                char *diceWeight = NULL;

                saIntFieldPtr fPtr = (saIntFieldPtr)malloc(sizeof(saIntFieldType));
                sprintf(lookupStr, "field.%i.init", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    fPtr->init = atol(saPropertiesGetValue(p, k));
                    fPtr->value = fPtr->init;
                    init = true;
                }

                sprintf(lookupStr, "field.%i.dice", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    if ((dice = saPropertiesGetValue(p, k)) == NULL)
                    {
                        fprintf(stderr, "field %d: dice can not be NULL\n", i);
                        return(false);
                    }
                }

                sprintf(lookupStr, "field.%i.increment", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    if ((fPtr->increment = atol(saPropertiesGetValue(p, k))) == 0)
                    {
                        fprintf(stderr, "field %d: increment can not be 0\n", i);
                        return(false);
                    }
                    increment = true;
                }
                else
                    fPtr->increment = (long)0;

                sprintf(lookupStr, "field.%i.dice.weight", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    if ((diceWeight = saPropertiesGetValue(p, k)) == NULL)
                    {
                        fprintf(stderr, "field %d: dice weight can not be NULL, defaulting to \"1\"\n", i);
                        diceWeight = "1";
                    }
                }
                else
                    diceWeight = "1";

                sprintf(lookupStr, "field.%i.valid", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    saFieldSetValid(fieldArray[i], saPropertiesGetValue(p, k));
                }
                else
                    saFieldSetValid(fieldArray[i], SAFIELD_VALID_ALL);

                if (dice == NULL && increment == false)
                {
                    fprintf(stderr, "field %d: field.%d.dice or field.%d.increment must be set\n", i, i, i);
                    return(false);
                }
                if (dice == NULL && init == false)
                {
                    fprintf(stderr, "field %d: field.%d.init must be set\n", i, i);
                    return(false);
                }
                if (dice != NULL)
                    fPtr->dice = saDiceCreate(dice, diceWeight);
                else
                    fPtr->dice = NULL;
                fieldArray[i]->fieldPtr = (void *)fPtr;
            }
            else if (!strcmp(fieldArray[i]->type, SAFIELD_STRING))
            {
                saStringFieldPtr fPtr = (saStringFieldPtr)malloc(sizeof(saStringFieldType));
                fPtr->field = 0;
                fPtr->file[0] = '\0';
                fPtr->value[0] = '\0';
                sprintf(lookupStr, "field.%i.file", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    strcpy(fPtr->file, saPropertiesGetValue(p, k));
                    if (saStringFieldCountLines(fPtr) == 0)
                    {
                        fprintf(stderr, "field %d: file %s is empty or can't be opened\n", i, fPtr->file);
                        return(false);
                    }
                }

                sprintf(lookupStr, "field.%i.field", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                    fPtr->field = atoi(saPropertiesGetValue(p, k));

                sprintf(lookupStr, "field.%i.value", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                    strcpy(fPtr->value, saPropertiesGetValue(p, k));

                sprintf(lookupStr, "field.%i.valid", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    saFieldSetValid(fieldArray[i], saPropertiesGetValue(p, k));
                }
                else
                    saFieldSetValid(fieldArray[i], SAFIELD_VALID_ALL);

                if (fPtr->file[0] == '\0' && fPtr->value[0] == '\0')
                {
                    fprintf(stderr, "field %d: either field.%d.file or field.%d.value must be set\n", i, i, i);
                    return(false);
                }
                fieldArray[i]->fieldPtr = (void *)fPtr;
            }
            else if (!strcmp(fieldArray[i]->type, SAFIELD_TIMESTAMP))
            {
                saTimeFieldPtr fPtr = (saTimeFieldPtr)malloc(sizeof(saTimeFieldType));
                sprintf(lookupStr, "field.%i.format", i);
                if ((k = saPropertiesMatch(p, lookupStr)) == -1)
                {
                    fprintf(stderr, "format not set for field %d\n", i);
                    return(false);
                }
                fPtr->format = saPropertiesGetValue(p, k);

                sprintf(lookupStr, "field.%i.init", i);
                if ((k = saPropertiesMatch(p, lookupStr)) == -1)
                {
                    fprintf(stderr, "init not set for field %d\n", i);
                    return(false);
                }
                fPtr->init = saTimeFieldCalcTime(saPropertiesGetValue(p, k));
                fPtr->value = fPtr->init;

                sprintf(lookupStr, "field.%i.increment", i);
                if ((k = saPropertiesMatch(p, lookupStr)) == -1)
                {
                    fprintf(stderr, "increment not set for field %d\n", i);
                    return(false);
                }
                if ((fPtr->increment = saTimeFieldConvertToSeconds(saPropertiesGetValue(p, k))) == 0)
                {
                    fprintf(stderr, "field %d: increment can not be 0\n", i);
                    return(false);
                }

                sprintf(lookupStr, "field.%i.increment.variance", i);
                if ((k = saPropertiesMatch(p, lookupStr)) == -1)
                    fPtr->incrementVariance = atof(saPropertiesGetValue(p, k));
                else
                    fPtr->incrementVariance = (float)0.0;
                fPtr->incrementVariance = atof(saPropertiesGetValue(p, k));

                sprintf(lookupStr, "field.%i.valid", i);
                if ((k = saPropertiesMatch(p, lookupStr)) != -1)
                {
                    saFieldSetValid(fieldArray[i], saPropertiesGetValue(p, k));
                }
                else
                    saFieldSetValid(fieldArray[i], SAFIELD_VALID_ALL);

                fieldArray[i]->fieldPtr = (void *)fPtr;
            }
            else
            {
                fprintf(stderr, "Unknown field type: %s\n", fieldArray[i]->type);
                return(false);
            }
        }
    }
    saStringFieldPtr sp = fieldArray[3]->fieldPtr;
    return(true);
}

void *saFieldGetByName(saFieldPtr fieldArray[], char *name)
{
    bool found = false;
    int i = 0;

    while(found == false && i<SAFIELD_MAX)
    {
        if (!strcmp(fieldArray[i]->name, name))
            found = true;
    }
    if (found == true)
        return(fieldArray[i]->fieldPtr);
    return(NULL);
}

bool saFieldIncrementAll(saFieldPtr fieldArray[], char *outputClock)
{
    int dayHour[2];
    int i;

    // get the hour and day of the "current" event so we know which field to write
    i=0;
    int tIndex = -1;
    while(tIndex == -1 && i<SAFIELD_MAX)
    {
        if (fieldArray[i] != NULL)
            if (!strcmp(outputClock, fieldArray[i]->name))
                tIndex = i;
        i++;
    }
    if (tIndex == -1)
    {
        fprintf(stderr, "Clock field '%s' does not exist, exiting...\n", outputClock);
        return(false);
    }
    saTimeFieldGetDayHour(((saTimeFieldPtr)fieldArray[tIndex]->fieldPtr), dayHour);

    for(i=0; i<SAFIELD_MAX; i++)
    {
        if (fieldArray[i] != NULL)
        {
            if (fieldArray[i]->valid[dayHour[0]][dayHour[1]] == 1)
            {
                if (!strcmp(fieldArray[i]->type, SAFIELD_DOUBLE))
                    saDoubleFieldIncrement((saDoubleFieldPtr)fieldArray[i]->fieldPtr);
                else if (!strcmp(fieldArray[i]->type, SAFIELD_FLOAT))
                    saFloatFieldIncrement((saFloatFieldPtr)fieldArray[i]->fieldPtr);
                else if (!strcmp(fieldArray[i]->type, SAFIELD_INT))
                    saIntFieldIncrement((saIntFieldPtr)fieldArray[i]->fieldPtr);
                else if (!strcmp(fieldArray[i]->type, SAFIELD_STRING))
                    saStringFieldIncrement((saStringFieldPtr)fieldArray[i]->fieldPtr);
                else if (!strcmp(fieldArray[i]->type, SAFIELD_TIMESTAMP))
                    saTimeFieldIncrement((saTimeFieldPtr)fieldArray[i]->fieldPtr);
            }
        }
    }
    return(true);
}

saFieldPtr *saFieldInitArray()
{
    return((saFieldPtr *)malloc(sizeof(saFieldPtr)*SAFIELD_MAX));
}

bool saFieldLoad(saPropertiesPtr p, saFieldPtr fieldArray[])
{
    int i;
    for(i=0; i<SAFIELD_MAX; i++)
    {
        char prefix[128];
        sprintf(prefix, "field.%d", i);
        int k = saPropertiesLookup(p, prefix);
        if (k >= 0)
        {
            fieldArray[i] = (saFieldPtr)malloc(sizeof(saFieldType));
            while(k >= 0)
            {
                char *fieldStr = saPropertiesGetKey(p, k);
                char *suffix = ".name";
                if (!strcmp(fieldStr+strlen(fieldStr)-strlen(suffix), suffix))
                    fieldArray[i]->name = saPropertiesGetValue(p, k);
                else
                {
                    suffix = ".type";
                    if (!strcmp(fieldStr+strlen(fieldStr)-strlen(suffix), suffix))
                        fieldArray[i]->type = saPropertiesGetValue(p, k);
                }
                k = saPropertiesLookupNext(p, prefix, k+1);
            }
        }
        else
            fieldArray[i] = NULL;
    }
    return(true);
}

void setValid(saFieldPtr p, int value)
{
    int i, j;
    for(i=0; i<7; i++)
        for(j=0; j<24; j++)
            p->valid[i][j] = value;
}

bool saFieldSetValid(saFieldPtr p, char *validStr)
{
    bool found;
    char *days;
    char *hours;
    char str[1024];
    int daySeries[7];
    int hourSeries[24];

    int i, j;

    setValid(p, 0);
    if (validStr == NULL || *validStr == '\0')
        return(false);

    if (!strcmp(validStr, SAFIELD_VALID_ALL))
    {
        setValid(p, 1);
        return(true);
    }

    strcpy(str, validStr);
    char *v = str;
    days = strsep(&v, ";");
    hours = strsep(&v, ";");

    for(i=0; i<7; i++)
        daySeries[i] = 0;
    found = false;
    if (days != NULL && days[0] != '\0')
    {
        char *d;
        while((d = strsep(&days, ",")) != NULL)
        {
            daySeries[atoi(d)] = 1;
            found = true;
        }
    }
    if (found == false)
        for(i=0; i<7; i++)
            daySeries[i] = 1;

    for(i=0; i<24; i++)
        hourSeries[i] = 0;
    found = false;
    if (hours != NULL)
    {
        char *h;
        while((h = strsep(&hours, ",")) != NULL)
        {
            hourSeries[atoi(h)] = 1;
            found = true;
        }
    }
    if (found == false)
        for(i=0; i<24; i++)
            hourSeries[i] = 1;

    for(i=0; i<7; i++)
        for(j=0; j<24; j++)
            if (daySeries[i] == 1 && hourSeries[j] == 1)
                p->valid[i][j] = 1;

    return(true);
}

int saFieldVarianceLong(long increment, float variance)
{
    if (variance == (float)0.0)
        return(0);

    float r = (float)(random() % 100) / 100.0;

    if (r > variance)
        return(0);

    r = r - variance/2.0;

    int v = (r * increment);
    return(v);
}    
