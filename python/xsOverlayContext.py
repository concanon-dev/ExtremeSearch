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
        if len(sys.argv) > 1:
            for arg in sys.argv[1:]:
                if arg.lower().startswith("count="):
                    eqsign = arg.find('=')
                    count = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("key="):
                    eqsign = arg.find('=')
                    key = arg[eqsign+1:len(arg)]
                else:
                    name = arg
        else:
            raise Exception("xsOverlayContext-F-001: Usage: xsOverlayContext key=field count=field context") 


        filePath = "../contexts/" + name + ".context"

        binary = os.path.dirname(sys.argv[0]) + "/"  + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsOverlayContext"
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsOverlayContext-F-000: Can't find binary file " + binary)

        subprocess.call([binary, '-c', count, '-f', filePath, '-k', key])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)

    except Exception, e:
        si.generateErrorResults(e)
