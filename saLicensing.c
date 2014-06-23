/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: saLicensing

Description: check to see if this product is licensed.  The commands will cease to operate on 12/1514 @ 10am EST

Functions:
    isLicensed
*/
#include <stdbool.h>
#include <time.h>

// set expire date time to 12/15/14 10:00:00 EST
#define EXPIRETIME ((time_t)1418655600)

bool isLicensed(void) 
{
    time_t now = time(NULL);
    if (now >= EXPIRETIME)
        return(false);
    return(true);
}

