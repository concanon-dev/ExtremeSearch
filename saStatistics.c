/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUCKETMAX   127
#define KM_TO_MILES (1.609344)
#define MAX 3e10
#define MIN -3e10
#define R (6372.8)
#define TO_RAD (3.1415926536 / 180)

static void saStatisticsGenerateIntervals(double, double, double, int, double[], double[]);
static void saStatisticsGetInterval(double, double[], double[], int, int *);
static void saStatisticsDataSort(double[], int, int);

bool saStatisticsAvg(double data[], int startIndex, int count, double *avg)
{
    double sum = 0;
    int i;

    if (count <= 0) 
        return(false);

    for (i = startIndex; i < count; i++) 
         sum += data[i];

    *avg = sum / (double)count;
    return(true);
}

bool saStatisticsMin(double data[], int startIndex, int count, double *min) 
{
    int i;
    *min = MAX;

    if (count <= 0)
        return (false);

    for (i=startIndex; i<count; i++)
        if (data[i] < *min) 
            *min = data[i];

    return (true);
}

bool saStatisticsMax(double data[], int startIndex, int count, double *max) 
{
    int i;
    *max = MIN;

    if (count <= 0)
        return(false);

    for (i=0; i<count; i++)
        if (data[i] > *max) 
            *max = data[i];

    return(true);
}

bool saStatisticsMean(double data[], int startIndex, int count, double *minRange, 
                      double *maxRange, double *mean)
{
    double max = MIN;
    double min = MAX;
    double sum = 0;
    int i;

    if (count <= 0)
        return(false);

    for (i=startIndex; i<count; i++) 
    {
        sum += data[i];
        if (data[i] > max) 
            max = data[i];
        if (data[i] < min) 
            min = data[i];
    }
    *minRange = min;
    *maxRange = max;
    *mean = (sum / (double) count);
    return(true);
}

bool saStatisticsRange(double data[], int startIndex, int count, double *minRange, 
                       double *maxRange, double *range)
{
    double max = MIN;
    double min = MAX;
    int i;

    if (count <=0)
        return(false);

    for (i=startIndex; i < count; i++) 
    {
        if (data[i] > max)
            max = data[i];
        if (data[i] < min) 
            min = data[i];
    }
    *minRange = min;
    *maxRange = max;
    *range = max - min;
    return(true);
}

bool saStatisticsVar(double data[], int startIndex, int count, double *var)
{
    double mean;
    double sum = 0.0;
    int i;

    if (saStatisticsAvg(data, startIndex, count, &mean) == false)
        return(false);

    for (i=startIndex; i<count; i++)
        sum += pow((data[i] - mean), 2);
    *var = sum / (double) count;
    return(true);
}

bool saStatisticsSdev(double data[], int startIndex, int count, double *sdev)
{
    double var;

    if (saStatisticsVar(data, startIndex, count, &var) == false)
        return(false);

    *sdev = sqrt(var);
    return (true);
}

bool saStatisticsSkew(double data[], int startIndex, int count, double *skew) 
{
    double avg;
    double diff;
    double sdev;
    double sum = 0.0;
    int i;

    if (count <= 0) 
        return(false);

    if (saStatisticsAvg(data, startIndex, count, &avg) == false)
        return(false);
    if (saStatisticsSdev(data, startIndex, count, &sdev) == false)
        return(false);
    if (sdev == 0.0) 
        return(false);

    for (i=startIndex; i<count; i++) 
    {
        diff = (data[i] - avg) / sdev;
        sum += (diff * diff * diff);
    }
    *skew = sum / (double)count;
    return(true);
}

bool saStatisticsMedian(double data[], int startIndex, int count, int *medianSize, double *median)
{
    int midPoint;

    if (count <= 0)
        return(false);
    *medianSize = 1;
    if (count == 1)
    {
        *median = data[startIndex];
        return(true);
    }
    saStatisticsDataSort(data, startIndex, count);

    if (fmod(count, 2) == 0) 
    {
        midPoint = startIndex + (count / 2);
        *median = (data[midPoint] + data[midPoint + 1]) / 2;
    } 
    else 
    {
        midPoint = startIndex + (count / 2);
        *median = data[midPoint];
    }
    //
    //---------------------------------------------------
    //--Now examine the values to the left and right
    //---------------------------------------------------
    //
    int i;
    for (i=midPoint+1; i<count; i++) 
    {
        if (data[i] == *median) 
            *medianSize += 1;
        else 
            break;
    }
    for (i=midPoint-1; i>=count; i--) 
    {
        if (data[i] == *median) 
            *medianSize += 1;
        else 
            break;
    }
    return(true);
}

void saStatisticsDataSort(double data[], int startIndex, int count)
{
    bool isSorted;
    double temp;
    int i, j;

    if (count > 1)
        for (j=startIndex; j<count; j++) 
        {
            isSorted = true;
            for (i=startIndex; i<count - 1; i++) 
            {
                if (data[i] > data[i + 1]) 
                {
                    temp = data[i + 1];
                    data[i + 1] = data[i];
                    data[i] = temp;
                    isSorted = false;
                }
            }
            if (isSorted) 
                return;
        }
}

bool saStatisticsFreq(double data[], int startIndex, int count, int frequency[], int bucketCount,
                      double bucketMin[], double bucketMax[])
{
    double maxRange;
    double minRange;
    double partitionSize;
    double range;
    int i;
    int index;

    if (count <= 1)
        return(false);
    if (bucketCount > BUCKETMAX) 
        return(false);

    for (i=0; i<bucketCount; i++)
         frequency[i] = 0;
    if (saStatisticsRange(data, startIndex, count, &minRange, &maxRange, &range) == false)
        return(false);

    partitionSize = range / (double)bucketCount;
    for (i=0; i<bucketCount; i++) 
    {
        bucketMin[i] = (double) 0.0;
        bucketMax[i] = (double) 0.0;
    }
    saStatisticsGenerateIntervals(minRange, maxRange, partitionSize, bucketCount, bucketMin, 
                                  bucketMax);
    for (i=startIndex; i<count; i++) 
    {
        saStatisticsGetInterval(data[i], bucketMin, bucketMax, bucketCount, &index);
        frequency[index]++;
    }
    return(true);
}

void saStatisticsGenerateIntervals(double minRange, double maxRange, double partitionSize,
                                   int bucketCount, double bucketMin[], double bucketMax[])
{
    double tempMax;

    int i;
    double tempMin = floor(minRange);
    for (i=0; i<bucketCount; i++) 
    {
        bucketMin[i] = tempMin;
        tempMax = tempMin + partitionSize;
        bucketMax[i] = tempMax;
        tempMin = tempMax;
    }
}

void saStatisticsGetInterval(double data, double minBucket[], double maxBucket[], 
                            int bucketCount, int *interval)
{
    int i;

    for(i=0; i<bucketCount; i++)
        if ((data >= minBucket[i]) && (data < maxBucket[i])) 
        {
            *interval = i;
            return;
        }
    *interval = bucketCount - 1;
}

double saStatisticsHaversineDistance(double lat1, double lon1, double lat2, double lon2)
{
    double dx, dy, dz;
    lon1 = (lon1 - lon2) * TO_RAD;
    lat1 = lat1 * TO_RAD;
    lat2 = lat2 * TO_RAD;

    dz = sin(lat1) - sin(lat2);
    dx = cos(lon1) * cos(lat1) - cos(lat2);
    dy = sin(lon1) * cos(lat1);

    double d = asin(sqrt(dx*dx + dy*dy + dz*dz) / 2) * R * 2;
    d = d / KM_TO_MILES;
    return(d);
}
