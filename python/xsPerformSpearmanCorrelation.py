#
# Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    by=''
    outfile=''
    x=''
    xCount=0
    y=''
    yCount=0
    byKeyword=''
    outputKeyword=''
    withKeyword=''

    if len(sys.argv) >1:
        for arg in sys.argv[1:]:
            if arg.lower() == "output":
                outputKeyword="output"
            elif arg.lower() == "with":
                withKeyword="with"
            elif outputKeyword == "output":
                outfile = arg
            elif not arg.startswith("__"):
                if withKeyword == '':
                    if len(x) < 1:
                        x = arg
                    else:
                        x = x + "," + arg
                    xCount = xCount + 1
                else:
                    if arg.lower() == "by":
                        byKeyword = "by"
                    elif byKeyword == "by":
                        by = arg
                    elif len(y) < 1:
                        y = arg
                        yCount = 1
                    else:
                        y = y + "," + arg
                        yCount = yCount + 1
    else:
        raise Exception("xsPerformSpearmanCorrelation-F-001: Usage: xsPerformSpearmanCorrelation (field)+ WITH (field)+ [BY field]")

    if xCount < 1:
        raise Exception("xsPerformSpearmanCorrelation-F-003: Missing x parameter"); 

    if yCount > 0 and yCount != xCount:
        raise Exception("xsPerformSpearmanCorrelation-F-005: Unequal number of parameters"); 

    try:
        (isgetinfo, sys.argv) = si.isGetInfo(sys.argv)

        if isgetinfo:
            reqsop = True
            preop = "xsprespearmancorrelate "
            preop += " x=\"%s\" " % x
            if yCount > 0:
                preop += " y=\"%s\" " % y
            preop += " b=\"%s\" " % by
            si.outputInfo(False, False, False, reqsop, preop) # calls sys.exit()

        binary = os.path.dirname(sys.argv[0]) + "/" +  platform.system() + "/" + platform.architecture()[0] + "/xsPerformSpearmanCorrelation"
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsPerformSpearmanCorrelation-F-000: Can't find binary file " + binary)

        if outfile == '':
            subprocess.call([binary])
        else: 
            subprocess.call([binary, '-f', outfile])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)

    except Exception, e:
        si.generateErrorResults(e)
