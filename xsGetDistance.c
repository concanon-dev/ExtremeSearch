#include <libgen.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "saConstants.h"
#include "saCSV.h"
#include "saGeoLiteCity.h"
#include "saHash.h"
#include "saLicensing.h"
#include "saSignal.h"

static void lookupByCityRegionCountry(char *, char *, char *, double []);
static void lookupByZipCode(char *, double *);

static char *fieldList[SA_CONSTANTS_MAXROWSIZE / 31];
static char *headerList[SA_CONSTANTS_MAXROWSIZE / 31];
static char inbuf[SA_CONSTANTS_MAXROWSIZE];

extern bool saGeoLiteCityLoadTable(char *);
extern saGeoLiteCityTypePtr saGeoLiteCityGetByCountryRegionCity(char *);
extern saGeoLiteCityTypePtr saGeoLiteCityGetByZipcode(char *);

extern double saStatisticsHaversineDistance(double, double, double, double);

extern char *strsep(char **, const char *);

extern char *optarg;
extern int optind, optopt;

int main(int argc, char *argv[])
{
    bool argError = false;
    char distanceField[128];
    char fromFieldList[1024];
    char *fromFields[4];
    char geoLiteCityFile[256];
    char lat1Field[128];
    char lon1Field[128];
    char lat2Field[128];
    char lon2Field[128];
    char toFieldList[1024];
    char *toFields[4];
    double latLon1[2];
    double latLon2[2];
    int c;
    int fromFieldIndex[3];
    int numFromFields, numToFields = 0;
    int toFieldIndex[3];

    if (!isLicensed())
        exit(EXIT_FAILURE);

    initSignalHandler(basename(argv[0]));

    lat1Field[0] = '\0';
    lon1Field[0] = '\0';
    lat2Field[0] = '\0';
    lon2Field[0] = '\0';
    strcpy(distanceField, "distance");
    strcpy(geoLiteCityFile, "../lookups/GeoLiteCity-Location.csv");
    while ((c = getopt(argc, argv, "d:g:f:t:")) != -1)
    {
        switch (c)
        {
            case 'd':
                strcpy(distanceField, optarg);
                break;
            case 'f':
                strcpy(fromFieldList, optarg);
                break;
            case 'g':
                strcpy(geoLiteCityFile, optarg);
                break;
            case 't':
                strcpy(toFieldList, optarg);
                break;
            case '?':
                fprintf(stderr, "xsGetDistance-F-101: Unrecognised option: -%c\n", optopt);
                argError = true;
        }
    }
    if (argError)
    {
        fprintf(stderr, "xsGetDistance-F-103: Usage: xsGetDistance [-d distanceField] -f fromFieldList [-g geoLiteCityFile] -t toFieldList\n");
        exit(EXIT_FAILURE);
    }

    char *f = fromFieldList;
    numFromFields = 0;
    while ((fromFields[numFromFields] = (char *)strsep(&f, ",")) != NULL)
            numFromFields++;

    char *t = toFieldList;
    numToFields = 0;
    while ((toFields[numToFields] = (char *)strsep(&t, ",")) != NULL)
            numToFields++;

    if (saGeoLiteCityLoadTable(geoLiteCityFile) == false)
    {
        fprintf(stderr, "xsGetDistance-F-109: Can't load GeoLite City table %s\n", geoLiteCityFile);
        exit(0);
    }

    // Get Header line
    int numHeaderFields = saCSVGetLine(inbuf, headerList);
    int i;
    int foundFrom = 0;
    int foundTo = 0;
    for(i=0; i<numHeaderFields; i++)
    {
        int j, k;
        for(j=0; j<numFromFields; j++)
            if (!saCSVCompareField(headerList[i], fromFields[j]))
            {
                fromFieldIndex[j] = i;
                foundFrom++;
            }
        for(k=0; k<numToFields;k++)
            if (!saCSVCompareField(headerList[i], toFields[k]))
            {
                toFieldIndex[k] = i;
                foundTo++;
            }
    }
    if (foundFrom != numFromFields)
    {
        fprintf(stderr, "xsGetDistance-F-105: one or more of the FROM fields can't be found\n");
        exit(0);
    }   
    if (foundTo != numToFields)
    {
        fprintf(stderr, "xsGetDistance-F-107: one or more of the TO fields can't be found\n");
        exit(0);
    }   

    i = 0;
    int distanceIndex = -1;
    bool found = false;
    while(i<numHeaderFields && !found)
    {
        if (!saCSVCompareField(headerList[i], distanceField))
            found = true;
        else
            i++;
    }
    if (found)
        distanceIndex = i;
    else
        fprintf(stdout, "%s,", distanceField);

    // Write out the header fields separated by ","
    for(i=0; i<numHeaderFields;i++)
    {
        if (i)
        {
            fputs(",", stdout);
        }
        fputs(headerList[i], stdout);
    }
    fputs("\n", stdout);

    bool isLatLon1 = false;
    bool isLatLon2 = false;
    bool done = false;
    while(done == false)
    {
        int fieldCount = saCSVGetLine(inbuf, fieldList);
        if (!feof(stdin))
        {
            double distance = -1;
            latLon1[0] = latLon1[1] = latLon2[0] = latLon2[1] = SA_CONSTANTS_BADLATLON;
            if (numFromFields == 1)
                lookupByZipCode(saCSVExtractField(fieldList[fromFieldIndex[0]]), latLon1);
            else if (numFromFields == 3)
            {
                lookupByCityRegionCountry(saCSVExtractField(fieldList[fromFieldIndex[0]]),
                                         saCSVExtractField(fieldList[fromFieldIndex[1]]),
                                         saCSVExtractField(fieldList[fromFieldIndex[2]]),
                                         latLon1);
            }
            else
            {
                char *f1 = saCSVExtractField(fieldList[fromFieldIndex[0]]);
                char *f2 = saCSVExtractField(fieldList[fromFieldIndex[1]]);
                if (!saCSVConvertToDouble(f1, &(latLon1[0])) || !saCSVConvertToDouble(f2, &(latLon1[1])))
                {
                    if (isLatLon1 == false)
                        lookupByCityRegionCountry(f1, f2, "US", latLon1);
                    else
                        latLon1[0] = latLon1[1] = SA_CONSTANTS_BADLATLON;
                }
                else
                    isLatLon1 = true;
            }

            if (numToFields == 1)
                lookupByZipCode(saCSVExtractField(fieldList[toFieldIndex[0]]), latLon2);
            else if (numToFields == 3)
            {
                lookupByCityRegionCountry(saCSVExtractField(fieldList[toFieldIndex[0]]),
                                         saCSVExtractField(fieldList[toFieldIndex[1]]),
                                         saCSVExtractField(fieldList[toFieldIndex[2]]),
                                         latLon2);
            }
            else
            {
                char *f1 = saCSVExtractField(fieldList[toFieldIndex[0]]);
                char *f2 = saCSVExtractField(fieldList[toFieldIndex[1]]);
                if (!saCSVConvertToDouble(f1, &latLon2[0]) || !saCSVConvertToDouble(f2, &latLon2[0]))
                {
                    if (isLatLon2 == false)
                        lookupByCityRegionCountry(f1, f2, "US", latLon2);
                    else
                        latLon2[0] = latLon2[1] = SA_CONSTANTS_BADLATLON;
                }
                else
                    isLatLon2 = true;
            }

            if (latLon1[0] == SA_CONSTANTS_BADLATLON || latLon1[1] == SA_CONSTANTS_BADLATLON
                || latLon2[0] == SA_CONSTANTS_BADLATLON || latLon2[1] == SA_CONSTANTS_BADLATLON)
                distance = -1.0;
            else
                distance = saStatisticsHaversineDistance(latLon1[0], latLon1[1], latLon2[0],
                                                         latLon2[1]);

            // write out the values of each field
            if (distanceIndex == -1)
                fprintf(stdout, "%.10f,", distance);

            for(i=0; i<numHeaderFields;i++)
            {
                if (i)
                    fputs(",", stdout);
                if (i == distanceIndex)
                    fprintf(stdout, "%.10f", distance);
                else
                    fputs(fieldList[i], stdout);
            }
            fputs("\n", stdout);
        }
        else
            done = true;
    }
}

void lookupByCityRegionCountry(char *city, char *region, char *country, double *latlon)
{
    char tempName[1024];

    sprintf(tempName, "%s|%s|%s", country, region, city);
    saGeoLiteCityTypePtr p = (saGeoLiteCityTypePtr)saGeoLiteCityGetByCountryRegionCity(tempName);
    if (p == NULL)
    {
        latlon[0] = latlon[1] = SA_CONSTANTS_BADLATLON;
    }
    else
    {
        latlon[0] = p->lat;
        latlon[1] = p->lon;
    }
}

void lookupByZipCode(char *zipcode, double *latlon)
{
    saGeoLiteCityTypePtr p = (saGeoLiteCityTypePtr)saGeoLiteCityGetByZipcode(zipcode);
    if (p == NULL)
    {
        latlon[0] = latlon[1] = SA_CONSTANTS_BADLATLON;
    }
    else
    {
        latlon[0] = p->lat;
        latlon[1] = p->lon;
    }
}
