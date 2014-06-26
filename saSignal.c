/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            

Module: saSignal

Description:
    Catch the signals registered in the initialization routine and provide handlers.  Currently, the only
    signal caught/handled is SIGSEGV (Segmentation Violation).  This signal is fatal and we don't want 
    any program to just die ungracefully.  So, we catch it, dump the stack to a safe place, write an
    error message to stderr, the exit gracefully.

Functions:
    dumpStack
    initSignalHandler
    setProgramName
    sigHandler
*/
#ifndef _UNICODE
#include <execinfo.h>
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static char programName[256];

void setProgramName(char *);
void sighandler(int);

void dumpStack()
{
#ifndef _UNICODE
     void* callstack[128];
     char errFile[512];
     int i, frames = backtrace(callstack, 128);

     char *splunk_home = getenv("SPLUNK_HOME");
     pid_t pid = getpid();
     sprintf(errFile, "%s/var/log/xtreme/", splunk_home);

     mkdir(errFile, 0755);

     sprintf(errFile, "%s/var/log/xtreme/crash_%d.log", splunk_home, pid);
     FILE *f = fopen(errFile, "w");
     if (f != NULL)
     {
         backtrace_symbols_fd(callstack, frames, fileno(f));
         fclose(f);
     }
     else
         backtrace_symbols_fd(callstack, frames, 2);
#endif
}

void initSignalHandler(char *str)
{
    setProgramName(str);
    signal(SIGSEGV, sighandler);
}

void setProgramName(char *str)
{
    strcpy(programName, str);
}

void sighandler(int signum)
{
    char signalName[256];
    signalName[0] = '\0';
    if (signum == SIGSEGV)
        strcpy(signalName, "Segmentation Violation");
    fprintf(stderr, "%s-F-000: Received signal (%d) %s\n", programName, signum, signalName);
    dumpStack();

#ifdef _WIN32
    exit(signum);
#endif

    signal(signum, SIG_DFL);
    kill(getpid(), signum);
}
