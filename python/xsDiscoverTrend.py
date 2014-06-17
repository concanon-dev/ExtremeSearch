#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    c=''
    time='_time'
    tmpby = ''
    tmpC = ''
    x=''
    xCount = 0
    y=''
    yCount = 0
    byKeyword=''
    inKeyword=''
    overKeyword=''

    if len(sys.argv) >1:
        for arg in sys.argv[1:]:
            if arg.lower() == "over":
                overKeyword = "over"
            elif arg.lower() == "in":
                inKeyword = "in"
            elif not arg.startswith("__"):
                if inKeyword == "in":
                    tmpC = arg
                    inKeyword = ''
                elif overKeyword == '':
                    if y == '':
                        y = arg
                    else:
                        y = y + "," + arg
                    if c == '':
                        c = tmpC
                    else:
                        c = c + "," + tmpC
                    tmpC = arg
                    yCount = yCount + 1
                else:
                    if arg.lower() == "by":
                        byKeyword = "by"
                    elif byKeyword == "by":
                        tmpby = arg
                        byKeyword = ''
                    elif x == '':
                        x = arg
                        xCount = 1
                    else:
                        x = x + "," + arg
                        xCount = xCount + 1
    else:
        raise Exception("xsDiscoverTrend-F-001: Usage: xsDiscoverTrend (field [IN context])+ [OVER (field)+] [BY field]")

    if len(y) < 1:
        raise Exception("xsDiscoverTrend-F-005: Missing y parameter")
    if xCount != 0 and xCount != yCount:
        raise Exception("xsDiscoverTrend-F-007: Unequal number of parameters")

    if x == '':
        x=time

    if c =='':
        c = tmpC
    else:
        c = c + "," + tmpC

    try:
        (isgetinfo, sys.argv) = si.isGetInfo(sys.argv)

        if isgetinfo:
            if xCount == 0:
                i = 1
                x = time
                while i < yCount:
                    x = x + "," + time
                    i = i + 1
            reqsop = True
            preop = "xspreregress -r "
            preop += " x=\"%s\" " % x
            preop += " y=\"%s\" " % y
            if tmpby != '':
                by = tmpby
                i=1
                while i<xCount:
                    by = by + "," + tmpby
                    i = i + 1
                preop += " b=\"%s\" " % by
            si.outputInfo(False, False, False, reqsop, preop) # calls sys.exit()

        binary = os.path.dirname(sys.argv[0]) + "/"  + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsDiscoverTrend"
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsDiscoverTrend-F-000: Can't find binary file " + binary)

        subprocess.call([binary, '-c', c, '-y', y])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)

    except Exception, e:
        si.generateErrorResults(e)
