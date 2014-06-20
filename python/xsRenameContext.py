#
# Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, shutil, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    dest=''
    destScope='0'
    srcScope='0'
    src=''    
    inKeyword = ''
    in2Keyword = ''
    toKeyword = ''
    try:
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower() == "in":
                    if toKeyword == '':
                        inKeyword="in"
                    else:
                        in2Keyword="in"
                elif arg.lower() == "to":
                    toKeyword = "to"
                elif inKeyword == 'in':
                    srcScope = arg.lower()
                    inKeyword = ''
                elif in2Keyword == 'in':
                    destScope = arg.lower()
                    in2Keyword = ''
                elif toKeyword == '': 
                    src = arg
                else:
                    dest = arg

        if src == '' or dest == '':
            raise Exception("xsRenameContext-F-001: Usage: xsRenameContext src [in scope] TO dest [in scope]");
        if src.find(".") != -1 or src.find("/") != -1:
            raise Exception("xsRenameContext-F-003: src can not contain a '/' or '.'");
        if dest.find(".") != -1 or dest.find("/") != -1:
            raise Exception("xsRenameContext-F-005: dest can not contain a '/' or '.'");

        binary = os.path.dirname(sys.argv[0]) + "/" +  platform.system() + "/" + platform.architecture()[0] + "/xsRenameContext"
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsRenameContext-F-000: Can't find binary file " + binary)

        subprocess.call([binary, '-s', src, '-d', dest, '-0', srcScope, '-1', destScope])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)
            
    except Exception, e:
        si.generateErrorResults(e)
