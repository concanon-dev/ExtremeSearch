#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    props = ''
    try:
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower() == "from":
                    fromKeyword = "from"
                elif fromKeyword == "from":
                    props = arg
                    fromKeyword = ''
        else:
            raise Exception("xsGenerateData-F-001: Usage: xsGenerateData FROM props-file")

        binary = platform.system() + "/" + platform.architecture()[0] + "/xsGenerateData"
        subprocess.call([binary, '-p', props ])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
