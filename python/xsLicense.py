#
# (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    key=''

    if len(sys.argv) >1:
        for arg in sys.argv[1:]:
            if not arg.startswith("__"):
                key = arg

    try:
        (isgetinfo, sys.argv) = si.isGetInfo(sys.argv)

        if isgetinfo:
            reqsop = True
            preop = "xsprelicense "
            if key != '':
                preop = preop + " key=" + key 
            si.outputInfo(False, False, False, reqsop, preop) # calls sys.exit()

        binary = platform.system() + "/" + platform.architecture()[0] + "/xsLicense"
        if key == '':
            subprocess.call([binary])
        else:
            subprocess.call([binary, key])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)

