/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "saContext.h"

saContextTypePtr saContextCreateConcepts(char *, double, double, double, double, double, 
                                              double, char *[], int, char *, char *, char *, char *);

extern saContextTypePtr saContextInit(char *, double, double, double, double, int, int, char *, 
                                      char *, char *);
extern saConceptTypePtr saConceptCreateCurveDecrease(char *, double, double, double,
                                                               double, double);
extern saConceptTypePtr saConceptCreateCurveIncrease(char *, double, double, double, 
                                                               double, double);
extern saConceptTypePtr saConceptCreateLinearDecrease(char *, double, double, double,
                                                                double);
extern saConceptTypePtr saConceptCreateLinearIncrease(char *, double, double, double,
                                                                double);
extern saConceptTypePtr saConceptCreatePI(char *, double, double, double, double, double);
extern saConceptTypePtr saConceptCreateTrapezoid(char *, double, double, double, double);
extern saConceptTypePtr saConceptCreateTriangle(char *, double, double, double, double,
                                                          double);


saContextTypePtr saContextCreateAvgCentered(char *name, double avg, double sdev, char *terms[],
                                            int numTerms, char *shape, char *endShape, int count,
                                            char *notes, char *uom) 
{
    if (numTerms > SA_CONTEXT_MAXTERMS)
        return(NULL);

    // calculate the necessary sizes
    double domainSize = sdev * (numTerms + 1);
    double domainMin = avg - (domainSize / 2); 
    double domainMax = avg + (domainSize / 2); 
    double halfTerm = (SA_CONCEPT_DEFAULT_SDEV_SIZE * sdev) / 2;

    return(saContextCreateConcepts(name, domainMin, domainMax, avg, sdev, count, halfTerm,
                                        terms, numTerms, shape, endShape, notes, uom));
}

saContextTypePtr saContextCreateDomain(char *name, double domainMin, double domainMax, 
                                       char *terms[], int numTerms, char *shape, char *endShape,
                                       int count, char *notes, char *uom) 
{
    if (numTerms > SA_CONTEXT_MAXTERMS)
        return(NULL);

    // calculate the necessary sizes
    double halfTerm = (domainMax - domainMin) / (numTerms + 1);
    return(saContextCreateConcepts(name, domainMin, domainMax, (float)0.0, (float)0.0,
                                        count, halfTerm, terms, numTerms, shape, endShape, notes, uom));
}

void saContextCreateConcept(saContextTypePtr cPtr, char *termName, char *shape, double min, 
                                 double max)
{
    int i = cPtr->numConcepts++;

    if (!strcmp(shape, SA_CONCEPT_SHAPE_CURVEDECREASE))
        cPtr->concepts[i] = saConceptCreateCurveDecrease(termName, cPtr->domainMin, 
                                                                   cPtr->domainMax, min, max,
                                                                   (max+min)/2);
    else if (!strcmp(shape, SA_CONCEPT_SHAPE_LINEARDECREASE))
        cPtr->concepts[i] = saConceptCreateLinearDecrease(termName, cPtr->domainMin, 
                                                                    cPtr->domainMax, min, max);
    else if (!strcmp(shape, SA_CONCEPT_SHAPE_CURVEINCREASE))
        cPtr->concepts[i] = saConceptCreateCurveIncrease(termName, cPtr->domainMin,
                                                                   cPtr->domainMax, min, max, 
                                                                   (max+min)/2);
    else if (!strcmp(shape, SA_CONCEPT_SHAPE_LINEARINCREASE))
        cPtr->concepts[i] = saConceptCreateLinearIncrease(termName, cPtr->domainMin, 
                                                                    cPtr->domainMax, min, max); 
    else if (!strcmp(shape, SA_CONCEPT_SHAPE_TRAPEZOID))
        cPtr->concepts[i] = saConceptCreateTrapezoid(termName, cPtr->domainMin, 
                                                               cPtr->domainMax, min, max);
    else if (!strcmp(shape, SA_CONCEPT_SHAPE_TRIANGLE))
        cPtr->concepts[i] = saConceptCreateTriangle(termName, cPtr->domainMin, 
                                                              cPtr->domainMax, min, max,
                                                              (max+min)/2);
    else
        // edault to SA_CONCEPT_SHAPE_PI
        cPtr->concepts[i] = saConceptCreatePI(termName, cPtr->domainMin, cPtr->domainMax,
                                                        min, max, (max+min)/2);
}

saContextTypePtr saContextCreateConcepts(char *name, double domainMin, double domainMax, 
                                              double avg, double sdev, double count,
                                              double halfTerm, char *terms[], int numTerms,
                                              char *shape, char *endShape, char *notes, char *uom)
{
    // create the context
    saContextTypePtr p = saContextInit(name, domainMin, domainMax, avg, sdev, count, 
                                       numTerms, SA_CONTEXT_TYPE_DOMAIN, notes, uom);

    if (numTerms == 1)
    {
        double min = domainMin;
        double center = (domainMin + domainMax) / 2;
        double max = domainMax;
        if (!strcmp(shape, SA_CONCEPT_SHAPE_CURVEINCREASE))
            p->concepts[0] = saConceptCreateCurveIncrease(terms[0], domainMin, 
                                                                    domainMax, min, max, 
                                                                    center);
        else if (!strcmp(shape, SA_CONCEPT_SHAPE_LINEARINCREASE))
            p->concepts[0] = saConceptCreateLinearIncrease(terms[0], domainMin, 
                                                                     domainMax, min, max); 
        else if (!strcmp(shape, SA_CONCEPT_SHAPE_CURVEDECREASE))
            p->concepts[0] = saConceptCreateCurveDecrease(terms[0], domainMin, 
                                                                    domainMax, min, max,
                                                                    center);
        else if (!strcmp(shape, SA_CONCEPT_SHAPE_LINEARDECREASE))
            p->concepts[0] = saConceptCreateLinearDecrease(terms[0], domainMin, 
                                                                     domainMax, min, max);
        else if (!strcmp(shape, SA_CONCEPT_SHAPE_TRAPEZOID))
            p->concepts[0] = saConceptCreateTrapezoid(terms[0], domainMin, domainMax, 
                                                                min, max);
        else if (!strcmp(shape, SA_CONCEPT_SHAPE_TRIANGLE))
            p->concepts[0] = saConceptCreateTriangle(terms[0], domainMin, domainMax, 
                                                               min, max, center);
        else
            // default to SA_CONCEPT_SHAPE_PI
            p->concepts[0] = saConceptCreatePI(terms[0], domainMin, domainMax,
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
            if (!strcmp(endShape, SA_CONCEPT_SHAPE_LINEAR))
                p->concepts[i] = saConceptCreateLinearIncrease(terms[i], domainMin, 
                                                                         domainMax, min, max); 
            else
                // default to SA_CONCEPT_SHAPE_CURVE
                p->concepts[i] = saConceptCreateCurveIncrease(terms[i], domainMin,
                                                                        domainMax, min, max,
                                                                        center);
        }
        else if (i == 0)
        {
            if (!strcmp(endShape, SA_CONCEPT_SHAPE_LINEAR))
                p->concepts[i] = saConceptCreateLinearDecrease(terms[i], domainMin, 
                                                                         domainMax, min, max);
            else
                // default to SA_CONCEPT_SHAPE_CURVE
                p->concepts[i] = saConceptCreateCurveDecrease(terms[i], domainMin,
                                                                        domainMax, min, max,
                                                                        center);
        }
        else
        {
            if (!strcmp(shape, SA_CONCEPT_SHAPE_TRAPEZOID))
                p->concepts[i] = saConceptCreateTrapezoid(terms[i], domainMin, domainMax, 
                                                             min, max);
            else if (!strcmp(shape, SA_CONCEPT_SHAPE_TRIANGLE))
                p->concepts[i] = saConceptCreateTriangle(terms[i], domainMin, domainMax, 
                                                             min, max, center);
            else 
                // default to SA_CONCEPT_SHAPE_PI
                p->concepts[i] = saConceptCreatePI(terms[i], domainMin, domainMax,
                                                             min, max, center);
        }
        i++;
    }
    return(p);
}

void saContextRecreateConcepts(saContextTypePtr cPtr, double min, double max)
{
    if (cPtr->domainMin > min)
        cPtr->domainMin = min;
    if (cPtr->domainMax < max)
        cPtr->domainMax = max;

    int i = 0;
    while(i<cPtr->numConcepts)
    {
        saConceptTypePtr p = cPtr->concepts[i];
        if (!strcmp(p->shape, SA_CONCEPT_SHAPE_CURVEDECREASE))
            cPtr->concepts[i] = saConceptCreateCurveDecrease(p->name, cPtr->domainMin, 
                                                                      cPtr->domainMax, p->points[0],
                                                                      p->points[2], p->points[1]);
        else if (!strcmp(p->shape, SA_CONCEPT_SHAPE_LINEARDECREASE))
            cPtr->concepts[i] = saConceptCreateLinearDecrease(p->name, cPtr->domainMin, 
                                                                      cPtr->domainMax, p->points[0],
                                                                      p->points[1]);
        else if (!strcmp(p->shape, SA_CONCEPT_SHAPE_CURVEINCREASE))
            cPtr->concepts[i] = saConceptCreateCurveIncrease(p->name, cPtr->domainMin,
                                                                      cPtr->domainMax, p->points[0],
                                                                      p->points[2], p->points[1]);
        else if (!strcmp(p->shape, SA_CONCEPT_SHAPE_LINEARINCREASE))
            cPtr->concepts[i] = saConceptCreateLinearIncrease(p->name, cPtr->domainMin, 
                                                                      cPtr->domainMax, p->points[0],
                                                                      p->points[1]);
        else if (!strcmp(p->shape, SA_CONCEPT_SHAPE_TRAPEZOID))
            cPtr->concepts[i] = saConceptCreateTrapezoid(p->name, cPtr->domainMin, 
                                                                   cPtr->domainMax, p->points[0], 
                                                                   p->points[3]);
        else if (!strcmp(p->shape, SA_CONCEPT_SHAPE_TRIANGLE))
            cPtr->concepts[i] = saConceptCreateTriangle(p->name, cPtr->domainMin, 
                                                                   cPtr->domainMax, p->points[0], 
                                                                   p->points[2], p->points[1]);
        else
            // default to SA_CONCEPT_SHAPE_PI
            cPtr->concepts[i] = saConceptCreatePI(p->name, cPtr->domainMin,
                                                            cPtr->domainMax, p->points[0],
                                                            p->points[4], p->points[2]);
        i++;
    }
}

