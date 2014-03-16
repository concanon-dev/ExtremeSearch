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

        context = ''
        fromKeyword = ''
        inKeyword = ''
        scope = 'none'
        term = ''
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower() == "from":
                    fromKeyword="from"
                elif arg.lower() == "in":
                    inKeyword = "in"
                elif fromKeyword != '':
                    context = arg
                    fromKeyword = ''
                elif inKeyword != '':
                    scope = arg
                    inKeyword = ''
                else:
                    term = arg
        else:
            raise Exception("xsDeleteConcept-F-001: Usage: xsDeleteConcept concept FROM context [IN scope]");

        if term == '' or context == '':
            raise Exception("xsDeleteConcept-F-001: Usage: xsDeleteConcept concept FROM context [IN scope]");

        binary = platform.system() + "/" + platform.architecture()[0] + "/xsDeleteConcept"
        subprocess.call([binary, '-c', context, '-s', scope, '-t', term])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
