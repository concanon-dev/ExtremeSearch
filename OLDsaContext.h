/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef SAFUZZYTERMSET_H
#define	SAFUZZYTERMSET_H

#define TIMESTAMP_SEPARATOR "::"
#define MAXFUZZYSETS 13

typedef struct OLDsaContext
{
    double avg;
    int count;
    double max;
    double min;
    char *name;
    char *notes;
    double sdev;
    char *type;
    int numConcepts;
    OLDsaConceptTypePtr concepts[MAXFUZZYSETS];
} OLDsaContextType;

typedef OLDsaContextType *OLDsaContextPtr;

#endif	/* SAFUZZYTERMSET_H */

