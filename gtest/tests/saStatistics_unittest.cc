// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "gtest/gtest.h"

extern "C" {
bool saStatisticsAvg(double data[], int startIndex, int count, double *avg);
bool saStatisticsFreq(double data[], int startIndex, int count, int frequency[], int bucketCount, double bucketMin[], double bucketMax[]);
double saStatisticsHaversineDistance(double lat1, double lon1, double lat2, double lon2);
bool saStatisticsMax(double data[], int startIndex, int count, double *max);
bool saStatisticsMean(double data[], int startIndex, int count, double *minRange, double *maxRange, double *mean);
bool saStatisticsMedian(double data[], int startIndex, int count, int *medianSize, double *median);
bool saStatisticsMin(double data[], int startIndex, int count, double *min);
bool saStatisticsRange(double data[], int startIndex, int count, double *minRange, double *maxRange, double *range);
bool saStatisticsSdev(double data[], int startIndex, int count, double *sdev);
bool saStatisticsSkew(double data[], int startIndex, int count, double *skew);
bool saStatisticsVar(double data[], int startIndex, int count, double *var);
}

TEST(saStatistics, saStatisticsAvg) {
    double data[100];
    int startIndex = 0;
    int count = 100; 
    double avg = 0.0;

    int i;
    for (i=0; i < 100; i++)
        data[i] = i+1;

    bool result = saStatisticsAvg(data, startIndex, count, &avg);
    EXPECT_EQ(true, result);
    EXPECT_EQ(50.5, avg);
}

TEST(saStatistics, saStatisticsFreq) {
    double data[200];
    int frequency[100];
    int startIndex = 0;
    int count = 100; 
    int bucketCount = 10; 
    double bucketMin[100];
    double bucketMax[100];


    int i;
    for (i=0; i < 100; i++)
        data[i] = i+1;
    for (i=0; i < 100; i++)
        data[i+100] = i * 2;

    bool result = saStatisticsFreq(data, startIndex, count, frequency, bucketCount, bucketMin, bucketMax);
    EXPECT_EQ(true, result);
    
    //for(i=0; i<100;i++)
    //    printf("i=%d, frequency=%d, bucketMin=%f, bucketMax=%f\n", i, frequency[i], bucketMin[i], bucketMax[i]);
}

TEST(saStatistics, saStatisticsHaversineDistance) {
}

TEST(saStatistics, saStatisticsMax) {
}

TEST(saStatistics, saStatisticsMeang) {
}

TEST(saStatistics, saStatisticsMedian) {
}

TEST(saStatistics, saStatisticsMin) {
}

TEST(saStatistics, saStatisticsRange) {
}

TEST(saStatistics, saStatisticsSdev) {
}

TEST(saStatistics, saStatisticsSkew) {
}

TEST(saStatistics, saStatisticsVar) {
}
