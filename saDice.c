/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "saDice.h"
#include "saUtils.h"

saDicePtr saDiceCreate(char *countAndSides, char *weights)
{
    char *d;
    int i, j, k;

    saDicePtr p = (saDicePtr)malloc(sizeof(saDiceType));
    p->numDice = 0;
    char *t = countAndSides;
    char *u = weights != NULL ? weights : "";
    while ((d = strsep(&t, ",")) != NULL)
    {
        // initialize the weights
        p->numWeights[p->numDice] = 0;

        // get the count  (2*6 is 2 6-sided dice)
        char *d1 = strsep(&d, "*");
        char *d2 = strsep(&d, "*");
        if (d2 == NULL)
        {
            p->countList[p->numDice] = 1;
            p->sidesList[p->numDice] = atoi(d1);
        }
        else
        {
            p->countList[p->numDice] = atoi(d1);
            p->sidesList[p->numDice] = atoi(d2);
        }

        // figure out the weights for each dice set
        // if none specified, then assume weight of 1
        // else parse them as a comma-separated list
        char *w = strsep(&u, ",");
        if (w == NULL)
        {
            p->weightsList[p->numDice][0] = 1;
            p->numWeights[p->numDice] = 1;
            p->sumWeights[p->numDice] = SADICE_PRECISION;
        }
        else
        {
            // determine each weight and the sum of all weights
            j = 0;
            p->sumWeights[p->numDice] = 0;
            char *w2 = strsep(&w, "/");
            while (w2 != NULL)
            {
                int q = atoi(w2);
                p->weightsList[p->numDice][j++] = q;
                p->sumWeights[p->numDice] += q;
                w2 = strsep(&w, "/");
            }
            p->numWeights[p->numDice] = j;

            // determine weight by percent
            p->weightsPercList[p->numDice][0] = (((float)p->weightsList[p->numDice][0] / (float)p->sumWeights[p->numDice])
                                                * (float)SADICE_PRECISION);
            k = 1;
            while (k<j)
            {
                p->weightsPercList[p->numDice][k] = (((float)p->weightsList[p->numDice][k] / (float)p->sumWeights[p->numDice]) 
                                                    * (float)SADICE_PRECISION) 
                                                    + p->weightsPercList[p->numDice][k-1];;
                k++;
            }
        }
        p->numDice++;
    }
    return(p);
}

void saDiceInit()
{
    srand(time(NULL));
}

long saDiceRoll(saDicePtr p)
{
    int i,j,k;
    long result = 0;
    long roll;

    for(i=0; i<p->numDice; i++)
    {
        int diceCount;
        for(diceCount = 0; diceCount<p->countList[i]; diceCount++)
        {
            long percentRoll = rand() % SADICE_PRECISION;

            j = 0;
            k = -1;
            while(k == -1 && j<p->numWeights[i])
                if (p->weightsPercList[i][j] >= percentRoll)
                    k = j;
                else
                    j++;
            int numSides = p->sidesList[i] / p->numWeights[i];
            roll = (rand() % numSides) + 1;

            //fprintf(stderr, "die roll=%ld\n", roll + (k * numSides));

            result += roll + (k * numSides);

            //fprintf(stderr, "result so far=%ld\n", result);
        }
    }
    //fprintf(stderr, "total result=%ld\n", result);
    return(result);
}
