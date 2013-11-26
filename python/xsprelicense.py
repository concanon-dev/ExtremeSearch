#
# (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    try:
        key=''

        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower().startswith("key="):
                    eqsign = arg.find('=')
                    key = arg[eqsign+1:len(arg)]

        binary = platform.system() + "/" + platform.architecture()[0] + "/xsprelicense"

        if key == '':
            subprocess.call([binary])
        else:
            subprocess.call([binary, '-s', key])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)
    
    except Exception, e:
        si.generateErrorResults(e)
