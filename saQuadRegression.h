/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software
 contains proprietary trade and business secrets.
*/
#ifndef _SA_QUADREGRESSION_H
#define _SA_QUADREGRESSION_H
#define SA_QUADREGRESSION_MAXCOEFFICIENTS 3

typedef struct dataRecordStructType
{
    double coef[SA_QUADREGRESSION_MAXCOEFFICIENTS]; 
    double *evidenceArray;
    double *XArray;
    double *YArray;
    double *ZArray;
    int size;
} dataRecordType;
typedef dataRecordType *dataRecordTypePtr;

#endif
