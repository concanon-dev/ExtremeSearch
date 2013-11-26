/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <libgen.h>
#include <stdbool.h>
#include <stdlib.h>
#include "saLicensing.h"
#include "saSignal.h"

extern bool writeHeader;

int main(int argc, char* argv[]) 
{
    initSignalHandler(basename(argv[0]));

    writeHeader = true;
    doMain(argc, argv);
    exit(0);
}
