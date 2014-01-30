#
# (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    scope = ''
    context = ''
    fromKeyword = ''
    inKeyword = ''
    try:
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower() == "from":
                    fromKeyword = "from"
                elif arg.lower() == "in":
                    inKeyword = "in"
                elif fromKeyword == "from":
                    context = arg
                elif inKeyword == "in":
                    scope = arg
        else:
            raise Exception("xsListSemanticTerms-F-001: Usage: xsListSemanticTerms FROM context [IN scope]")

        binary = platform.system() + "/" + platform.architecture()[0] + "/xsListSemanticTerms"
        subprocess.call([binary, '-c', context, '-p', scope ])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)