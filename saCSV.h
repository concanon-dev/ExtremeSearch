
#ifndef CSV_H
#define CSV_H

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
