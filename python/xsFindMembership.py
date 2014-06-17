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

        context=''
        field=''
        scope=''
        inKeyword=''
        in2Keyword=''
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower() == "in":
                    if inKeyword == "in":
                        in2Keyword = "in"
                    else:
                        inKeyword="in"
                elif in2Keyword == 'in':
                    scope = arg
                elif inKeyword == 'in':
                    context = arg
                else: 
                    field = arg
        else:
            raise Exception("xsFindMembership-F-001: Usage: xsFindMembership field [IN context [IN scope]]");

        if len(context) < 1:
            context = field

        binary = os.path.dirname(sys.argv[0]) + "/" +  platform.system() + "/" + platform.architecture()[0] + "/xsFindMembership"
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsFindMembership-F-000: Can't find binary file " + binary)

        if scope == '':
            subprocess.call([binary, '-c', context, '-f', field ])
        else:
            subprocess.call([binary, '-c', context, '-f', field, '-s', scope])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)

    except Exception, e:
        si.generateErrorResults(e)
