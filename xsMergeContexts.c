#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "saContext.h"

extern saContextTypePtr saContextLoad(FILE *);
extern saContextTypePtr saContextMerge(saContextTypePtr, saContextTypePtr, char *);
extern bool saContextSave(FILE *, saContextTypePtr);

int main(int argc, char *argv[])
{
    FILE *f1 = fopen("./f1.context", "r");
    saContextTypePtr c1 = saContextLoad(f1);
    if (c1 == NULL)
    {
        fprintf(stderr, "c1 == null\n");
        exit(0);
    }
    fclose(f1);

    FILE *f2 = fopen("./f2.context", "r");
    saContextTypePtr c2 = saContextLoad(f1);
    if (c2 == NULL)
    {
        fprintf(stderr, "c2 == null\n");
        exit(0);
    }
    fclose(f2);

    saContextTypePtr c3 = saContextMerge(c1, c2, "f3");
    if (c3 == NULL)
    {
        fprintf(stderr, "c3 == null\n");
        exit(0);
    }
    FILE *f3 = fopen("./f3.context", "w");
    if (saContextSave(f3, c3) == false)
        fprintf(stderr, "c3 save failed\n");
    fclose(f3);

    exit(0);
}
