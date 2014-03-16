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
        b=''
        u=''
        x=''

        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower().startswith("b="):
                    eqsign = arg.find('=')
                    b = arg[eqsign+1:len(arg)]
                if arg.lower().startswith("u="):
                    eqsign = arg.find('=')
                    u = arg[eqsign+1:len(arg)]
                if arg.lower().startswith("x="):
                    eqsign = arg.find('=')
                    x = arg[eqsign+1:len(arg)]
        else:
            raise Exception("xspreautoregress-F-001: Usage: xspreautoregress x=\"field1,field2...\" [u=method] [b=field]")

        if len(x) < 1:
            raise Exception("xspreautoregress-F-003: Missing x parameter")

        binary = platform.system() + "/" + platform.architecture()[0] + "/xspreautoregress"
        if b != '':
            subprocess.call([binary, '-b', b, '-i', '-m', u, '-x', x ])
        else:
            subprocess.call([binary, '-i', '-m', u, '-x', x ])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
