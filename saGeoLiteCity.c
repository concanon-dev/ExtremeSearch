/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software
 contains proprietary trade and business secrets.

Module: saGeoLiteCity

Description: The APIs to lookup information in the GeoLiteCity table.

Functions:
    External:
    convertZipcodeToInt
    saGeoLiteCityGetByCountryRegionCity
    saGeoLiteCityGetByZipcode
    saGeoLiteCityGetCityTable
    saGeoLiteCityGetZipcodes
    saGeoLiteCityLoadTable

    Internal:
    convertZipcodeToInt
*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv3.h"
#include "saConstants.h"
#include "saCSV.h"
#include "saGeoLiteCity.h"
#include "saHash.h"

extern inline saHashtableTypePtr saHashCreate(int size);
extern inline void *saHashGet(saHashtableTypePtr, char *);
extern inline void saHashSet(saHashtableTypePtr, char *, char *);

static saHashtableTypePtr cityTable;
static saGeoLiteCityTypePtr zipcodes[100000];
static saGeoLiteCityType cache[350000];

static saCSVType csv;

bool convertZipcodeToInt(char *zipcode, int *zipint)
{
    char zipDigits[6];

    *zipint = -1;

    if (zipcode == NULL)
        return(NULL);

    if (*zipcode == '"')
        zipcode++;

    int i = 0;
    while(*zipcode != '\0' && *zipcode != '"' && i < 5)
    {
          zipDigits[i++] = *zipcode;
          zipcode++;
    }
    zipDigits[i] = '\0';
    if (i != 5)
        return(false);

    *zipint = atoi(zipDigits);
    return(true);    
}

saHashtableTypePtr saGeoLiteCityGetCityTable()
{
    return(cityTable);
}

saGeoLiteCityTypePtr *saGeoLiteCityGetZipcodes()
{
    return(zipcodes);
}

saGeoLiteCityTypePtr saGeoLiteCityGetByCountryRegionCity(char *tempName)
{
    return((saGeoLiteCityTypePtr)saHashGet(cityTable, tempName));
}

saGeoLiteCityTypePtr saGeoLiteCityGetByZipcode(char *zipcode)
{
    int zipint;

    if (convertZipcodeToInt(zipcode, &zipint) == false)
        return(NULL);
    return(zipcodes[zipint]);
}

bool saGeoLiteCityLoadTable(char *geoLiteCityFile)
{
    char *fieldList[32];
    char headerbuf[2048];
    char *headerList[32];
    char inbuf[2048];
    char tempName[1024];
    int cityIndex, countryIndex, latIndex, lonIndex, regionIndex, zipcodeIndex = -1;
    int zipInt;
    int i;

    for(i=0; i<100000; i++)
        zipcodes[i] = NULL;
    cityTable = saHashCreate(65537);

    FILE *f = fopen(geoLiteCityFile, "r");
    if (f == NULL)
        return(false);
    //void *cache = (void *)malloc(sizeof(saGeoLiteCityType)*350000);
    saCSVOpen(&csv, f);
    int numHeaderFields;
    bool isComment = true;
    while(isComment == true)
    {
        headerbuf[0] = '\0';
        numHeaderFields = saCSV3GetLine(&csv, headerbuf, headerList);
        if (headerbuf[0] == '#' || headerbuf[0] == '\0')
            isComment = true;
        else
            isComment = false;
    }
    for(i=0; i<numHeaderFields; i++)
    {
        if (!saCSVCompareField(headerList[i], "City"))
            cityIndex = i;
        else if (!saCSVCompareField(headerList[i], "CountryCode"))
            countryIndex = i;
        else if (!saCSVCompareField(headerList[i], "Latitude"))
            latIndex = i;
        else if (!saCSVCompareField(headerList[i], "Longitude"))
            lonIndex = i;
        else if (!saCSVCompareField(headerList[i], "Region"))
            regionIndex = i;
        else if (!saCSVCompareField(headerList[i], "PostalCode"))
            zipcodeIndex = i;
    }
    if (cityIndex == -1 || countryIndex == -1 || latIndex == -1 || lonIndex == -1
        || regionIndex == -1 || zipcodeIndex == -1)
    {
        fclose(f);
        return(false);
    }

    saGeoLiteCityTypePtr p = &cache[0];
    bool done = false;
    while(!done)
    {
        int zzz = saCSV3GetLine(&csv, inbuf, fieldList);
        if (saCSVEOF(&csv) == false)
        {
            strcpy(p->city, fieldList[cityIndex]);
            strcpy(p->country, fieldList[countryIndex]);
            strcpy(p->region, fieldList[regionIndex]);
            strcpy(p->zipcode, fieldList[zipcodeIndex]);
            if (saCSVConvertToDouble(fieldList[latIndex], &(p->lat)) == false)
                p->lat = SA_CONSTANTS_BADLATLON;
            if (saCSVConvertToDouble(fieldList[lonIndex], &(p->lon)) == false)
                p->lon = SA_CONSTANTS_BADLATLON;
            sprintf(tempName, "%s|%s|%s", p->country, p->region, 
                    p->city);
            saHashSet(cityTable, tempName, (void *)p);
            if (!strcmp(p->country, "US") || !strcmp(p->country, "PR") 
                || !strcmp(p->country, "VI"))
            {
                int zipint;
                if (convertZipcodeToInt(p->zipcode, &zipint) == true)
                    zipcodes[zipint] = p;
            }
            p++;
        }
        else
            done = true;
    }
    fclose(f);
    return(true);
}

