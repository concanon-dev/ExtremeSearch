/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef _SAFIELD
#define _SAFIELD true

#define SAFIELD_MAX 128

#define SAFIELD_DOUBLE "double"
#define SAFIELD_FLOAT "float"
#define SAFIELD_INT "int"
#define SAFIELD_STRING "string"
#define SAFIELD_TIMESTAMP "timestamp"

#define SAFIELD_VALID_ALL ","

typedef struct saFieldStruct
{
    char *name;
    char *type;
    void *fieldPtr;
    int valid[7][24];
} saFieldType;

typedef saFieldType *saFieldPtr;

#endif
