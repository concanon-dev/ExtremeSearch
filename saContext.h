/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef _SA_CONTEXT_H
#define	_SA_CONTEXT_H

#include "saConcept.h"

#define SA_CONTEXT_DEFAULT_ALFACUT 0.2
#define SA_CONTEXT_FORMAT "v2.0"
#define SA_CONTEXT_NOTES_SINGLE_TERM "single term"
#define SA_CONTEXT_SCALAR_PERCENT 0.05
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
    char *format;
    char *name;
    char *notes;
    double sdev;
    char *type;
    char *uom;
    int numConcepts;
    saConceptTypePtr concepts[SA_CONTEXT_MAXTERMS];
} saContextType;

typedef saContextType *saContextTypePtr;

#endif	

