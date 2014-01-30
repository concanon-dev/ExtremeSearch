/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "saCSV.h"
#include "saHedge.h"
#include "saSemanticTerm.h"

static char *fieldList[256];
static char headerBuf[1024];

extern void saSemanticTermCopy(saSemanticTermTypePtr, saSemanticTermTypePtr);

saSemanticTermTypePtr saHedgeApply(int hedge, saSemanticTermTypePtr semanticTerm)
{
    saSemanticTermTypePtr newSemanticTerm = malloc(sizeof(saSemanticTermType));
    double     x, dExpVal = 0.0;
    float      tmpTerm[SA_SEMANTICTERM_VECTORSIZE+1];
    float      tmpA,tmpB;
    int        i,j,k,n;

    double domainSize = semanticTerm->domainMax - semanticTerm->domainMin;
   
    saSemanticTermCopy(newSemanticTerm, semanticTerm);
    switch(hedge)
    {
        case SA_HEDGE_ABOUT:
            if (dExpVal == 0)
                dExpVal = 2;
        case SA_HEDGE_VICINITY:
            if (dExpVal == 0)
                dExpVal = 4;
        case SA_HEDGE_CLOSE:
            if (dExpVal == 0)
                dExpVal = 1.2;
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++) 
                tmpTerm[i] = (float)0.0;
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
            {
                if (semanticTerm->vector[i] != 0)
                {
                    double x = (double)i;
                    for(j=0; j<SA_SEMANTICTERM_VECTORSIZE; j++)
                    {

                        tmpA=(float)(fabs((float)j-(float)x)*domainSize/SA_SEMANTICTERM_VECTORSIZE);
                        tmpB=(float)((1.000/(1+pow(tmpA,dExpVal)))*semanticTerm->vector[i]);
                        tmpTerm[j]=fmaxf(tmpTerm[j],tmpB);
                        //tmpA = fabs((float)j-(float)x) * domainSize/SA_SEMANTICTERM_VECTORSIZE;
                        //tmpB = ((1.000/(1+powf(tmpA,dExpVal)))*semanticTerm->vector[i]);
                        //tmpTerm[j] = fmaxf(tmpTerm[j], tmpB);
                    }
                }
            }
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                newSemanticTerm->vector[i] = tmpTerm[i];
            break;
        //
        //--------------------------------------------------------------
        //--The above hedge creates a fuzzy set that follow the surface
        //--of another fuzzy set. The out come fuzzy has m[x]=0 when the
        //--target fuzzy set has m[x]=1 and has m[x]=1 when the target
        //--has m[x]=0; otherwise the target member is m[x]=1-m.
        //--------------------------------------------------------------
        //
        case SA_HEDGE_ABOVE:
            x = 0.0;
            j = 0;
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                if (semanticTerm->vector[i] >= x)
                {
                    j = i;
                    x = semanticTerm->vector[i];
                    if (x == 1.00)
                        i=SA_SEMANTICTERM_VECTORSIZE;
                }
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
            {
                if (i <= j)
                    newSemanticTerm->vector[i] = (float)0.0;
                else
                    newSemanticTerm->vector[i] = 1 - semanticTerm->vector[i];
            }
            break;
        //
        //--------------------------------------------------------------
        //--Thee positively hedge tightens the width of an approximation
        //--and intensifies the surface of other fuzzy sets.This has the
        //--affect of making the fuzzy membership function slightly less
        //--true toward the edge of the set's domain.
        //--------------------------------------------------------------
        //
        case SA_HEDGE_POSITIVE:
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                if (semanticTerm->vector[i] >= 0.5)
                {
                    newSemanticTerm->vector[i] = (float)2.0*powf(semanticTerm->vector[i],2);
                    if (newSemanticTerm->vector[i] > 1.0)
                        newSemanticTerm->vector[i] = 1.0;
                }
                else
                {
                    newSemanticTerm->vector[i] = (float)1.0-(2.0*(powf(1-semanticTerm->vector[i],2)));
                    if (newSemanticTerm->vector[i] < 0.0)
                        newSemanticTerm->vector[i] = 0.0;
                }
            break;
        //
        //--------------------------------------------------------------
        //--The after hedge becomes more and more true as you move to
        //--the right across the domain of the underlying target semanticTerm.
        //--------------------------------------------------------------
        //
        case SA_HEDGE_AFTER:
            n = 0;
            float m = semanticTerm->vector[0];
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
            {
                if (semanticTerm->vector[i] > m)
                    m = semanticTerm->vector[i];
                if (semanticTerm->vector[i] > 0)
                    if (n == 0)
                        n = i;
            }
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                newSemanticTerm->vector[i] = (float)0.0;
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                if (semanticTerm->vector[i] >= m)
                {
                    for(j=i,k=n+1; j<SA_SEMANTICTERM_VECTORSIZE; j++,k++)
                        newSemanticTerm->vector[j] = semanticTerm->vector[k];
                }
            break;
        //
        //--------------------------------------------------------------
        //--The before hedge becomes more and more true as you move to
        //--the left across the domain of the underlying target semanticTerm.
        //--------------------------------------------------------------
        //
        case SA_HEDGE_BEFORE:
            n = 0;
            m = semanticTerm->vector[i];
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                if (semanticTerm->vector[i] > m)
                    m = semanticTerm->vector[i];
            for(i=SA_SEMANTICTERM_VECTORSIZE-1; i >= 0; i--)
                if (semanticTerm->vector[i] > 0)
                    if (n == 0)
                        n = i;
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                newSemanticTerm->vector[i] = (float)0.0;
            for(i=SA_SEMANTICTERM_VECTORSIZE-1; i >= 0; i--)
                if (semanticTerm->vector[i] >= m)
                    for(j=i,k=n-1; j>=0 && k>=0; j--,k--)
                        if (j >= 0 && k >= 0)
                            newSemanticTerm->vector[j] = semanticTerm->vector[k];
            break;
            //
            //--------------------------------------------------------------
            //--The below hedge is the counterpart of the above hedge and
            //--follows the shape of the target fuzzy set by becoming more
            //--and more true as it dives under the membership function.
            //--------------------------------------------------------------
            //
          case SA_HEDGE_BELOW:
            x = 0.0;
            j = SA_SEMANTICTERM_VECTORSIZE;
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                if (semanticTerm->vector[i] >= x)
                {
                    j = i;
                    x = semanticTerm->vector[i];
                    if (x == 1.00)
                        i=SA_SEMANTICTERM_VECTORSIZE;
                }
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
            {
                if (i >= j)
                    newSemanticTerm->vector[i] = (float)0.0;
                else
                    newSemanticTerm->vector[i] = 1-semanticTerm->vector[i];
            }
            break;
            //
            //--------------------------------------------------------------
            //--The generally hedge is the counterpart of the positively
            //--hedge and dilutes or diffuses the fuzzy set making it a bit
            //--more true toward the edge of its domain.
            //--------------------------------------------------------------
            //
          case SA_HEDGE_GENERALLY:
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                if (semanticTerm->vector[i] >= 0.5)
                    newSemanticTerm->vector[i] = (float)0.8*powf(semanticTerm->vector[i], 0.5);
                else
                    newSemanticTerm->vector[i] = (float)1.0-(0.8*powf(1-semanticTerm->vector[i], 0.5));
            break;
            //
            //--------------------------------------------------------------
            //--The not hedge creates the complement of the fuzzy set. The
            //--negation function has the form: NOT A(i) = 1-m[A(i)]
            //--------------------------------------------------------------
            //
          case SA_HEDGE_NOT:
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                newSemanticTerm->vector[i] = 1 - semanticTerm->vector[i];
            break;
            //
            //--------------------------------------------------------------
            //--The somewhat hedge is the basic dilution hedge and works by
            //--taking the square root of the membership function.
            //--------------------------------------------------------------
            //
          case SA_HEDGE_SOMEWHAT:
            for(i=0;i<SA_SEMANTICTERM_VECTORSIZE; i++)
                newSemanticTerm->vector[i] = (float)powf(semanticTerm->vector[i], 0.5);
            break;
            //
            //--------------------------------------------------------------
            //--The very hedge is the basic intensifier hedge and works by
            //--squaring the membership function.
            //--------------------------------------------------------------
            //
          case SA_HEDGE_VERY:
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                newSemanticTerm->vector[i] = (float)powf(semanticTerm->vector[i], 2.0);
            break;
            //
            //--------------------------------------------------------------
            //--The extremely hedge is a variant of the very hedge and works
            //--by cubing the membership value.
            //--------------------------------------------------------------
            //
          case SA_HEDGE_EXTREMELY:
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                newSemanticTerm->vector[i] = (float)powf(semanticTerm->vector[i], 3.0);
            break;
            //
            //--------------------------------------------------------------
            //--The slightly  hedge is a variant of the somewhat hedge and
            //--works by taking the cube root of the membership function.
            //--------------------------------------------------------------
            //
          case SA_HEDGE_SLIGHTLY:
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                newSemanticTerm->vector[i] = (float)powf(semanticTerm->vector[i], 0.3);
            break;
          case SA_HEDGE__NOISE:
          default:
            for(i=0; i<SA_SEMANTICTERM_VECTORSIZE; i++)
                newSemanticTerm->vector[i] = semanticTerm->vector[i];
            break;
    }
    return(newSemanticTerm);
}

bool saHedgeLoadLookup(FILE *infile, saSynonymTableTypePtr synonymPtr)
{
    int i;
    int numFields;
    int hedgeIndex = -1;
    int wordIndex = -1;

    bool foundHeader = false;
    while(foundHeader == false)
    {
        if (!feof(infile))
        {
            numFields = saCSVFGetLine(infile, headerBuf, fieldList);
            if (*fieldList[0] != '#')
                foundHeader = true;
        }
        else
            return(false);
    }
    if (numFields > 2)
        return(false);

    for(i=0; i<numFields; i++)
    {
        if (!saCSVCompareField(fieldList[i], "hedge"))
            hedgeIndex = i;
        else if (!saCSVCompareField(fieldList[i], "word"))
            wordIndex = i;
    }

    // if no word and hedge columns, then return false
    if (wordIndex == -1 || hedgeIndex == -1)
        return(false);

    int numRows = 0;
    while(!feof(infile))
    {
        char *thisRow = synonymPtr->charbuf + numRows * (SA_HEDGE_MAXWORDSIZE+2) * 2; 
        numFields = saCSVFGetLine(infile, thisRow, fieldList);
        if (!feof(infile))
        {
            if (*fieldList[0] != '#')
            {
                if (numFields == 2)
                {
                    synonymPtr->word[numRows] = fieldList[wordIndex];
                    synonymPtr->hedge[numRows] = fieldList[hedgeIndex];
                    numRows++;
                }
                else
                    return(false);
            }
        }
    }
    synonymPtr->numWords = numRows;
    return(true);
}
 
char *saHedgeLookup(saSynonymTableTypePtr synonymPtr, char *newWord)
{
    bool found = false;

    int i = 0;
    while(i<synonymPtr->numWords && found == false)
    {
        if (!strcmp(synonymPtr->word[i], newWord))
            return(synonymPtr->hedge[i]);
        else
            i++;
    }
    return(newWord);
}
