/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "saDice.h"
#include "saIntField.h"

extern long saDiceRoll(saDicePtr);

long saIntFieldGetValue(saIntFieldPtr p)
{
    if (p->dice != NULL)
        p->value = saDiceRoll(p->dice);
    return(p->value);
}

bool saIntFieldIncrement(saIntFieldPtr p)
{
    if (p->dice == NULL)
        p->value += p->increment;
    return(true);
}
