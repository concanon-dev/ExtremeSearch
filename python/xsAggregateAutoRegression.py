#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
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
        binary = os.path.dirname(sys.argv[0]) + "/"  + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsAggregateAutoRegression"
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsAggregateAutoRegression-F-000: Can't find binary file " + binary)

        if outfile == '':
            subprocess.call([binary])
        else:
            subprocess.call([binary, '-f', outfile])
        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.)

    except Exception, e:
        si.generateErrorResults(e)
