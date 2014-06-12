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

        newContext = ''
        newScope = 'private'
        oldContext = ''
        oldScope = 'private'
        concept = ''

        fromKeyword = ''
        inKeyword = ''
        toKeyword = ''
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower() == "to":
                    toKeyword="to"
                    inKeyword = ''
                elif arg.lower() == "in":
                    inKeyword="in"
                elif arg.lower() == "from":
                    fromKeyword = 'from'
                elif toKeyword == '':
                    if fromKeyword == '':
                        concept = arg
                    elif inKeyword == '':
                        oldContext = arg
                    else:
                        oldScope = arg
                        inKeyword = ''
                else:
                    if inKeyword == '':
                        newContext = arg
                    else:
                        newScope = arg
        else:
            raise Exception("xsCloneConcept-F-001: Usage: xsCloneConcept concept FROM oldContext [ IN scope ] TO newContext [ IN scope ]");

        if newScope == '' or newContext == '' or oldScope == '' or oldContext == '' or concept == '':
            raise Exception("xsCloneConcept-F-001: Usage: xsCloneConcept oldConcept IN oldContext TO newConcept IN newContext");

        binary = os.environ["SPLUNK_HOME"] + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsCloneConcept"
        if not os.path.isfile(binary):
            raise Exception("xsCloneConcept-F-000: Can't find binary file " + binary)

        subprocess.call([binary, '-N', newScope, '-O', oldScope, '-n', newContext, '-o', oldContext, '-c', concept])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
