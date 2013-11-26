#
# (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    isKeyword=''
    p=''
    tmpX=''
    whenKeyword=''
    x=''
    y=''
    yCount=0

    if len(sys.argv) >1:
        for arg in sys.argv[1:]:
            if arg.lower() == "when":
                whenKeyword = "when"
            elif whenKeyword == "when":
                if arg == "is" or arg == "IS":
                    isKeyword = "is"
                elif isKeyword == "is":
                    p = arg
                else:
                    tmpX = arg
            elif not arg.startswith("__"):
                if y == '':
                    y = arg
                else:
                    y = y + "," + arg
                yCount = yCount + 1
    else:
        raise Exception("xspredict-F-001: Usage: xspredict (field)+ WHEN field IS value")

    if len(tmpX) < 1:
        raise Exception("xspredict-F-003: Missing x parameter")

    if len(y) < 1:
        raise Exception("xspredict-F-005: Missing y parameter")

    if len(p) < 1:
        raise Exception("xspredict-F-007: Missing p parameter")

    if len(isKeyword) < 1:
        raise Exception("xspredict-F-009: Missing IS keyqord")

    if len(whenKeyword) < 1:
        raise Exception("xspredict-F-011: Missing WHEN keyword")

    try:
        (isgetinfo, sys.argv) = si.isGetInfo(sys.argv)
        out=sys.argv[1]

        if isgetinfo:
            x=tmpX
            i=1
            while i<yCount:
                x = x + "," + tmpX
                i = i + 1
            reqsop = True
            preop = "xspreregress "
            preop += " x=\"%s\" " % x
            preop += " y=\"%s\" " % y
            si.outputInfo(False, False, False, reqsop, preop) # calls sys.exit()

        binary = platform.system() + "/" + platform.architecture()[0] + "/xspredict"
        subprocess.call([binary, '-p', p])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
