/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef _SATIMEFIELD
#define _SATIMEFIELD true

#include <time.h>

#define SATIMEFIELD_MAXFORMAT 128

typedef struct saTimeFieldStruct
{
    char *format;
    long init;
    long increment;
    float incrementVariance;
    long value;
    char valueFormatted[SATIMEFIELD_MAXFORMAT];
    int tzoffset;
} saTimeFieldType;

typedef saTimeFieldType *saTimeFieldPtr;

#endif
