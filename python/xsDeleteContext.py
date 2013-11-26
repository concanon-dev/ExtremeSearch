#
# (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, shutil
import splunk.Intersplunk as si

if __name__ == '__main__':

    context=''    
    scope=''
    try:

        if len(sys.argv) > 1:
            for arg in sys.argv[1:]:
                if arg.lower() == 'in'
                    inKeyword = 'in'
                elif inKeyword == 'in':
                    scope=arg
                else:
                    context=arg
        else:
            raise Exception("xsDeleteContext-F-001: Usage: xsDeleteContext Context [IN scope]");

        binary = platform.system() + "/" + platform.architecture()[0] + "/xsDeleteContext"
        if scope == '':
            subprocess.call([binary, '-c', context ])
        else:
            subprocess.call([binary, '-c', context, '-s', scope])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
