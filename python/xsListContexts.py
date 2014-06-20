#
# Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    scope = '3'
    try:
        if len(sys.argv) < 1:
            raise Exception("xsListContexts-F-001: Usage: xsListContexts [scope]") 

        if len(sys.argv) == 2:
            scope = sys.argv[1]
            scope = scope.lower()

        binary = os.path.dirname(sys.argv[0]) + "/" +  platform.system() + "/" + platform.architecture()[0] + "/xsListContexts"
	if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsListContexts-F-000: Can't find binary file " + binary)

        subprocess.call([binary, '-s', scope])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)

    except Exception, e:
        si.generateErrorResults(e)
