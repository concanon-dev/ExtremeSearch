/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "saSemanticTerm.h"

static saSemanticTermTypePtr saSemanticTermInit(char *, double, double);

void saSemanticTermCopy(saSemanticTermTypePtr newTerm, saSemanticTermTypePtr oldTerm)
{
    newTerm->alfacut = oldTerm->alfacut;
    newTerm->domainMin = oldTerm->domainMin; 
    newTerm->domainMax = oldTerm->domainMax; 
    newTerm->name = malloc(strlen(oldTerm->name)+1);
    strcpy(newTerm->name, oldTerm->name);
    newTerm->numPoints = oldTerm->numPoints;
    newTerm->shape = malloc(strlen(oldTerm->shape)+1);
    newTerm->shape = oldTerm->shape;

    int i;
    for(i=0; i<SA_SEMANTICTERM_MAXPOINTS; i++)
        newTerm->points[i] = oldTerm->points[i];

    for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
    {
        newTerm->indexVector[i] = oldTerm->indexVector[i];
        newTerm->vector[i] = oldTerm->vector[i];
    }
}

saSemanticTermTypePtr saSemanticTermCreateCurveDecrease(char *name, double domainMin, 
                                                        double domainMax, double termMin, 
                                                        double termMax, double inflectionPoint)
{
    // make sure term is not inverted
    if (termMin >= termMax || termMax <= termMin)
        return(NULL);

    // make sure term fits inside context
    if (termMin < domainMin || termMax > domainMax)
        return(NULL);

    // make sure curve is legal
    if (termMin >= inflectionPoint)
        return(NULL);

    // create the term 
    saSemanticTermTypePtr p = saSemanticTermInit(name, domainMin, domainMax);
    p->domainMin = domainMin;
    p->domainMax = domainMax;
    p->numPoints = 3;
    p->points[0] = termMin;
    p->points[1] = inflectionPoint;
    p->points[2] = termMax;
    p->shape = malloc(strlen(SA_SEMANTICTERM_SHAPE_CURVEDECREASE)+1);
    strcpy(p->shape, SA_SEMANTICTERM_SHAPE_CURVEDECREASE);
    double domainSize = domainMax - domainMin;
    double termSize = termMax - termMin;
    double leftSize = (termMax-inflectionPoint) * 2;
    double rightSize = (inflectionPoint - termMin) * 2;
    int i;
    for(i=0;i<SA_SEMANTICTERM_VECTORSIZE;i++)
    {
        double bucketSize = domainMin + (float)i/(SA_SEMANTICTERM_VECTORSIZE) * domainSize;
        if (bucketSize <= termMin)
            p->vector[i] = (float)1.00;
        else if (bucketSize < inflectionPoint)
            p->vector[i] = (float)(1 - (2 * (pow((bucketSize-termMin)/rightSize,2))));
        else if (bucketSize < termMax)
            p->vector[i] = (float)(2 * (pow((bucketSize-termMax)/leftSize,2)));
    }
    return(p);
 }

saSemanticTermTypePtr saSemanticTermCreateCurveIncrease(char *name, double domainMin, 
                                                        double domainMax, double termMin, 
                                                        double termMax, double inflectionPoint)
{
    // make sure term is not inverted
    if (termMin >= termMax || termMax <= termMin)
        return(NULL);

    // make sure term fits inside context
    if (termMin < domainMin || termMax > domainMax)
        return(NULL);

    // make sure curve is legal
    if (termMin >= inflectionPoint)
        return(NULL);

    // create the term 
    saSemanticTermTypePtr p = saSemanticTermInit(name, domainMin, domainMax);
    p->domainMin = domainMin;
    p->domainMax = domainMax;
    p->numPoints = 3;
    p->points[0] = termMin;
    p->points[1] = inflectionPoint;
    p->points[2] = termMax;
    p->shape = malloc(strlen(SA_SEMANTICTERM_SHAPE_CURVEINCREASE)+1);
    strcpy(p->shape, SA_SEMANTICTERM_SHAPE_CURVEINCREASE);
    double domainSize = domainMax - domainMin;
    double termSize = termMax - termMin;
    double leftSize = (termMax-inflectionPoint) * 2;
    double rightSize = (inflectionPoint - termMin) * 2;
    int i;
    for(i=0;i<SA_SEMANTICTERM_VECTORSIZE;i++)
    {
        double bucketSize = domainMin + (float)i/(SA_SEMANTICTERM_VECTORSIZE) * domainSize;
        if (bucketSize >= termMax)
            p->vector[i] = (float)1.00;
        else if (bucketSize > inflectionPoint)
            p->vector[i] = (float)(1 - (2 * (pow((bucketSize-termMax)/rightSize,2))));
        else if (bucketSize > termMin)
            p->vector[i] = (float)(2 * (pow((bucketSize-termMin)/leftSize,2)));
    }
    return(p);
}

saSemanticTermTypePtr saSemanticTermCreateLinearDecrease(char *name, double domainMin, 
                                                         double domainMax, double termMin, 
                                                         double termMax)
{
    // make sure term is not inverted
    if (termMin >= termMax)
        return(NULL);

    // make sure term fits inside context
    if (termMin < domainMin || termMax > domainMax)
        return(NULL);

    double domainSize = domainMax - domainMin;
    double termSize = termMax - termMin;

    // create the term 
    saSemanticTermTypePtr p = saSemanticTermInit(name, domainMin, domainMax);
    p->domainMin = domainMin;
    p->domainMax = domainMax;
    p->numPoints = 2;
    p->points[0] = termMin;
    p->points[1] = termMax;
    p->shape = malloc(strlen(SA_SEMANTICTERM_SHAPE_LINEARDECREASE)+1);
    strcpy(p->shape, "SA_SEMANTICTERM_SHAPE_LINEARDECREASE");

    // set the CIX for each bucket 
    int i;
    for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
    {
        double distanceFromMin = domainMin + (float)i/SA_SEMANTICTERM_VECTORSIZE * domainSize;
        if (distanceFromMin <= termMin)
            p->vector[i] = (float)1.00;
        else if(distanceFromMin < termMax)
            p->vector[i] = (float)1.00 - (float)((distanceFromMin - termMin)/ termSize);
    }
    return(p);
}

saSemanticTermTypePtr saSemanticTermCreateLinearIncrease(char *name, double domainMin, 
                                                         double domainMax, double termMin, 
                                                         double termMax)
{
    // make sure term is not inverted
    if (termMin >= termMax)
        return(NULL);

    // make sure term fits inside context
    if (termMin < domainMin || termMax > domainMax)
        return(NULL);

    double domainSize = domainMax - domainMin;
    double termSize = termMax - termMin;

    // create the term 
    saSemanticTermTypePtr p = saSemanticTermInit(name, domainMin, domainMax);
    p->domainMin = domainMin;
    p->domainMax = domainMax;
    p->numPoints = 2;
    p->points[0] = termMin;
    p->points[1] = termMax;
    p->shape = malloc(strlen(SA_SEMANTICTERM_SHAPE_LINEARINCREASE)+1);
    strcpy(p->shape, SA_SEMANTICTERM_SHAPE_LINEARINCREASE);

    // set the CIX for each bucket 
    int i;
    for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
    {
        double distanceFromMin = domainMin + (float)i/SA_SEMANTICTERM_VECTORSIZE * domainSize;
        if (distanceFromMin > termMax)
            p->vector[i] = (float)1.00;
        else if(distanceFromMin > termMin)
            p->vector[i] = (float)((distanceFromMin - termMin)/ termSize);
    }
    return(p);
}

saSemanticTermTypePtr saSemanticTermCreatePI(char *name, double domainMin, double domainMax, 
                                             double termMin, double termMax, double center)
{
    // make sure term is not inverted
    if (termMin >= termMax || termMax <= termMin)
        return(NULL);

    // make sure term fits inside context
    if (termMin < domainMin || termMax > domainMax)
        return(NULL);

    // make sure center is between min and max
    if (center <= termMin || center >= termMax)
        return(NULL);

    double domainSize = domainMax - domainMin;
    double termSize = termMax - termMin;
    double halfSize = termSize/2;
    double segmentSize = termSize/4;
    double leftInflection = termMin + segmentSize;
    double rightInflection = termMax - segmentSize;
    saSemanticTermTypePtr p = saSemanticTermInit(name, domainMin, domainMax);
    p->shape = malloc(strlen(SA_SEMANTICTERM_SHAPE_PI)+1);
    strcpy(p->shape, SA_SEMANTICTERM_SHAPE_PI);
    p->domainMin = domainMin;
    p->domainMax = domainMax;
    p->numPoints = 5;
    p->points[0] = termMin;
    p->points[1] = leftInflection;
    p->points[2] = center;
    p->points[3] = rightInflection;
    p->points[4] = termMax;
    
    int i;
    for(i=0;i<SA_SEMANTICTERM_VECTORSIZE;i++)
    {
        double bucketSize = domainMin + (float)i/(SA_SEMANTICTERM_VECTORSIZE) * domainSize;
        if (bucketSize >= termMin)
        {
            if (bucketSize < leftInflection)
                p->vector[i] = (float)(2 * (pow((bucketSize-termMin)/halfSize,2)));
            else if (bucketSize < center)
                p->vector[i] = (float)(1 - (2 * (pow((bucketSize-center)/halfSize,2))));
            else if (bucketSize < rightInflection)
                p->vector[i] = (float)(1 - (2 * (pow((bucketSize-center)/halfSize,2))));
            else if (bucketSize < termMax)
                p->vector[i] = (float)(2 * (pow((bucketSize-termMax)/halfSize,2)));
        }
    }
    return(p);
}

saSemanticTermTypePtr saSemanticTermCreateTrapezoid(char *name, double domainMin, double domainMax,
                                             double termMin, double termMax, double center)
{
    int i;

    // make sure term is not inverted
    if (termMin >= termMax || termMax <= termMin)
        return(NULL);

    // make sure term fits inside context
    if (termMin < domainMin || termMax > domainMax)
        return(NULL);

    // make sure center is between min and max
    if (center <= termMin || center >= termMax)
        return(NULL);

    double domainSize = domainMax - domainMin;
    double termSize = termMax - termMin;
    double trapSize = termSize * SA_SEMANTICTERM_TRAPEZOID_SIZE;
    saSemanticTermTypePtr p = saSemanticTermInit(name, domainMin, domainMax);
    p->shape = malloc(strlen(SA_SEMANTICTERM_SHAPE_TRAPEZOID)+1);
    strcpy(p->shape, SA_SEMANTICTERM_SHAPE_TRAPEZOID);
    p->domainMin = domainMin;
    p->domainMax = domainMax;
    p->numPoints = 4;
    p->points[0] = termMin;
    p->points[1] = termMin + trapSize;
    p->points[2] = termMax - trapSize;
    p->points[3] = termMax;

    for(i=0;i<SA_SEMANTICTERM_VECTORSIZE;i++)
    {
        double bucketSize = domainMin + (float)i/(SA_SEMANTICTERM_VECTORSIZE) * domainSize;
        if (bucketSize > termMin)
        {
        if (bucketSize < (termMin + trapSize))
            p->vector[i]=(float)((bucketSize - termMin)/trapSize);
        else if (bucketSize > (termMax - trapSize))
            p->vector[i]=(float)(1 - (bucketSize - (termMax-trapSize))/trapSize);
        else
            p->vector[i]=(float)1.0;
        }
    }
    return(p);
}

saSemanticTermTypePtr saSemanticTermCreateTriangle(char *name, double domainMin, double domainMax,
                                             double termMin, double termMax, double center)
{
    int i;

    // make sure term is not inverted
    if (termMin >= termMax || termMax <= termMin)
        return(NULL);

    // make sure term fits inside context
    if (termMin < domainMin || termMax > domainMax)
        return(NULL);

    // make sure center is between min and max
    if (center <= termMin || center >= termMax)
        return(NULL);

    double domainSize = domainMax - domainMin;
    double termSize = termMax - termMin;
    saSemanticTermTypePtr p = saSemanticTermInit(name, domainMin, domainMax);
    p->shape = malloc(strlen(SA_SEMANTICTERM_SHAPE_TRIANGLE)+1);
    strcpy(p->shape, SA_SEMANTICTERM_SHAPE_TRIANGLE);
    p->domainMin = domainMin;
    p->domainMax = domainMax;
    p->numPoints = 3;
    p->points[0] = termMin;
    p->points[1] = center;
    p->points[2] = termMax;

    for(i=0;i<SA_SEMANTICTERM_VECTORSIZE;i++)
    {
        double bucketSize = domainMin + (float)i/(SA_SEMANTICTERM_VECTORSIZE) * domainSize;
        if (bucketSize > termMin)
        {
            if (bucketSize < center)
                p->vector[i]=(float)((bucketSize - termMin)/(termSize/2));
            else if (bucketSize == center)
                p->vector[i] = (float)1.0;
            else
                p->vector[i]=(float)(1 - (bucketSize - center)/(termSize/2));
        }
    }
    return(0);
}


saSemanticTermTypePtr saSemanticTermInit(char *name, double domainMin, double domainMax)
{
    saSemanticTermTypePtr p = malloc(sizeof(saSemanticTermType));
    if (p == NULL)
        return(NULL);

    memset(p, '\0', sizeof(saSemanticTermType));
    p->name = malloc(strlen(name)+1);
    strcpy(p->name, name);
    p->alfacut = SA_SEMANTICTERM_DEFAULT_ALFACUT;
    int i;
    for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
        p->indexVector[i] = domainMin 
                            + (i * ((domainMax - domainMin + 1) / SA_SEMANTICTERM_VECTORSIZE));
    return(p);
}

