/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//
#define BUCKETMAX   127
#define KM_TO_MILES (1.609344)
#define R (6372.8)
#define TO_RAD (3.1415926536 / 180)
//
//--------------------------------------------------------
//
void saStatisticsGenerateIntervals(double, double, double, int, double[], double[], int*);
int saStatisticsGetInterval(double, double[], double[], int, int*);
void saStatisticsDataSort(double[], int, int, int*);

double saStatisticsAvg(double data[], int startIndex, int numData, int *status)
{
    double sum = 0;
    int i;

    *status = 0;
    if (numData < 1) 
    {
        *status = 1;
        return ((double) 0);
    }
    for (i = startIndex; i < numData; i++) 
         sum += data[i];
    return (sum / ((double) numData));
}

double saStatisticsMin(double data[], int startIndex, int numData, int *status) 
{
    int i;
    double minVal = 3e10;

    *status = 0;
    if (numData < 1) 
    {
        *status = 1;
        return ((double) 0);
    }
    for (i=startIndex; i<numData; i++)
        if (data[i] < minVal) 
            minVal = data[i];
    return (minVal);
}

double saStatisticsMax(double data[], int startIndex, int numData, int *status) 
{
    int i;
    double maxVal = -3e10;

    *status = 0;
    if (numData < 1) 
    {
        *status = 1;
        return ((double) 0);
    }
    for (i=0; i<numData; i++)
        if (data[i] > maxVal) 
            maxVal = data[i];
    return (maxVal);
}

double saStatisticsMean(double data[], int startIndex, int numData, double *minRange, 
                        double *maxRange, int *status)
/*
 Compute the Mean (Average) of the data vector. We also return    
 the minimum and the maximum values in the array (thus the Range).
*/
{
    int i;
    double maxVal = -3e10;
    double minVal = 3e10;
    double sum = 0;

    *status = 0;
    if (numData < 1) 
    {
        *status = 1;
        return ((double) 0);
    }
    for (i=startIndex; i<numData; i++) 
    {
        sum += data[i];
        if (data[i] > maxVal) 
            maxVal = data[i];
        if (data[i] < minVal) 
            minVal = data[i];
    }
    *minRange = minVal;
    *maxRange = maxVal;
    return (sum / (double) numData);
}

double saStatisticsRange(double data[], int startIndex, int numData, double *dpMinRng, double *dpMaxRng,
                         int *ipStatus)
/*
 We compute the range of the data elements in the data vector.   
 This function also returns the minimum and maximum range. While 
 the information is also returned by the Mean statistics, we have
 this routine to extract just the range and the lower and uppers.
*/
{
    int i;
    double dMinVal,
            dMaxVal;

    dMinVal = 3e10;
    dMaxVal = -3e10;

    *ipStatus = 0;
    if (numData < 1)
    {
        *ipStatus = 1;
        return ((double) 0);
    }
    for (i=startIndex; i < numData; i++) 
    {
        if (data[i] > dMaxVal)
            dMaxVal = data[i];
        if (data[i] < dMinVal) 
            dMinVal = data[i];
    }
    *dpMinRng = dMinVal;
    *dpMaxRng = dMaxVal;
    return (dMaxVal - dMinVal);
}

double saStatisticsVar(double data[], int startIndex, int numData, int *ipStatus)
/*
 This function computes the variance of the data vector. The      
 variance is the square the of the distance from the mean to each
 data point. A variance is the basic part of standard deviation. 
*/
{
    int i;
    double dMean, dVSum;

    *ipStatus = 0;
    dMean = saStatisticsAvg(data, startIndex, numData, ipStatus);
    if (*ipStatus > 0) 
        return ((double) 0);
    dVSum = 0;
    for (i=startIndex; i<numData; i++)
        dVSum += pow((data[i] - dMean), 2);
    return (dVSum / (double) numData);
}

double saStatisticsSdev(double data[], int startIndex, int numData, int *ipStatus)
/* Computes the standard deviation of the data vector */
{
    double dVar;

    dVar = saStatisticsVar(data, startIndex, numData, ipStatus);
    return (sqrt(dVar));
}

double saStatisticsSkew(double data[], int startIndex, int numData, int *ipStatus) 
{
    int i;
    double dAvgVal,
            dDiff,
            dStdDev,
            dTotVal,
            dSkew;

    *ipStatus = 0;
    if (numData < 1) 
    {
        *ipStatus = 1;
        return ((double) 0);
    }
    dAvgVal = saStatisticsAvg(data, startIndex, numData, ipStatus);
    if (*ipStatus > 0) 
        return ((double) 0);
    dStdDev = saStatisticsSdev(data, startIndex, numData, ipStatus);
    if (*ipStatus > 0) 
        return ((double) 0);
    if (dStdDev == 0) 
        return ((double) 0);
    //
    dTotVal = 0;
    for (i=startIndex; i<numData; i++) 
    {
        dDiff = (data[i] - dAvgVal) / dStdDev;
        dTotVal += (dDiff * dDiff * dDiff);
    }
    dSkew = (dTotVal / (double) numData);
    return (dSkew);
}

double saStatisticsMedian(double data[], int startIndex, int numData, int *ipMedSize, int *ipStatus)
/*
 Finds the Median value of the data array. We also look left and   
 right to see if the median is siting on a plateau. If so, we then 
 return the width of the plateau.                                 
*/
{
    double dMedian;
    int i,
            iMidpt,
            iMedWidth;

    *ipStatus = 0;
    if (numData < 1) 
    {
        *ipStatus = 1;
        return ((double) 0);
    }
    if (numData < 2)
        return (data[startIndex]);
    saStatisticsDataSort(data, startIndex, numData, ipStatus);
    if (fmod(numData, 2) == 0) 
    {
        iMidpt = startIndex + (numData / 2);
        dMedian = (data[iMidpt] + data[iMidpt + 1]) / 2;
    } else 
    {
        iMidpt = startIndex + (numData / 2);
        dMedian = data[iMidpt];
    }
    iMedWidth = 1;
    //
    //---------------------------------------------------
    //--Now examine the values to the left and right
    //---------------------------------------------------
    //
    for (i = iMidpt + 1; i <numData; i++) 
    {
        if (data[i] == dMedian) 
            iMedWidth++;
        else 
            break;
    }
    for (i = iMidpt - 1; i >= numData; i--) 
    {
        if (data[i] == dMedian) 
            iMedWidth++;
        else 
            break;
    }
    *ipMedSize = iMedWidth;
    return (dMedian);
}

void saStatisticsDataSort(double data[], int startIndex, int numData, int *ipStatus)
/* BubbleSort */
{
    int i, j;
    double dTempData;
    bool bSorted;

    if (numData < 2) return;
    for (j=startIndex; j<numData; j++) 
    {
        bSorted = true;
        for (i=startIndex; i<numData - 1; i++) 
        {
            if (data[i] > data[i + 1]) 
            {
                dTempData = data[i + 1];
                data[i + 1] = data[i];
                data[i] = dTempData;
                bSorted = false;
            }
        }
        if (bSorted) 
            return;
    }
    return;
}

void saStatisticsFreq(double data[], //--Data Space data vector
        int startIndex, // Index in data to start analysis
        int numData, //--Number of Data elements
        int iFreq[], //--The Frequency Tally Vector
        int iBucketNum, //--Number of Buckets
        double dBucMin[], //--Interval's Minimum threshold
        double dBucMax[], //--Interval's Maximum threshold
        int *ipStatus) //--Status Pointer
/*
 Generate a frequency histogram of the data. We indicate the       
 number of buckets (intervals) across the range. The data is then 
 clustered into the buckets so that we can see how the elements  
 are distributed in the vector.                                 
*/
{
    int i, iBNo;
    double dPartSize,
            dRange,
            dpMinRng,
            dpMaxRng;

    *ipStatus = 0;
    if (numData < 2) 
    {
        *ipStatus = 1;
        return;
    }
    if (iBucketNum > BUCKETMAX) 
    {
        *ipStatus = 1;
        return;
    }
    //
    for (i = 0; i < iBucketNum; i++) iFreq[i] = 0;
    dRange = saStatisticsRange(data, startIndex, numData, &dpMinRng, &dpMaxRng, ipStatus);
    //
    dPartSize = (dRange / (double) iBucketNum);
    //
    for (i = 0; i < iBucketNum; i++) 
    {
        dBucMin[i] = (double) 0;
        dBucMax[i] = (double) 0;
    }
    saStatisticsGenerateIntervals(dpMinRng, dpMaxRng, dPartSize, iBucketNum, dBucMin, 
                        dBucMax, ipStatus);
    if (*ipStatus != 0) 
        return;
    //
    for (i=startIndex; i<numData; i++) 
    {
        iBNo = saStatisticsGetInterval(data[i], dBucMin, dBucMax, iBucketNum, ipStatus);
        iFreq[iBNo]++;
    }
    return;
}

void saStatisticsGenerateIntervals(
        double dpMinRng, //--Minimum Range
        double dpMaxRng, //--Maximum Range
        double dPartSize, //--Size of each Bucket
        int iBucketNum, //--Number of Bucket
        double dBucMin[], //--Minimal Intervals
        double dBucMax[], //--Maximal Intervals
        int *ipStatus) //--Status Pointer
/*
 Given the required number of buckets, this routine partitions     
 the data into a set of lower and upper boundary sets. we start 
 with the minimum value and construct sets of intervals equal to
 the number required by the parameter specificatios (iBuckNum).
*/
{
    int iBNo;
    double dThisMax,
            dThisMin;

    *ipStatus = 0;
    iBNo = 0;
    dThisMin = floor(dpMinRng);
    //
    for (iBNo = 0; iBNo < iBucketNum; iBNo++) 
    {
        dBucMin[iBNo] = dThisMin; //--Set lower bound to Minimum
        dThisMax = dThisMin + dPartSize; //--Max equals min plus partition size
        dBucMax[iBNo] = dThisMax; //--Set upper bound to Maximum
        dThisMin = dThisMax; //--Make next lower equal this maximum
    }
}

int saStatisticsGetInterval(double dDataPt, double dBucMin[], double dBucMax[], 
                            int iBucNum, int *ipStatus)
/*
 Given a value form the data space, we decide where it belongs in 
 the set of bucket intervals.                                    
*/
{
    int i;

    *ipStatus = 0;
    for (i = 0; i < iBucNum; i++)
        if ((dDataPt >= dBucMin[i]) && (dDataPt < dBucMax[i])) 
            return (i);
    return (iBucNum - 1);
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
