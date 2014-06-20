/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software
 contains proprietary trade and business secrets.
*/
#ifndef __geolitecity__
#define __geolitecity__

typedef struct saGeoLiteCity
{
    // locId,country,region,city,postalCode,latitude,longitude,metroCode,areaCode
    int locId;
    char country[3];
    char region[3];
    char city[64];
    char zipcode[10];
    double lat;
    double lon;
    int metroCode;
    int areaCode;
} saGeoLiteCityType;
typedef saGeoLiteCityType *saGeoLiteCityTypePtr;

#endif
