/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.      
*/
#ifndef _SA_CSV_H
#define _SA_CSV_H

#include <stdbool.h>

extern int saCSVCompareField(char *, char []);
extern bool saCSVConvertToDouble(char *, double *);
extern char *saCSVExtractField(char *);
extern int saCSVFGetHeaderLine(FILE *, char [], char *[]);
extern int saCSVFGetLine(FILE *, char [], char *[]);
extern int saCSVGetHeaderLine(char [], char *[]);
extern int saCSVGetLine(char [], char *[]);
extern int saCSVParseFieldList(char *[], char *);

#endif
