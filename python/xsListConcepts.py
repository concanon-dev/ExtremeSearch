#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
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
                    fromKeyword = ''
                elif inKeyword == "in":
                    scope = arg
                    inKeyword = ''
        else:
            raise Exception("xsListConcepts-F-001: Usage: xsListConcepts FROM context [IN scope]")

        binary = os.environ["SPLUNK_HOME"] + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsListConcepts"
        if not os.path.isfile(binary):
            raise Exception("xsListConcepts-F-000: Can't find binary file " + binary)

        subprocess.call([binary, '-c', context, '-p', scope ])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
