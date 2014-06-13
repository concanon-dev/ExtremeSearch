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
        key=''
        reactivate=''
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower().startswith("key="):
                    eqsign = arg.find('=')
                    key = arg[eqsign+1:len(arg)]
                if arg.lower().startswith("reactivate="):
                    eqsign = arg.find('=')
                    reactivate = arg[eqsign+1:len(arg)]

        binary = os.environ["SPLUNK_HOME"] + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsprelicense"
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsprelicense-F-000: Can't find binary file " + binary)


        if key != '' and reactivate != '':
            subprocess.call([binary, '-r', key])
        elif key != '' and reactivate == '':
            subprocess.call([binary, '-s', key])
        else:
            subprocess.call([binary])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)
    
    except Exception, e:
        si.generateErrorResults(e)
