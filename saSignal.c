/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static char programName[256];

void dumpStack()
{
     void* callstack[128];
     int i, frames = backtrace(callstack, 128);
     backtrace_symbols_fd(callstack, frames, 2);
}

void sighandler(int signum)
{
    char signalName[256];
    signalName[0] = '\0';
    if (signum == SIGSEGV)
        strcpy(signalName, "Segmentation Violation");
    fprintf(stderr, "%s-F-000: Received signal (%d) %s\n", programName, signum, signalName);
    dumpStack();
    signal(signum, SIG_DFL);


#ifdef _WIN32
    exit(signum);
#else
    kill(getpid(), signum);
#endif
}

void setProgramName(char *str)
{
    strcpy(programName, str);
}

void initSignalHandler(char *str)
{
    setProgramName(str);
    signal(SIGSEGV, sighandler);
}

