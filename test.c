/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdio.h>
#include "saSemanticTerm.h"


extern saSemanticTermTypePtr saSemanticTermCreateLinearDecrease(char *, double, double, double, 
                                                                double);
extern saSemanticTermTypePtr saSemanticTermCreateLinearIncrease(char *, double, double, double, 
                                                                double);
extern saSemanticTermTypePtr saSemanticTermCreatePI(char *, double, double, double, double, double);
extern saSemanticTermTypePtr saSemanticTermCreateSDecrease(char *, double, double,
                                                    double, double, double);
extern saSemanticTermTypePtr saSemanticTermCreateSIncrease(char *, double, double,
                                                    double, double, double);

int main(int argc, char *argv[])
{
    double dMin = (double)0.0;
    double dMax = (double)255.0;
    double tMin = (double)65.0;
    double tMax = (double)191.0;
    double center = (double)128.0;
    saSemanticTermTypePtr p = saSemanticTermCreatePI("hahaha", dMin, dMax, tMin, tMax, center);
    fprintf(stderr, "alfa=%10.4f shape=%s dmin=%10.4f dmax=%10.4f\n", p->alfacut, p->shape, p->domainMin, p->domainMax);
    fprintf(stderr, "num=%d ", p->numPoints);
    int i;
    for(i=0; i<p->numPoints; i++)
        fprintf(stderr, "point[%d]=%10.4f\n", i, p->points[i]);
    fprintf(stderr, "\n");
    for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
        fprintf(stderr, "iv[%d]=%10.4f v[%d]=%10.4f\n", i, p->indexVector[i], i, p->vector[i]);
}
