#
# (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, shutil
import splunk.Intersplunk as si

if __name__ == '__main__':

    new=''
    old=''    
    toKeyword=''
    try:
        if len(sys.argv) > 1:
            for arg in sys.argv[1:]:
                if arg.lower() == "to":
                    toKeyword="to"
                elif toKeyword == '':
                    old=arg
                else:
                    new=arg
        else:
            raise Exception("xsconvert-F-001: Usage: xsconvert oldContext TO newContext");

        binary = platform.system() + "/" + platform.architecture()[0] + "/xsconvert"
        subprocess.call([binary, oldContext, newContext ])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)
    
    except Exception, e:
        si.generateErrorResults(e)
