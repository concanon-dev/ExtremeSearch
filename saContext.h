/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef __SACONTEXT
#define	__SACONTEXT

#include "saSemanticTerm.h"

#define SA_CONTEXT_NOTES_SINGLE_TERM "single term"
#define SA_CONTEXT_TIMESTAMP_SEPARATOR "::"
#define SA_CONTEXT_TYPE_AVERAGE_CENTERED "average_centered"
#define SA_CONTEXT_TYPE_DOMAIN "domain"
#define SA_CONTEXT_MAXTERMS 13

typedef struct saContextStruct
{
    double avg;
    int count;
    double domainMax;
    double domainMin;
    char *name;
    char *notes;
    double sdev;
    char *type;
    int numSemanticTerms;
    saSemanticTermTypePtr semanticTerms[SA_CONTEXT_MAXTERMS];
} saContextType;

typedef saContextType *saContextTypePtr;

#endif	

