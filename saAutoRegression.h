/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.      
*/
#ifndef _SA_AUTOREGRESS_H
#define _SA_AUTOREGRESS_H

#include "saConstants.h"

#define SA_AUTOREGRESSION_NUMCOEFS 3
#define SA_AUTOREGRESSION_MAXCOEFFICIENTS 3

typedef struct dataRecordStructType
{
    double coef[SA_AUTOREGRESSION_MAXCOEFFICIENTS]; 
    double *evidenceArray;
    double *XArray;
    double *YArray;
    int size;
} dataRecordType;
typedef dataRecordType *dataRecordTypePtr;
#endif
