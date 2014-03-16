#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, shutil
import splunk.Intersplunk as si

if __name__ == '__main__':

    count=1
    fromField=''
    fromKeyword=''
    toField=''
    toKeyword=''
    try:
        if len(sys.argv) > 1:
            for arg in sys.argv[1:]:
                if toKeyword == '':
                    if count == 1:
                        if arg.lower() != "from":
                            fromField = arg
                        count = 2
                    else:
                        if arg.lower() == "to":
                            toKeyword = "to"
                        elif fromField == '':
                            fromField = arg
                        else:
                            fromField = fromField + "," + arg
                else:
                    if toField == '':
                        toField = arg
                    else:
                        toField = toField + "," + arg

        else:
            raise Exception("xsGetDistance-F-001: Usage: FROM (lat long | zipcode | city region [country] TO (lat long | zipcode | city region [country])");

        binary = platform.system() + "/" + platform.architecture()[0] + "/xsGetDistance"
        subprocess.call([binary, '-f', fromField, '-t', toField ])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)
    
    except Exception, e:
        si.generateErrorResults(e)
