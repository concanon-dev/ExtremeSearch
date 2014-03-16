/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdio.h>
#include <stdbool.h>

//For LimeLM Licensing  
#ifdef _WIN32
    #include <tchar.h>
#else
    #define _T(x) x
    typedef char    TCHAR;
#endif

#include "LimeLM/TurboActivate.h"
#define TA_GUID _T("63742178519400e06b3553.51442890")

bool isLicensed(void) 
{
    HRESULT hr;
    GENUINE_OPTIONS opts;
    bool licenseOK = false;

    opts.nLength = sizeof(GENUINE_OPTIONS);

    // How often to verify with the LimeLM servers
    opts.nDaysBetweenChecks = 1;

    // The grace period if TurboActivate couldn't connect to the servers.
    opts.nGraceDaysOnInetErr = 1;

    // Support offline activation
    opts.flags = TA_SKIP_OFFLINE;

    hr = IsGenuineEx(TA_GUID, &opts);
    if (hr == TA_OK || hr == TA_E_FEATURES_CHANGED || hr == TA_E_INET || hr == TA_E_INET_DELAYED)
    {
	TCHAR ExpirationDate[32];

	// Product Key is valid, check if it has expired
#ifdef _WIN32
	hr = GetFeatureValue(_T("License_Expires"), ExpirationDate, _countof(ExpirationDate));
#else
	hr = GetFeatureValue(_T("License_Expires"), ExpirationDate, sizeof(ExpirationDate));
#endif

	if (hr == TA_OK)
        {
	    if ((hr = IsDateValid(ExpirationDate, TA_HAS_NOT_EXPIRED)) == TA_OK)
	        licenseOK = true;
            else
            {
                fprintf(stderr, 
                        "License-F-003: There is an issue with your license, please contact Scianta Analytics support (support@sciantaanalytics.com). code == %#x\n", hr);
                return(false);
            }
        }
        else
        {
            fprintf(stderr, "License-F-005: There is an issue with your license, please contact Scianta Analytics support (support@sciantaanalytics.com). code == %#x\n", hr);
            return(false);
        }
    }
    else // not genuine / not activated
    {
        unsigned int daysRemain = 0;

        // Start or re-validate the trial if it has already started.
        if ((hr = UseTrial(TA_SYSTEM)) != TA_OK)
            // If SYSTEM failed, try as USER
            hr = UseTrial(TA_USER);

        if (hr == TA_OK)
        {
            // Get the number of trial days remaining.
            if (((hr = TrialDaysRemaining(TA_GUID,  &daysRemain)) == TA_OK) && (daysRemain > 0))
                licenseOK = true;
            else
            {
                fprintf(stderr, "License-F-007: Your license has expired, please contact Scianta Analytics support (support@sciantaanalytics.com). code == %#x daysRemain == %d\n", hr, daysRemain);
                return(false);
            }
        }
        else
        {
            fprintf(stderr, "License-F-009: Your trial license has expired, please contact Scianta Analytics support (support@sciantaanalytics.com). code == %#x\n", hr);
            return(false);
        }

    }
    return(true);
}

