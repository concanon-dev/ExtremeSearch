/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef __sasplunk
#define __sasplunk TRUE

#include <libgen.h>

#define SA_SPLUNK_MAXSTR 1024

#define SA_SPLUNK_SCOPE_NONE    0
#define SA_SPLUNK_SCOPE_PRIVATE 1
#define SA_SPLUNK_SCOPE_APP     2
#define SA_SPLUNK_SCOPE_GLOBAL  3

typedef struct saSplunkInfoStruct 
{
    char infoPath[SA_SPLUNK_MAXSTR];
    char app[SA_SPLUNK_MAXSTR];
    char user[SA_SPLUNK_MAXSTR];
} saSplunkInfoType;

typedef saSplunkInfoType *saSplunkInfoPtr;

#endif
