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

saContextTypePtr saContextCreateSemanticTerms(char *name, double domainMin, double domainMax, 
                                              double avg, double sdev, double count,
                                              double halfTerm, char *terms[], int numTerms,
                                              char *shape, char *endShape, char *notes)
{
    // create the context
    saContextTypePtr p = saContextInit(name, domainMin, domainMax, avg, sdev, count, 
                                       numTerms, SA_CONTEXT_TYPE_DOMAIN, notes);
    
    int i = 0;
    while(i < numTerms)
    {
        double min = domainMin + (halfTerm * i);
        double center = domainMin + (halfTerm * (i + 1));
        double max = domainMin + (halfTerm * (i + 2));
fprintf(stderr, "m=%10.4f x=%10.4f c=%10.4f %d %d\n", min, max, center, numTerms, i);
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
        }
        i++;
    }
    return(p);
}

