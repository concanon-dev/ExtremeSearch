#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _SA_INDEX_C
#include "saIndex.h"

/*
   Find an index based on the tuple (x,y,byFields,byValues)
   If it exists, return the index
   else add the index tuple to the list
*/
inline int getIndex(char *x, char *y, char *byF, char *byV)
{
    char tuple[1024];
    int i=0;

    // build the tuple
    sprintf(tuple, "%s,%s,%s,%s", x, y, byF, byV);
    bool found = false;
    while(i<numTuples && !found)
    {
        if (!strcmp(tuple, tupleString[i]))
            found = true;
        else
            i++;
    }
    if (!found)
    {
        tupleString[i] = malloc(strlen(tuple)+1);
        strcpy(tupleString[i], tuple);
        numTuples++;
    }
    return(i);
}
