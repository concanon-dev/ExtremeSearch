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
    tmpby=''
    x=''
    xCount=0
    using=''
    byKeyword=''
    outputKeyword=''
    usingKeyword=''
    using="maxentropy"
    if len(sys.argv) >1:
        for arg in sys.argv[1:]:
            if arg.lower() == "output":
                outputKeyword="output"
            elif arg.lower() == "using":
                usingKeyword="using"
            elif not arg.startswith("__"):
                if arg.lower() == "by":
                    byKeyword = "by"
                elif byKeyword == "by":
                    tmpby = arg
                    byKeyword = ''
                elif usingKeyword == "using":
                    using = arg.lower()
                    usingKeyword = ''
                elif outputKeyword == "output":
                    outfile = arg
                elif len(x) < 1:
                    x = arg
                    xCount = 1
                else:
                    x = x + "," + arg
                    xCount = xCount + 1
    else:
        raise Exception("xsPerformAutoRegression-F-001: Usage: xsPerformAutoRegression (field)+ [BY field] [USING (maxentropy | leastsquares)]]")

    if xCount < 1:
        raise Exception("xsPerformAutoRegression-F-003: Missing x parameter"); 

    try:
        (isgetinfo, sys.argv) = si.isGetInfo(sys.argv)

        if isgetinfo:
            reqsop = True
            preop = "xspreautoregress "
            preop += " x=\"%s\" " % x
            if using != '':
                preop += " u=\"%s\" " % using
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
            raise Exception("xsPerformAutoRegression-F-000: Can't find binary file " + binary)

        if outfile == '':
            subprocess.call([binary])
        else:
            subprocess.call([binary, '-f', outfile])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)

    except Exception, e:
        si.generateErrorResults(e)
