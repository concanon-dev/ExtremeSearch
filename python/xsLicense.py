#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    key=''
    reactivate=''
    if len(sys.argv) >1:
        for arg in sys.argv[1:]:
            if not arg.startswith("__"):
                if arg.lower().startswith("reactivate"):
                    reactivate="true"
                else:
                    key = arg

    try:
        (isgetinfo, sys.argv) = si.isGetInfo(sys.argv)

        if isgetinfo:
            reqsop = True
            preop = "xsprelicense "
            if key != '':
                preop = preop + " key=" + key 
            if reactivate != '':
                preop = preop + " reactivate=true"
            si.outputInfo(False, False, False, reqsop, preop) # calls sys.exit()

        binary = os.environ["SPLUNK_HOME"] + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsLicense"
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsLicense-F-000: Can't find binary file " + binary)

        if key == '':
            subprocess.call([binary])
        else:
            subprocess.call([binary, key])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)

    except Exception, e:
        si.generateErrorResults(e)

