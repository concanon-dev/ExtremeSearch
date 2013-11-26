/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "saContext.h"

extern saSemanticTermTypePtr saSemanticTermCreateLinearDecrease(char *, double, double, double, 
                                                                double);
extern saSemanticTermTypePtr saSemanticTermCreateLinearIncrease(char *, double, double, double, 
                                                                double);
extern saSemanticTermTypePtr saSemanticTermCreatePI(char *, double, double, double, double, double);
extern saSemanticTermTypePtr saSemanticTermCreateSDecrease(char *, double, double,
                                                    double, double, double);
extern saSemanticTermTypePtr saSemanticTermCreateSIncrease(char *, double, double,
                                                    double, double, double);
extern saContextTypePtr saContextCreateDomain(char *, double, double, char *[], int, char *, char *,
                                              int, char *);
extern void saContextDisplay(saContextTypePtr);
extern saContextTypePtr saContextLoad(FILE *);
extern bool saContextSave(FILE *, saContextTypePtr);

int main(int argc, char *argv[])
{
    double dMin = (double)0.0;
    double dMax = (double)255.0;
    double center = (double)128.0;

    char *terms[3];
    int i;
    for(i=0; i<3; i++)
        terms[i] = malloc(256);
    strcpy(terms[0], "low");
    strcpy(terms[1], "medium");
    strcpy(terms[2], "high");
fprintf(stderr, "got here\n");
    saContextTypePtr p = saContextCreateDomain("test", dMin, dMax, terms, 3, "PI", "S",
                                               1000, "hello world");
fprintf(stderr, "numSemanticTerms=%d\n", p->numSemanticTerms);
    int j;
    for(j=0; j<p->numSemanticTerms; j++)
    {
        saSemanticTermTypePtr pp = p->semanticTerms[j];
        for(i=0; i<pp->numPoints; i++)
            fprintf(stderr, "point[%d]=%10.4f\n", i, pp->points[i]);
        fprintf(stderr, "\n");
        for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
            fprintf(stderr, "iv[%d]=%10.4f v[%d]=%10.4f\n", i, pp->indexVector[i], i, 
                    pp->vector[i]);
    }
    FILE *f = fopen("./x.context", "w");
    if (saContextSave(f, p) == false)
        fprintf(stderr, "oops\n");
    else
    {
        fclose(f);
        f = fopen("./x.context", "r");
        saContextTypePtr q = saContextLoad(f);
        if (q == NULL)
            fprintf(stderr, "bad load\n");
        else
            saContextDisplay(q);
    }
}
