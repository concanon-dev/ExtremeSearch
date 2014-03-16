/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef _SAOUTPUT
#define _SAOUTPUT true

#define SAOUTPUT_OUTBUFSIZE 1024*1024

typedef struct saOutputStruct
{
   FILE *f;
   char *outputClock;
   char *outputFile;
   char *outputHeader;
   char *outputTemplate;
} saOutputType;

typedef saOutputType *saOutputPtr;

#endif

