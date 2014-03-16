//-----------------------------------------------------------------
//--(c)2012-2013 Scianta Analytics LLC
//-----------------------------------------------------------------
//
#ifndef __saFuzzyDescriptorBlock
#define __saFuzzyDescriptorBlock

#define MAXFUZZYPARAMETERS 16
#define MAXFUZZYSTRLEN 256
#define MAXFUZZYVECTORSIZE 256

typedef struct OLDsaSemanticTermStruct
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
} OLDsaSemanticTermType;
typedef OLDsaSemanticTermType *OLDsaSemanticTermTypePtr;

#endif
