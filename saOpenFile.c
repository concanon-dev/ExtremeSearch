/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

FILE *saOpenFile(char *fname, char *mode)
{
    // only check directory of file if you are writing
    if (!strcmp(mode, "w"))
    {
        if (access(dirname(fname), F_OK) != -1)
            return(fopen(fname, mode));
        else
            return(NULL);
    }

    // check the file itself if you are reading or appending
    if (access(fname, F_OK) != -1)
        return(fopen(fname, mode));
    else
        return(NULL);
}
