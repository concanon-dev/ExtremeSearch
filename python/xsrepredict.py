#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
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
    p=''

    if len(sys.argv) >1:
        for arg in sys.argv[1:]:
            if arg.lower().startswith("p="):
                eqsign = arg.find("=")
                p = arg[eqsign+1:len(arg)]
    else:
        raise Exception("xsrepredict-F-001: Usage: xpredict (x=field y=field)+ p=value")

    if len(p) < 1:
        raise Exception("xsrepredict-F-003: Missing predict parameter (p=)")

    try:
        binary = os.environ["SPLUNK_HOME"] + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsrepredict"
        subprocess.call([binary, '-p', p])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
