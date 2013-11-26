#
# (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

#    print platform.platform()
#    print platform.processor()
#    release = platform.release()
#    system = platform.system() + "/" + platform.architecture()[0]
#    version = platform.version()
#    print platform.system_alias(system, release, version)
#    print platform.uname()

    x=''
    y=''

    if len(sys.argv) != 1:
        raise Exception("xsrecorrelate-F-001: Usage: xsrecorrelate"); 

    try:
        binary = platform.system() + "/" + platform.architecture()[0] + "/xsrecorrelate"
        subprocess.call([binary])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
