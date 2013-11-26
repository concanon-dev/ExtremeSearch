#
# (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    try:

        coef0=''
        coef1=''
        coef2=''
        f=''
        xList=''
        fromKeyword=''
        overKeyword=''
        rType="Quad"

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
                        if arg.lower().startswith("coef0="):
                            eqsign = arg.find('=')
                            coef0 = arg[eqsign+1:len(arg)]
                        elif arg.lower().startswith("coef1="):
                            eqsign = arg.find('=')
                            coef1 = arg[eqsign+1:len(arg)]
                        elif arg.lower().startswith("coef2="):
                            eqsign = arg.find('=')
                            coef2 = arg[eqsign+1:len(arg)]
                    else:
                        if xList == '':
                            xList = arg
                        else:
                            xList = xList + "," + arg 
        else:
            raise Exception("xsApplyAutoRegression-F-001: Usage: xsApplyAutoRegression ((FROM file) | (coef0=value coef1=value ceof2=value OVER (field)+))  ");

        if f != '':
            f = "../lookups/" + f + ".csv"

            binary = platform.system() + "/" + platform.architecture()[0] + "/xsApplyAutoRegressionFromFile"
            subprocess.call([binary, '-f', f])

        else:
            if len(coef0) < 1:
                raise Exception("xsApplyAutoRegression-F-003: Missing coef0 parameter");

            if len(coef1) < 1:
                raise Exception("xsApplyAutoRegression-F-005: Missing coef1 parameter");

            if len(coef2) < 1:
                raise Exception("xsApplyAutoRegression-F-007: Missing coef2 parameter");

            if len(xList) < 1:
                raise Exception("xsApplyAutoRegression-F-009: Missing X parameter");

            if rType == "Quad":
                binary = platform.system() + "/" + platform.architecture()[0] + "/xsApplyAutoRegression"
                subprocess.call([binary, '-0', coef0, '-1', coef1, '-2', coef2, '-x', xList])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
