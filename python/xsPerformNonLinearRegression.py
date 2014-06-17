#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    outfile=''
    rType='Quad'
    tmpby=''
    x=''
    xCount=0
    y=''
    yCount=0
    byKeyword=''
    outputKeyword=''
    overKeyword=''

    if len(sys.argv) >1:
        for arg in sys.argv[1:]:
            if arg.lower() == "over":
                overKeyword="over"
            elif arg.lower() == "output":
                outputKeyword="output"
            elif not arg.startswith("__"):
                if overKeyword == '':
                    if len(y) < 1:
                        y = arg
                    else:
                        y = y + "," + arg
                    yCount = yCount + 1
                else:
                    if arg == "by" or arg == "BY":
                        byKeyword = "by"
                    elif byKeyword == "by":
                        tmpby = arg
                        byKeyword = ''
                    elif outputKeyword == "output":
                        outfile = arg
                    elif len(x) < 1:
                        x = arg
                        xCount = 1
                    else:
                        x = x + "," + arg
                        xCount = xCount + 1
    else:
        raise Exception("xsPerformNonLinearRegression-F-001: Usage: xsPerformNonLinearRegression (field)+ OVER (field)+ [BY field] [OUTPUT name]")

    if xCount < 1:
        raise Exception("xsPerformNonLinearRegression-F-003: Missing x parameter"); 

    if yCount > 0 and yCount != xCount:
        raise Exception("xsPerformNonLinearRegression-F-005: Unequal number of parameters"); 

    try:
        (isgetinfo, sys.argv) = si.isGetInfo(sys.argv)

        if isgetinfo:
            reqsop = True
            if rType == "Quad":
                preop = "xspreautoregress u=maxentropy "
            preop += " x=\"%s\" " % x
            if yCount > 0:
                preop += " y=\"%s\" " % y
            if tmpby != '':
                by = tmpby
                i=1
                while i<xCount:
                    by = by + "," + tmpby
                    i = i + 1
                preop += " b=\"%s\" " % by
            si.outputInfo(False, False, False, reqsop, preop) # calls sys.exit()

        binary = os.path.dirname(sys.argv[0]) + "/"  + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsPerformAutoRegression"
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsPerformNonLinearRegression-F-000: Can't find binary file " + binary)

        if outfile == '':
            if rType == "Quad":
                subprocess.call([binary])
        else:
            if rType == "Quad":
                subprocess.call([binary, '-f', outfile])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)

    except Exception, e:
        si.generateErrorResults(e)
