#
# (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    avg = "0.0"
    count = "0"
    max = "0.0"
    min = "0.0"
    stdev = "0.0"
    set_list = ''
    set_type = "ZC"

    oldset = ''
    try:
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower().startswith("avg="):
                    eqsign = arg.find('=')
                    avg = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("count="):
                    eqsign = arg.find('=')
                    count = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("max="):
                    eqsign = arg.find('=')
                    max = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("min="):
                    eqsign = arg.find('=')
                    min = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("name="):
                    eqsign = arg.find('=')
                    set_name = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("old="):
                    eqsign = arg.find('=')
                    oldset = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("terms="):
                    eqsign = arg.find('=')
                    set_list = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("stdev="):
                    eqsign = arg.find('=')
                    stdev = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("type="):
                    eqsign = arg.find('=')
                    set_type = arg[eqsign+1:len(arg)]
                else:
                    errString = "xsUpdateContext-F-003: Invalid argument: " + arg
                    raise Exception(errString) 
        else:
            raise Exception("xsUpdateContext-F-001: Usage: xsUpdateContext old=filename name=<string> terms=<semantictermlist-option> (type=<semantictermtype-option>)? (<fuzzyvalues-option>)*")

        if oldset == '':
            raise Exception("xsUpdateContext-F-005: no old set specified")
            
        results = si.readResults(None, None, False)
        for res in results:
            count = res['count']
            min = res['min']
            max = res['max']
            avg = res['avg']
            stdev = res['stdev']

        file_name = "../contexts/" + set_name + ".context"

        binary = platform.system() + "/" + platform.architecture()[0] + "/xsUpdateContext"
        subprocess.call([binary, '-f', file_name, '-n', set_name, '-c', count, '-m', min, '-x', max, '-a', avg, '-d', stdev, '-s', set_list, '-z', set_type, '-o', oldset ])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
