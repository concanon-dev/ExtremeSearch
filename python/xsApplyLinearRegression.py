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

        a=''
        b=''
        f=''
        xList=''
        fromKeyword=''
        overKeyword=''

        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower() == "from":
                    fromKeyword="from"
                elif arg.lower() == "over":
                    overKeyword="over"
                elif not arg.startswith("__"):
                    if fromKeyword != '':
                        f = arg
                    elif overKeyword == '':
                        if arg.lower().startswith("slope="):
                            eqsign = arg.find('=')
                            a = arg[eqsign+1:len(arg)]
                        elif arg.lower().startswith("intercept="):
                            eqsign = arg.find('=')
                            b = arg[eqsign+1:len(arg)]
                    else:
                        if xList == '':
                            xList = arg
                        else:
                            xList = xList + "," + arg 
        else:
            raise Exception("xsApplyLinearRegression-F-001: Usage: xsApplyLinearRegression ((FROM file) | (slope=value intercept=value OVER (field)+))");

        if f != '':
            f = "../lookups/" + f + ".csv"

            binary = os.environ["SPLUNK_HOME"] + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + '/xsApplyLinearRegressionFromFile'
            if not os.path.isfile(binary):
                raise Exception("xsApplyLinearRegression-F-000: Can't find binary file " + binary)

            subprocess.call([binary, '-f', f])

        else:
            if len(a) < 1:
                raise Exception("xsApplyLinearRegression-F-003: Missing slope parameter");

            if len(b) < 1:
                raise Exception("xsApplyLinearRegression-F-005: Missing intercept parameter");

            if len(xList) < 1:
                raise Exception("xsApplyLinearRegression-F-007: Missing X parameter");

            binary = os.environ["SPLUNK_HOME"] + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + '/xsApplyLinearRegression'
            subprocess.call([binary, '-a', a, '-b', b, '-x', xList])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
