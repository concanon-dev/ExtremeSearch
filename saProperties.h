/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef __saProperties
#define __saProperties TRUE

#define SAPROPERTIES_MAX 1024
typedef struct saPropertiesStruct
{
   int numFields;
   char *keyList[SAPROPERTIES_MAX];
   char *valueList[SAPROPERTIES_MAX];
} saPropertiesType;

typedef saPropertiesType *saPropertiesPtr;

#define saPropertiesGetKey(p, i) (i >= 0) ? p->keyList[i] : NULL
#define saPropertiesGetValue(p, i) (i >= 0) ? p->valueList[i] : NULL

#endif

