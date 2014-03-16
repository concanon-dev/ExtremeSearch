/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef _SADOUBLEFIELD
#define _SADOUBLEFIELD true

#include "saDice.h"

typedef struct saDoubleFieldStruct
{
    saDicePtr dice;
    double init;
    double increment;
    double incrementVariance;
    double value;
} saDoubleFieldType;

typedef saDoubleFieldType *saDoubleFieldPtr;

#endif
