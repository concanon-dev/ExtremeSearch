/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static char programName[256];

void dumpStack()
{
     void* callstack[128];
     char errFile[512];
     int i, frames = backtrace(callstack, 128);

     char *splunk_home = getenv("SPLUNK_HOME");
     pid_t pid = getpid();
     sprintf(errFile, "%s/var/log/xtreme/", splunk_home);

#ifdef _UNICODE
     mkdir(errFile);
#else
     mkdir(errFile, 0755);
#endif

     sprintf(errFile, "%s/var/log/xtreme/crash_%d.log", splunk_home, pid);
     FILE *f = fopen(errFile, "w");
     if (f != NULL)
     {
         backtrace_symbols_fd(callstack, frames, fileno(f));
         fclose(f);
     }
     else
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

