#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define __DEBUG_C true
#include "saDebug.h"

#define MAXBUF 256

FILE *debug_dataFile = NULL;
FILE *debug_logFile = NULL;
FILE *debug_outputFile = NULL;
char fileNames[3][MAXBUF];
int debug_levels[3];

char *replace_str(char *str, char *orig, char *rep)
{
    static char buffer[4096];
    static char buffer2[4096];
    char *p;

    if (!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
        return(str);
  
    strncpy(buffer, str, p-str); 
    strcpy(buffer+(p-str), rep);
    strcpy(buffer+(p-str)+strlen(rep), str+strlen(orig));
    return(buffer);
}

void saDebugBegin()
{
    FILE *debugFile;
    char debugFileName[MAXBUF];
    char *fieldList[MAXBUF];
    char inbuf[MAXBUF];

    char *splunk_home = getenv("SPLUNK_HOME");
    pid_t pid = getpid();

    int i;
    for(i=0; i<3; i++)
        debug_levels[i] = 0;

    debugFile = NULL;
    debugFile = fopen("../default/debug.txt", "r");
    if (debugFile == NULL)
        return;

    while(!feof(debugFile))
    {
        int numCols = saCSVFGetLine(debugFile, inbuf, fieldList);
        if (*fieldList[0] != '#')
        {
            if (!strcmp(fieldList[0], "data"))
            {
                if (numCols >= 2)
                {
                    debug_levels[DEBUG_DATA] = atoi(fieldList[1]);
                    if (numCols >= 3)
                        sprintf(fileNames[DEBUG_DATA], "%s/debug_data.%d", 
                                replace_str(fieldList[2], "$SPLUNK_HOME", splunk_home), pid);
                }
            }
            else if (!strcmp(fieldList[0], "log"))
            {
                if (numCols >= 2)
                {
                    debug_levels[DEBUG_LOG] = atoi(fieldList[1]);
                    if (numCols >= 3)
                        sprintf(fileNames[DEBUG_LOG], "%s/debug_log.%d",
                                replace_str(fieldList[2], "$SPLUNK_HOME", splunk_home), pid);
                }
            }

            if (!strcmp(fieldList[0], "output"))
            {
                if (numCols >= 2)
                {
                    debug_levels[DEBUG_OUTPUT] = atoi(fieldList[1]);
                    if (numCols >= 3)
                        sprintf(fileNames[DEBUG_OUTPUT], "%s/debug_output.%d",
                                replace_str(fieldList[2], "$SPLUNK_HOME", splunk_home), pid);
                }
            }
        }
    }
    fclose(debugFile);
}

void saDebugCloseFile(int whichFile)
{
    switch (whichFile)
    {
        case DEBUG_DATA: 
            fclose(debug_dataFile);
            break;
        case DEBUG_LOG: 
            fclose(debug_logFile);
            break;
        case DEBUG_OUTPUT: 
            fclose(debug_outputFile);
            break;
        default: return;
    }
    return;
}

saDebugEnd()
{
    if (debug_dataFile != NULL)
        fclose(debug_dataFile);
    if (debug_logFile != NULL)
        fclose(debug_logFile);
    if (debug_outputFile != NULL)
        fclose(debug_outputFile);
}

FILE *saDebugGetFile(int whichFile)
{
    switch (whichFile)
    {
        case DEBUG_DATA: return(debug_dataFile);
        case DEBUG_LOG: return(debug_logFile);
        case DEBUG_OUTPUT: return(debug_outputFile);
        default: return(NULL);
    }
    return(NULL);
}

FILE *saDebugOpenFile(int whichFile)
{
    switch (whichFile)
    {
        case DEBUG_DATA: 
            debug_dataFile = fopen(fileNames[DEBUG_DATA], "a");
            return(debug_dataFile);
            break;
        case DEBUG_LOG:
            debug_logFile = fopen(fileNames[DEBUG_LOG], "a");
            return(debug_logFile);
            break;
        case DEBUG_OUTPUT: 
            debug_outputFile = fopen(fileNames[DEBUG_OUTPUT], "a");
            return(debug_outputFile);
            break;
        default: return(NULL);
    }
    return(NULL);
}

void saDebugOpenFiles()
{
    saDebugOpenFile(DEBUG_DATA);
    saDebugOpenFile(DEBUG_LOG);
    saDebugOpenFile(DEBUG_OUTPUT);
}
