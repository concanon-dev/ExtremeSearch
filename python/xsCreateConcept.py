#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    contextName = ''
    inKeyword = ''
    max = ''
    min = ''
    scope = '0'
    shape = 'pi'
    termName = ''
    withKeyword = ''
    try:
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if inKeyword == '':
                    if arg.lower() == 'in':
                        inKeyword = 'in'
                    else:
                        termName = arg
                else:
                    if withKeyword == '':
                        if arg.lower() == 'with':
                            withKeyword = 'with'
                        else:
                            contextName = arg
                    else:
                        if arg.lower().startswith("max="):
                            eqsign = arg.find('=')
                            max = arg[eqsign+1:len(arg)]
                        elif arg.lower().startswith("min="):
                            eqsign = arg.find('=')
                            min = arg[eqsign+1:len(arg)]
                        elif arg.lower().startswith("scope="):
                            eqsign = arg.find('=')
                            scope = arg[eqsign+1:len(arg)]
                        elif arg.lower().startswith("shape="):
                            eqsign = arg.find('=')
                            shape = arg[eqsign+1:len(arg)]
                        else:
                            errString = "xsCreateConcept-F-003: Invalid argument: " + arg
                            raise Exception(errString) 
        else:
            raise Exception("xsCreateConcept-F-001: Usage: xsCreateConcept termName IN contextName WITH min=double max=double [shape=shapeStr] [scope=(none|private|app|global)]")

        if max == '':
            raise Exception("xsCreateConcept-F-007: parameter 'max' not found")
        if min == '':
            raise Exception("xsCreateConcept-F-009: parameter 'min' not found")

        binary = os.environ["SPLUNK_HOME"] + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsCreateConcept"
        subprocess.call([binary, '-c', contextName, '-m', min, '-n', termName, '-p', shape, '-s', scope, '-x', max ])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
