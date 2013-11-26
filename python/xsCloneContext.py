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
    newScope='private'
    old=''    
    oldScope='private'
    inKeyword=''
    toKeyword=''
    try:
        if len(sys.argv) > 1:
            for arg in sys.argv[1:]:
                if arg.lower() == "to":
                    toKeyword="to"
                elif arg.lower() == "in":
                    inKeyword="in"
                elif toKeyword == '':
                    if inKeyword == '':
                        old=arg
                    else:
                        oldScope=arg
                        inKeyword = ''
                else:
                    if inKeyword == '':
                        new=arg
                    else:
                        newScope=arg
        else:
            raise Exception("xsCloneContext-F-001: Usage: xsCloneContext oldContext [IN scope] TO newContext [IN scope]");

        binary = platform.system() + "/" + platform.architecture()[0] + "/xsCloneContext"
        subprocess.call([binary, '-N', newScope, '-O', oldScope, '-n', new, '-o', old])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)
    
    except Exception, e:
        si.generateErrorResults(e)
