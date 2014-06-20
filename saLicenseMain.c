/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
 
   Options:
   /activate:filename - Enter Activation Key to activate the license
   /createactrequest:filename - Create Activation Request File  for offline activation
   /setprodkey:ProductKey - adds the product key and attempts to activate online
  
   Without any options it will display the current licensing status
 */
#include <getopt.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "saLicensing.h"
#include "saSignal.h"

//For LimeLM Licensing  
#ifdef _WIN32
    #include <tchar.h>
#else
    #define _T(x) x
    typedef char    TCHAR;
#endif

#include "LimeLM/TurboActivate.h"
#define TA_GUID _T("63742178519400e06b3553.51442890")

HRESULT AddProductKey(TCHAR *pProductKey)
{   
    HRESULT hr;
    char errMsg[320];

    if (pProductKey == NULL)
    {
        fprintf(stderr, "xsprelicense-F-101: Error no Product Key specified.\n");
        return(TA_FAIL);
    }

    hr = CheckAndSavePKey(pProductKey, TA_SYSTEM);
    if (hr != TA_OK) 
    {
        // If unable to add the key as an admin, try as user
        hr = CheckAndSavePKey(pProductKey, TA_USER);
        if (hr != TA_OK)
            return((HRESULT)-1);
    }

    // If the license key was added successfully, try to activate it
    hr = Activate();
    if (hr != TA_OK)
    {
        fprintf(stderr, "xsprelicense-F-105: Can't activate key return_code=%#x\n", hr);
        return(TA_FAIL);
    }

    return(hr);
}


void ActivateLicense(TCHAR *pFilename)
{   
    HRESULT hr;
    TCHAR tchErrMsg;

    // If there's no filename, activate online
    if (pFilename == NULL) {
        hr = Activate();
    }
    else
    {
        hr = ActivateFromFile(pFilename);
    }
}


void CreateLicenseRequestFile(TCHAR *pFilename)
{   
    HRESULT hr;

    if (pFilename == NULL) {
        fprintf(stderr, "xsprelicense-F-104: Error no filename specified.\n");
        return;
    }

    hr = ActivationRequestToFile(pFilename);
}

HRESULT ReactivateLicense(TCHAR *tString)
{
    HRESULT hr = Deactivate((char)0);
    if (hr != TA_OK)
    {
        fprintf(stderr, "xsprelicense-F-105: Faile to deactivate the key, return_code=%#x\n", hr);
        return(hr);
    }
    return(AddProductKey(tString));
}

void trimPipe(TCHAR *str)
{
    TCHAR *ch;

#ifdef _WIN32
    ch = _tcschr(str, '_');
#else
    ch = strchr(str, '_');
#endif

    if (ch != NULL) {
        *ch = '\0';
    }
}

bool writeHeader;

int doMain(int argc, char *argv[])
{
    int c;
    bool argError = false;
    bool verbose = false;
    GENUINE_OPTIONS opts;
    HRESULT hr = TA_OK;
    HRESULT setProdKeyHR = TA_OK;
    char hostname[256];
    char productKey[40];
    TCHAR tString[256];

    while ((c = getopt(argc, argv, "ac:f:r:s:v")) != -1) 
    {
       if (optarg != NULL) {
#ifdef _WIN32
           mbstowcs(tString, &optarg[0], _countof(tString));
#else
           strcpy(tString, &optarg[0]);
#endif

       }
       else
       {
           tString[0] = '\0';
       }

        switch Copyright
        {
            // Activate Online
            case 'a':
                ActivateLicense(NULL);
                break;

            // Create Activation Request File
            case 'c':
                trimPipe(tString);
                CreateLicenseRequestFile(tString);
                break;

            // Activate thru activation file
            case 'f':
                trimPipe(tString);
                ActivateLicense(tString);
                break;

            // Reactivate License with new Product Key
            case 'r':
                trimPipe(tString);
                hr = ReactivateLicense(tString);
                break;

            // Set Product Key
            case 's':
                trimPipe(tString);
                hr = AddProductKey(tString);
                setProdKeyHR = hr;
                break;

            // Set Verbose Flag - displays more info on LimeLM commands
            case 'v':
                verbose = true;
                break;


            // Unknown Option   
            case '?':
                argError = true;
                break;
        }
    }

    if (argError)
    {
        fprintf(stderr, "xsprelicense-F-108: Usage: xsprelicense -a | -c filename | -f filename | -s ProductKey\n");
        fprintf(stderr, "                -a: Activate License Online\n");
        fprintf(stderr, "                -c: Create Activation Request File\n");
        fprintf(stderr, "                -f: Activate License using Activation File\n");
        fprintf(stderr, "                -s: Set Product Key\n");
        exit(0);
    }

    gethostname(hostname, 256);
    if (writeHeader)
        fputs("host,key,status,expiration\n", stdout);

#ifdef _WIN32
    hr = GetPKey(tString, _countof(tString));

    if (hr == TA_OK)
        wcstombs(productKey, tString, sizeof(productKey));

#else
    hr = GetPKey(productKey, sizeof(productKey));
#endif  

    if (hr != TA_OK)
        productKey[0] = '\0';

    opts.nLength = sizeof(GENUINE_OPTIONS);

    opts.nDaysBetweenChecks = 7;
    opts.nGraceDaysOnInetErr = 2;
    opts.flags = TA_SKIP_OFFLINE;

    hr = IsGenuineEx(TA_GUID, &opts);

    if (hr == TA_OK || hr == TA_E_FEATURES_CHANGED || hr == TA_E_INET || hr == TA_E_INET_DELAYED)
    {
        TCHAR tchExpDate[32];
        char ExpirationDate[32];

        // Product Key is valid, check if it has expired
#ifdef _WIN32
        hr = GetFeatureValue(_T("License_Expires"), tchExpDate, _countof(tchExpDate));
#else
        hr = GetFeatureValue(_T("License_Expires"), tchExpDate, sizeof(tchExpDate));
#endif

        if (hr == TA_OK)
            hr = IsDateValid(tchExpDate, TA_HAS_NOT_EXPIRED);

#ifdef _WIN32
        wcstombs(ExpirationDate, tchExpDate, sizeof(ExpirationDate));
#else
        strcpy(ExpirationDate, tchExpDate);
#endif

        if (hr == TA_OK)
        {
            fprintf(stdout, "%s,%s,valid,%s\n", hostname, productKey, ExpirationDate);
            if (setProdKeyHR != TA_OK)
                fprintf(stderr, "xsprelicense-F-103: Current Product Key is valid.  Please use REACTIVATE switch to overwrite key\n");
        }
        else
            fprintf(stdout, "%s,%s,expired,%s\n", hostname, productKey, ExpirationDate);
    }
    else
    {
        uint32_t trialDays = 0;
        HRESULT hrTrial;
        int pKeyLen;
        char pkeyString[40] = "NULL";
        char plicState[32]= "INVALID";

        pKeyLen = strlen(productKey);

        // Display the product key, if it's available
        if (pKeyLen > 0) 
            strcpy(pkeyString, productKey);

        // TA_E_ACTIVATE means the license isn't activated
        if (hr == TA_E_ACTIVATE) 
            strcpy(plicState, "INACTIVE");

        // Check for trial period
        hrTrial = UseTrial(TA_SYSTEM);

        // If unsuccesful, try as user
        if (hrTrial != TA_OK) 
            hrTrial = UseTrial(TA_USER);

        if (hrTrial == TA_OK)
        {
            // Get the number of trial days remaining.
            hrTrial = TrialDaysRemaining(TA_GUID, &trialDays);

            if (hrTrial == TA_OK)
            {
                if (trialDays > 0)
                    fprintf(stdout, "%s,%s,TRIAL,%d Days\n", hostname, pkeyString, trialDays);
                else
                    fprintf(stdout, "%s,%s,%s,NULL\n", hostname, plicState, pkeyString);
            }
        }
        else
        {
            fprintf(stdout, "%s,%s,%#x,NULL\n", hostname, pkeyString, hr);
        }
    }

    fflush(stdout);

    exit(0);
}

