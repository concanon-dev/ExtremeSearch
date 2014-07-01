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

extern bool saCSVEOF(saCSVTypePtr);
extern int saCSV3GetLine(saCSVTypePtr, char [], char *[]);
extern int saCSV3FGetLine(saCSVTypePtr, char [], char *[]);
extern void saCSVOpen(saCSVTypePtr, FILE *);
