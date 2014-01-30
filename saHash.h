/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef __hash
#define __hash true
#define SA_HASHTABLE_DEFAULTSIZE 193
#define SA_HASHTABLE_MAXKEYLEN 256

typedef struct saHashentryTypeStruct {
    char key[SA_HASHTABLE_MAXKEYLEN];
    void *value;
    struct saHashentryTypeStruct *next;
} saHashentryType;

typedef saHashentryType *saHashentryTypePtr;

typedef struct saHashtableTypeStruct {
    int size;
    saHashentryTypePtr *table;     
} saHashtableType;

typedef saHashtableType *saHashtableTypePtr;
#endif
