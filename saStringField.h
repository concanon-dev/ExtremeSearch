/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef _SASTRINGFIELD
#define _SASTRINGFIELD true

#define SASTRINGFIELD_MAXVALUE 4096

typedef struct saStringFieldStruct
{
    int field;
    char file[SASTRINGFIELD_MAXVALUE];
    int fileSize;
    char value[SASTRINGFIELD_MAXVALUE];
} saStringFieldType;

typedef saStringFieldType *saStringFieldPtr;

#endif
