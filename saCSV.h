/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.      
*/
#ifndef _SA_CSV_H
#define _SA_CSV_H

#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
extern inline int saCSVCompareField(char *, char []);
extern inline bool saCSVConvertToDouble(char *, double *);
extern inline char *saCSVExtractField(char *);
extern inline int saCSVFGetHeaderLine(FILE *, char [], char *[]);
extern inline int saCSVFGetLine(FILE *, char [], char *[]);
extern inline int saCSVGetHeaderLine(char [], char *[]);
extern inline int saCSVGetLine(char [], char *[]);
extern inline int saCSVParseFieldList(char *[], char *);
#ifdef __cplusplus
}
#endif
#endif
