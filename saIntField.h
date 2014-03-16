/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef _SAINTFIELD
#define _SAINTFIELD true
#include "saDice.h"

typedef struct saIntFieldStruct
{
    saDicePtr dice;
    long init;
    long increment;
    long incrementVariance;
    long value;
} saIntFieldType;

typedef saIntFieldType *saIntFieldPtr;

#endif
