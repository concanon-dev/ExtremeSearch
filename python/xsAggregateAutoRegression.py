#
# (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    outfile=''
    outputKeyword=''

    if len(sys.argv) >1:
        for arg in sys.argv[1:]:
            if arg.lower() == "output":
                outputKeyword="output"
            elif outputKeyword == "output":
                outfile = arg
    else:
        raise Exception("xsAggregateAutoRegression-F-001: Usage: xsAggregateAutoRegression [OUTPUT filename]")

    try:
        binary = platform.system() + "/" + platform.architecture()[0] + "/xsAggregateAutoRegression"
        if outfile == '':
            subprocess.call([binary])
        else:
            outfile = "../lookups/" + outfile + ".csv"
            subprocess.call([binary, '-f', outfile])
        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
