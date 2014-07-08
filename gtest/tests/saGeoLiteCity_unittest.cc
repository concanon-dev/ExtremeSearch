// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <iostream>
#include <stdbool.h>
#include "../../saHash.h"
#include "../../saGeoLiteCity.h"
#include "gtest/gtest.h"

extern "C" {
bool convertZipcodeToInt(char *zipcode, int *zipint);
bool saGeoLiteCityLoadTable(char *geoLiteCityFile);
saGeoLiteCityTypePtr saGeoLiteCityGetByZipcode(char *zipcode);
saGeoLiteCityTypePtr saGeoLiteCityGetByCountryRegionCity(char *tempName);
saGeoLiteCityTypePtr *saGeoLiteCityGetZipcodes();
saHashtableTypePtr saGeoLiteCityGetCityTable();
}

TEST(saGeoLitCity, saGeoLiteCityLoadTable) {
    char geoLiteCityFile[256];
    sprintf(geoLiteCityFile, "etc/apps/xtreme/lookups/locations.csv");
    bool loaded = saGeoLiteCityLoadTable(geoLiteCityFile);

    EXPECT_EQ(true, loaded);
}

TEST(saGeoLitCity, convertZipcodeToInt) {
    int zipint;
    char zipcode[256] = "80126";
    bool success = convertZipcodeToInt(zipcode, &zipint);

    EXPECT_EQ(true, success);
    EXPECT_EQ(80126, zipint);
}

TEST(saGeoLitCity, saGeoLiteCityGetByZipCode) {

    char zipcode[256] = "80126";
    saGeoLiteCityTypePtr p = saGeoLiteCityGetByZipcode(zipcode);
    EXPECT_EQ(39.5347, p->lat);
    EXPECT_EQ(-104.9582, p->lon);
}

TEST(saGeoLitCity, saGeoLiteCityGetByCountryRegionCity) {

    char country[256] = "US";
    char region[256] = "CO";
    char city[256] = "Littleton";
    char tempName[1024];
    sprintf(tempName, "%s|%s|%s", country, region, city);
    saGeoLiteCityTypePtr p = saGeoLiteCityGetByCountryRegionCity(tempName);
    EXPECT_EQ(39.7388, p->lat);
    EXPECT_EQ(-104.4083, p->lon);
}

TEST(saGeoLitCity, saGeoLiteCityGetZipcodes) {
    // Not Used
}

TEST(saGeoLitCity, saGeoLiteCityGetCityTable) {
    // Not Used
}

