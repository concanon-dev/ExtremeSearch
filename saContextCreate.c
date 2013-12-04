/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "saContext.h"

saContextTypePtr saContextCreateSemanticTerms(char *, double, double, double, double, double, 
                                              double, char *[], int, char *, char *, char *);

extern saContextTypePtr saContextInit(char *, double, double, double, double, int, int, char *, 
                                      char *);
extern saSemanticTermTypePtr saSemanticTermCreateCurveDecrease(char *, double, double, double,
                                                               double, double);
extern saSemanticTermTypePtr saSemanticTermCreateCurveIncrease(char *, double, double, double, 
                                                               double, double);
extern saSemanticTermTypePtr saSemanticTermCreateLinearDecrease(char *, double, double, double,
                                                                double);
extern saSemanticTermTypePtr saSemanticTermCreateLinearIncrease(char *, double, double, double,
                                                                double);
extern saSemanticTermTypePtr saSemanticTermCreatePI(char *, double, double, double, double, double);
extern saSemanticTermTypePtr saSemanticTermCreateTrapezoid(char *, double, double, double, double);
extern saSemanticTermTypePtr saSemanticTermCreateTriangle(char *, double, double, double, double,
                                                          double);


saContextTypePtr saContextCreateAvgCentered(char *name, double avg, double sdev, char *terms[],
                                            int numTerms, char *shape, char *endShape, int count,
                                            char *notes) 
{
    if (numTerms > SA_CONTEXT_MAXTERMS)
        return(NULL);

    // calculate the necessary sizes
    double domainSize = sdev * (numTerms + 1);
    double domainMin = avg - (domainSize / 2); 
    double domainMax = avg + (domainSize / 2); 
    double halfTerm = (SA_SEMANTICTERM_DEFAULT_SDEV_SIZE * sdev) / 2;

    return(saContextCreateSemanticTerms(name, domainMin, domainMax, avg, sdev, count, halfTerm,
                                        terms, numTerms, shape, endShape, notes));
}

saContextTypePtr saContextCreateDomain(char *name, double domainMin, double domainMax, 
                                       char *terms[], int numTerms, char *shape, char *endShape,
                                       int count, char *notes) 
{
    if (numTerms > SA_CONTEXT_MAXTERMS)
        return(NULL);

    // calculate the necessary sizes
    double halfTerm = (domainMax - domainMin) / (numTerms + 1);
    return(saContextCreateSemanticTerms(name, domainMin, domainMax, (float)0.0, (float)0.0,
                                        count, halfTerm, terms, numTerms, shape, endShape, notes));
}

void saContextCreateSemanticTerm(saContextTypePtr cPtr, char *termName, char *shape, double min, 
                                 double max)
{
    int i = cPtr->numSemanticTerms++;

    if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_CURVEDECREASE))
        cPtr->semanticTerms[i] = saSemanticTermCreateCurveDecrease(termName, cPtr->domainMin, 
                                                                   cPtr->domainMax, min, max,
                                                                   (max+min)/2);
    else if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_LINEARDECREASE))
        cPtr->semanticTerms[i] = saSemanticTermCreateLinearDecrease(termName, cPtr->domainMin, 
                                                                    cPtr->domainMax, min, max);
    else if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_CURVEINCREASE))
        cPtr->semanticTerms[i] = saSemanticTermCreateCurveIncrease(termName, cPtr->domainMin,
                                                                   cPtr->domainMax, min, max, 
                                                                   (max+min)/2);
    else if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_LINEARINCREASE))
        cPtr->semanticTerms[i] = saSemanticTermCreateLinearIncrease(termName, cPtr->domainMin, 
                                                                    cPtr->domainMax, min, max); 
    else if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_PI))
        cPtr->semanticTerms[i] = saSemanticTermCreatePI(termName, cPtr->domainMin, cPtr->domainMax, 
                                                        min, max, (max+min)/2);
    else if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_TRAPEZOID))
        cPtr->semanticTerms[i] = saSemanticTermCreateTrapezoid(termName, cPtr->domainMin, 
                                                               cPtr->domainMax, min, max);
    else if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_TRIANGLE))
        cPtr->semanticTerms[i] = saSemanticTermCreateTriangle(termName, cPtr->domainMin, 
                                                              cPtr->domainMax, min, max,
                                                              (max+min)/2);
    else
        fprintf(stderr, "No Such shape: %s\n", shape);
}

saContextTypePtr saContextCreateSemanticTerms(char *name, double domainMin, double domainMax, 
                                              double avg, double sdev, double count,
                                              double halfTerm, char *terms[], int numTerms,
                                              char *shape, char *endShape, char *notes)
{
    // create the context
    saContextTypePtr p = saContextInit(name, domainMin, domainMax, avg, sdev, count, 
                                       numTerms, SA_CONTEXT_TYPE_DOMAIN, notes);
    if (numTerms == 1)
    {
        double min = domainMin;
        double center = (domainMin + domainMax) / 2;
        double max = domainMax;
        if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_CURVEINCREASE))
            p->semanticTerms[0] = saSemanticTermCreateCurveIncrease(terms[0], domainMin, 
                                                                    domainMax, min, max, 
                                                                    center);
        else if (!strcmp(endShape, SA_SEMANTICTERM_SHAPE_LINEARINCREASE))
            p->semanticTerms[0] = saSemanticTermCreateLinearIncrease(terms[0], domainMin, 
                                                                     domainMax, min, max); 
        else if (!strcmp(endShape, SA_SEMANTICTERM_SHAPE_CURVEDECREASE))
            p->semanticTerms[0] = saSemanticTermCreateCurveDecrease(terms[0], domainMin, 
                                                                    domainMax, min, max,
                                                                    center);
        else if (!strcmp(endShape, SA_SEMANTICTERM_SHAPE_LINEARDECREASE))
            p->semanticTerms[0] = saSemanticTermCreateLinearDecrease(terms[0], domainMin, 
                                                                     domainMax, min, max);
        else if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_PI))
            p->semanticTerms[0] = saSemanticTermCreatePI(terms[0], domainMin, domainMax, 
                                                         min, max, center);
        else if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_TRAPEZOID))
            p->semanticTerms[0] = saSemanticTermCreateTrapezoid(terms[0], domainMin, domainMax, 
                                                                min, max);
        else if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_TRIANGLE))
            p->semanticTerms[0] = saSemanticTermCreateTriangle(terms[0], domainMin, domainMax, 
                                                               min, max, center);
        return(p);
    }

    int i = 0;
    while(i < numTerms)
    {
        double min = domainMin + (halfTerm * i);
        double center = domainMin + (halfTerm * (i + 1));
        double max = domainMin + (halfTerm * (i + 2));
        if (i == numTerms - 1)
        {
            if (!strcmp(endShape, SA_SEMANTICTERM_SHAPE_CURVE))
                p->semanticTerms[i] = saSemanticTermCreateCurveIncrease(terms[i], domainMin, 
                                                                        domainMax, min, max, 
                                                                        center);
            else if (!strcmp(endShape, SA_SEMANTICTERM_SHAPE_LINEAR))
                p->semanticTerms[i] = saSemanticTermCreateLinearIncrease(terms[i], domainMin, 
                                                                         domainMax, min, max); 
        }
        else if (i == 0)
        {
            if (!strcmp(endShape, SA_SEMANTICTERM_SHAPE_CURVE))
                p->semanticTerms[i] = saSemanticTermCreateCurveDecrease(terms[i], domainMin, 
                                                                        domainMax, min, max,
                                                                        center);
            else if (!strcmp(endShape, SA_SEMANTICTERM_SHAPE_LINEAR))
                p->semanticTerms[i] = saSemanticTermCreateLinearDecrease(terms[i], domainMin, 
                                                                         domainMax, min, max);
        }
        else
        {
            if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_PI))
                p->semanticTerms[i] = saSemanticTermCreatePI(terms[i], domainMin, domainMax, 
                                                             min, max, center);
            else if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_TRAPEZOID))
                p->semanticTerms[i] = saSemanticTermCreateTrapezoid(terms[i], domainMin, domainMax, 
                                                             min, max);
            else if (!strcmp(shape, SA_SEMANTICTERM_SHAPE_TRIANGLE))
                p->semanticTerms[i] = saSemanticTermCreateTriangle(terms[i], domainMin, domainMax, 
                                                             min, max, center);
        }
        i++;
    }
    return(p);
}

void saContextRecreateSemanticTerms(saContextTypePtr cPtr, double min, double max)
{
    if (cPtr->domainMin > min)
        cPtr->domainMin = min;
    if (cPtr->domainMax < max)
        cPtr->domainMax = max;

    int i = 0;
    while(i<cPtr->numSemanticTerms)
    {
        saSemanticTermTypePtr p = cPtr->semanticTerms[i];
        if (!strcmp(p->shape, SA_SEMANTICTERM_SHAPE_CURVEDECREASE))
            cPtr->semanticTerms[i] = saSemanticTermCreateCurveDecrease(p->name, cPtr->domainMin, 
                                                                      cPtr->domainMax, p->points[0],
                                                                      p->points[2], p->points[1]);
        else if (!strcmp(p->shape, SA_SEMANTICTERM_SHAPE_LINEARDECREASE))
            cPtr->semanticTerms[i] = saSemanticTermCreateLinearDecrease(p->name, cPtr->domainMin, 
                                                                      cPtr->domainMax, p->points[0],
                                                                      p->points[1]);
        else if (!strcmp(p->shape, SA_SEMANTICTERM_SHAPE_CURVEINCREASE))
            cPtr->semanticTerms[i] = saSemanticTermCreateCurveIncrease(p->name, cPtr->domainMin,
                                                                      cPtr->domainMax, p->points[0],
                                                                      p->points[2], p->points[1]);
        else if (!strcmp(p->shape, SA_SEMANTICTERM_SHAPE_LINEARINCREASE))
            cPtr->semanticTerms[i] = saSemanticTermCreateLinearIncrease(p->name, cPtr->domainMin, 
                                                                      cPtr->domainMax, p->points[0],
                                                                      p->points[1]);
        else if (!strcmp(p->shape, SA_SEMANTICTERM_SHAPE_PI))
            cPtr->semanticTerms[i] = saSemanticTermCreatePI(p->name, cPtr->domainMin,
                                                            cPtr->domainMax, p->points[0], 
                                                            p->points[4], p->points[2]);
        else if (!strcmp(p->shape, SA_SEMANTICTERM_SHAPE_TRAPEZOID))
            cPtr->semanticTerms[i] = saSemanticTermCreateTrapezoid(p->name, cPtr->domainMin, 
                                                                   cPtr->domainMax, p->points[0], 
                                                                   p->points[3]);
        else if (!strcmp(p->shape, SA_SEMANTICTERM_SHAPE_TRIANGLE))
            cPtr->semanticTerms[i] = saSemanticTermCreateTriangle(p->name, cPtr->domainMin, 
                                                                   cPtr->domainMax, p->points[0], 
                                                                   p->points[2], p->points[1]);
        i++;
    }
}

