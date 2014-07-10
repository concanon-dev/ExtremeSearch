/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.      
*/
#ifndef SA_CSV_H_3
#define SA_CSV_H_3
#include <stdio.h>
#include <sys/types.h>

#define MAXSIZE ((1024 * 1024) - 1)

typedef struct saCSVStruct
{
    FILE *csvFile;
    char buffer[MAXSIZE+1];
    int pointer;
    ssize_t size;
} saCSVType;

typedef saCSVType *saCSVTypePtr;

#endif

#ifndef _CSV3_C
#ifdef __cplusplus
extern "C" {
#endif
extern inline bool saCSVEOF(saCSVTypePtr);
extern inline int saCSV3GetLine(saCSVTypePtr, char [], char *[]);
extern inline void saCSVOpen(saCSVTypePtr, FILE *);
#ifdef __cplusplus
}
#endif
#endif
