/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef _SAFLOATFIELD
#define _SAFLOATFIELD true

#include "saDice.h"

typedef struct saFloatFieldStruct
{
    saDicePtr dice;
    int decimal;
    float increment;
    float incrementVariance;
    float init;
    float value;
} saFloatFieldType;

typedef saFloatFieldType *saFloatFieldPtr;

#endif
