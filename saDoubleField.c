/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdbool.h>
#include "saDoubleField.h"

double saDoubleFieldGetValue(saDoubleFieldPtr p)
{
    return(p->value);
}

bool saDoubleFieldIncrement(saDoubleFieldPtr p)
{
    p->value += p->increment;
    return(true);
}
