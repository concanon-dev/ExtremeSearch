/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: saListDir

Description:
    Write a list of files contained in a directory, based on a filter, to a FILE stream

Functions:
    saListDir

*/
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline void saListDir(char *dir, char *filter, bool stripFilter, FILE *outfile, char *header)
{
    if (dir == NULL || *dir == '\0')
        return;
    if (filter == NULL)
        filter = "";

    if (header == NULL)
        fprintf(outfile, "file\n");
    else
        fprintf(outfile, "%s\n", header);

    DIR *d = opendir(dir);
    struct dirent *f;
    int filter_len = strlen(filter);
    while((f = readdir(d)) != NULL)
    {
        char *fname = f->d_name;
        if (strlen(fname) >= filter_len)
        {
            if (!filter_len || !strcmp(fname+strlen(fname)-filter_len, filter))
            {
                if (stripFilter == true)
                    fname[strlen(fname)-filter_len] = '\0';
                fprintf(outfile, "%s\n", fname);
            }
        }
    }
    closedir(d);
}
