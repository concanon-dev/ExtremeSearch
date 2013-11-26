#ifndef _saQuadRegress
#define _saQuadRegress
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
