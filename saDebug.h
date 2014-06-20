/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef __SA_XDEBUG
#define __SA_XDEBUG true

#define DEBUG_DATA 0
#define DEBUG_LOG 1
#define DEBUG_OUTPUT 2

#ifndef __SA_DEBUG_C
extern int debug_levels[];
#endif

#endif
extern void saDebugBegin();
extern void saDebugCloseFile(int);
extern void saDebugEnd();
extern FILE *saDebugGetFile(int);
extern FILE *saDebugGetFile(int);
extern FILE *saDebugOpenFile(int);
extern void saDebugOpenFiles();
