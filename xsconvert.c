#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "OLDsaConcept.h"
#include "OLDsaContext.h"
#include "saContext.h"

extern OLDsaContextPtr saFuzzyLoadContext(FILE *);
extern bool saContextSave(FILE *, saContextTypePtr p);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: xsconvert oldFile newFile\n");
        fclose(0);
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        fprintf(stderr, "Can't open file %s\n", argv[1]);
        exit(1);
    }
    OLDsaContextPtr oldP = saFuzzyLoadContext(f);
    fclose(f);

    saContextTypePtr p = malloc(sizeof(saContextType));
    p->avg = oldP->avg;
    p->count = oldP->count;
    p->domainMax = oldP->max;
    p->domainMin = oldP->min;
    p->name = oldP->name;
    p->notes = oldP->notes;
    p->sdev = oldP->sdev;
    p->type = oldP->type;
    p->numConcepts = oldP->numConcepts;
    int i;
    for(i=0; i<oldP->numConcepts; i++)
    {
        OLDsaConceptTypePtr oldPP = oldP->concepts[i];
        saConceptTypePtr newP = malloc(sizeof(saConceptType));
        p->concepts[i] = newP;
        newP->name = malloc(strlen(oldPP->name)+1);
        strcpy(newP->name, oldPP->name);
        newP->shape = malloc(256);
        switch (oldPP->type)
        {
            case 1:
               strcpy(newP->shape, "linearincrease");
               break;
            case 2:
               strcpy(newP->shape, "lineardecrease");
               break;
            case 3:
               strcpy(newP->shape, "curveincrease");
               break;
            case 4:
               strcpy(newP->shape, "curvedecrease");
               break;
            case 5:
               strcpy(newP->shape, "pi");
               break;
            case 8:
               strcpy(newP->shape, "triangle");
               break;
            case 9:
               strcpy(newP->shape, "leftshoulder");
               break;
            case 10:
               strcpy(newP->shape, "rightshoulder");
               break;
            case 11:
               strcpy(newP->shape, "trapezoid");
               break;
            case '?':
               fprintf(stderr, "xsconvert-F-101: Unrecognised value: %d\n", oldPP->type);
        }
        newP->domainMin = oldPP->domain[0];
        newP->domainMax = oldPP->domain[1];
        newP->alfacut = oldPP->alfacut;
        int k;
        for(k=0; k<256; k++)
        {
            newP->indexVector[k] = oldPP->indexVector[k];
            newP->vector[k] = oldPP->vector[k];
        }
        newP->numPoints = oldPP->numParameters;
        int j;
        for(j=0; j<oldPP->numParameters; j++)
            newP->points[j] = oldPP->parameters[j];
    }
    FILE *o = fopen(argv[2], "w");
    if (o == NULL)
    {
        fprintf(stderr, "can't open new file: %s\n", argv[2]);
        exit(1);
    }
    if (saContextSave(o, p) == false)
        fprintf(stderr, "couldn't open file %s\n", argv[2]);
    fclose(o);
}
