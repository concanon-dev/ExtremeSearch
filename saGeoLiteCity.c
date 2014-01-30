#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "saConstants.h"
#include "saCSV.h"
#include "saGeoLiteCity.h"
#include "saHash.h"

extern saHashtableTypePtr saHashCreate(int size);
extern void *saHashGet(saHashtableTypePtr, char *);
extern void saHashSet(saHashtableTypePtr, char *, char *);

static saHashtableTypePtr cityTable;
static saGeoLiteCityTypePtr zipcodes[100000];
static void *cache;

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
    void *cache = (void *)malloc(sizeof(saGeoLiteCityType)*450000);
    int numHeaderFields;
    bool isComment = true;
    while(isComment == true)
    {
        headerbuf[0] = '\0';
        numHeaderFields = saCSVFGetLine(f, headerbuf, headerList);
        if (headerbuf[0] == '#' || headerbuf[0] == '\0')
            isComment = true;
        else
            isComment = false;
    }
    for(i=0; i<numHeaderFields; i++)
    {
        if (!saCSVCompareField(headerList[i], "city"))
            cityIndex = i;
        else if (!saCSVCompareField(headerList[i], "country"))
            countryIndex = i;
        else if (!saCSVCompareField(headerList[i], "latitude"))
            latIndex = i;
        else if (!saCSVCompareField(headerList[i], "longitude"))
            lonIndex = i;
        else if (!saCSVCompareField(headerList[i], "region"))
            regionIndex = i;
        else if (!saCSVCompareField(headerList[i], "postalCode"))
            zipcodeIndex = i;
    }
    if (cityIndex == -1 || countryIndex == -1 || latIndex == -1 || lonIndex == -1
        || regionIndex == -1 || zipcodeIndex == -1)
    {
        fclose(f);
        return(false);
    }

    saGeoLiteCityTypePtr p = cache;
    bool done = false;
    while(!done)
    {
        int zzz = saCSVFGetLine(f, inbuf, fieldList);
        if (!feof(f))
        {
            strcpy(p->city, fieldList[cityIndex]);
            strcpy(p->country, fieldList[countryIndex]);
            strcpy(p->region, fieldList[regionIndex]);
            strcpy(p->zipcode, fieldList[zipcodeIndex]);
            if (saCSVConvertToDouble(fieldList[latIndex], &(p->lat)) == false)
                p->lat = SA_CONSTANTS_BADLATLON;
            if (saCSVConvertToDouble(fieldList[lonIndex], &(p->lon)) == false)
                p->lon = SA_CONSTANTS_BADLATLON;
            sprintf(tempName, "%s|%s|%s", p->country, p->region, p->city);
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

