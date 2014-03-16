/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef __SADICE
#define __SADICE true

#define MAXDICE 32
#define MAXWEIGHTS 10
#define SADICE_PRECISION 100

typedef struct saDiceStruct
{
   int numDice;
   int countList[MAXDICE];
   int sidesList[MAXDICE];
   int numWeights[MAXDICE];
   int sumWeights[MAXDICE];
   int weightsList[MAXDICE][MAXWEIGHTS];
   float weightsPercList[MAXDICE][MAXWEIGHTS];
} saDiceType;

typedef saDiceType *saDicePtr;

#endif
