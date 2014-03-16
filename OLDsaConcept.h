/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software
 contains proprietary trade and business secrets.
*/
#ifndef __OLDsa
#define __OLDsa

#define MAXFUZZYPARAMETERS 16
#define MAXFUZZYSTRLEN 256
#define MAXFUZZYVECTORSIZE 256

typedef struct OLDsaConceptStruct
{
    char name[MAXFUZZYSTRLEN]; 
    char description[MAXFUZZYSTRLEN]; 
    int type; 
    int setOrder; 
    int numParameters; 
    double domain[2];
    double parameters[MAXFUZZYPARAMETERS];
    float alfacut;
    float indexVector[MAXFUZZYVECTORSIZE]; // indexVector[i] = ((float)i/VECMAX)*(max-min)
    float vector[MAXFUZZYVECTORSIZE]; // The fuzzy set truth vector
} OLDsaConceptType;
typedef OLDsaConceptType *OLDsaConceptTypePtr;

#endif
