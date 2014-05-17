#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    try:
        b=''
        x=''
        y='0'

        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower().startswith("x="):
                    eqsign = arg.find('=')
                    x = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("y="):
                    eqsign = arg.find('=')
                    y = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("b="):
                    eqsign = arg.find('=')
                    b = arg[eqsign+1:len(arg)]
        else:
            raise Exception("xsprecorrelate-F-001: Usage: xsprecorrelate x=field y=field"); 

        if len(x) < 1:
            raise Exception("xsprecorrelate-F-003: Missing x parameter"); 

        binary = os.environ["SPLUNK_HOME"] + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsprecorrelate"
        if b == '':
            subprocess.call([binary, '-i', '-x', x, '-y', y])
        else:
            subprocess.call([binary, '-i', '-x', x, '-y', y, '-b', b])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
