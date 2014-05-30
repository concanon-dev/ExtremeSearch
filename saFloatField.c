/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "saDice.h"
#include "saFloatField.h"

extern long saDiceRoll(saDicePtr);
 
float saFloatFieldGetValue(saFloatFieldPtr p)
{
    if (p->dice != NULL)
    {
        p->value = (float)saDiceRoll(p->dice);
        int a = (int)(powf(10, p->decimal));
        int r = rand() % a;
        p->value = p->value + (float)r / (float)a;
    }
    return(p->value);
}

bool saFloatFieldIncrement(saFloatFieldPtr p)
{
    if (p->dice == NULL)
        p->value += p->increment;
    return(true);
}

