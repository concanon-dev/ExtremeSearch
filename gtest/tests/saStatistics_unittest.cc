// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
    double lat1 = 39.553617;
    double lon1 = -104.942473;
    double lat2 = 26.314670;
    double lon2 = -80.078123;

    double result = saStatisticsHaversineDistance(lat1, lon1, lat2, lon2);
    EXPECT_EQ(1699, round(result));
}

TEST(saStatistics, saStatisticsMax) {
    double data[100];
    int startIndex = 0;
    int count = 100;
    double max = 0;

    int i;
    for (i=0; i < 100; i++)
        data[i] = i+1;

    bool result = saStatisticsMax(data, startIndex, count, &max);
    EXPECT_EQ(true, result);
    EXPECT_EQ(100, max);
}

TEST(saStatistics, saStatisticsMean) {
    double data[100];
    int startIndex = 0;
    int count = 100;
    double mean = 0;
    double minRange = 0;
    double maxRange = 0;

    int i;
    for (i=0; i < 100; i++)
        data[i] = i+1;

    bool result = saStatisticsMean(data, startIndex, count, &minRange, &maxRange, &mean);
    EXPECT_EQ(true, result);
    EXPECT_EQ(50.5, mean);
    EXPECT_EQ(1, minRange);
    EXPECT_EQ(100, maxRange);
}

TEST(saStatistics, saStatisticsMedian) {
    double data[100];
    int startIndex = 0;
    int count = 100;
    int medianSize = 0;
    double median = 0;

    int i;
    for (i=0; i < 100; i++)
        data[i] = i+1;

    bool result = saStatisticsMedian(data, startIndex, count, &medianSize, &median);
    EXPECT_EQ(true, result);
    EXPECT_EQ(51.5, median);
    EXPECT_EQ(1, medianSize);
}

TEST(saStatistics, saStatisticsMin) {
    double data[100];
    int startIndex = 0;
    int count = 100;
    double max = 0;

    int i;
    for (i=0; i < 100; i++)
        data[i] = i+1;

    bool result = saStatisticsMin(data, startIndex, count, &max);
    EXPECT_EQ(true, result);
    EXPECT_EQ(1, max);
}

TEST(saStatistics, saStatisticsRange) {
    double data[100];
    int startIndex = 0;
    int count = 100;
    double minRange = 0;
    double maxRange = 0;
    double range = 0;

    int i;
    for (i=0; i < 100; i++)
        data[i] = i+1;

    bool result = saStatisticsRange(data, startIndex, count, &minRange, &maxRange, &range);
    EXPECT_EQ(true, result);
    EXPECT_EQ(99, range);
    EXPECT_EQ(1, minRange);
    EXPECT_EQ(100, maxRange);
}

TEST(saStatistics, saStatisticsSdev) {
    double data[100];
    int startIndex = 0;
    int count = 100;
    double sdev = 0;

    int i;
    for (i=0; i < 100; i++)
        data[i] = i+1;

    bool result = saStatisticsSdev(data, startIndex, count, &sdev);
    EXPECT_EQ(true, result);
    EXPECT_EQ(29, round(sdev));
}

TEST(saStatistics, saStatisticsSkew) {
    double data[100];
    int startIndex = 0;
    int count = 100;
    double skew = 0;

    int i;
    for (i=0; i < 100; i++)
        data[i] = i+1;

    bool result = saStatisticsSkew(data, startIndex, count, &skew);
    EXPECT_EQ(true, result);
    EXPECT_EQ(0, round(skew));
}

TEST(saStatistics, saStatisticsVar) {
    double data[100];
    int startIndex = 0;
    int count = 100;
    double var = 0;

    int i;
    for (i=0; i < 100; i++)
        data[i] = i+1;

    bool result = saStatisticsVar(data, startIndex, count, &var);
    EXPECT_EQ(true, result);
    EXPECT_EQ(833, round(var));
}
