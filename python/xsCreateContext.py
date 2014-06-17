#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    avg = ''
    context_type = "domain"
    count = ''
    end_shape = 'curve'
    max = ''
    min = ''
    notes = ''
    scope = '3'
    settings = {} 
    shape = 'pi'
    stdev = ''
    uom = ''

    try:
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower().startswith("avg="):
                    eqsign = arg.find('=')
                    avg = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("count="):
                    eqsign = arg.find('=')
                    count = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("endshape="):
                    eqsign = arg.find('=')
                    end_shape = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("max="):
                    eqsign = arg.find('=')
                    max = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("min="):
                    eqsign = arg.find('=')
                    min = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("name="):
                    eqsign = arg.find('=')
                    set_name = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("notes="):
                    eqsign = arg.find('=')
                    notes = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("scope="):
                    eqsign = arg.find('=')
                    scope = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("shape="):
                    eqsign = arg.find('=')
                    shape = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("stdev="):
                    eqsign = arg.find('=')
                    stdev = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("terms="):
                    eqsign = arg.find('=')
                    term_list = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("type="):
                    eqsign = arg.find('=')
                    context_type = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("uom="):
                    eqsign = arg.find('=')
                    uom = arg[eqsign+1:len(arg)]
                else:
                    errString = "xsCreateContext-F-003: Invalid argument: " + arg
                    raise Exception(errString) 
        else:
            raise Exception("xsCreateContext-F-001: Usage: xsCreateContext name=<string> terms=<conceptlist-option> (type=<contexttype-option>)? (<fuzzyvalues-option>)*")

        results = si.readResults(None, settings, True)
        for res in results:
            if 'avg' in res:
                avg = res['avg']
            if 'count' in res:
                count = res['count']
            if 'max' in res:
                max = res['max']
            if 'min' in res:
                min = res['min']
            if 'stdev' in res:
                stdev = res['stdev']

        if avg == '':
            raise Exception("xsCreateContext-F-003: parameter 'avg' not found")
        if count == '':
            raise Exception("xsCreateContext-F-005: parameter 'count' not found")
        if max == '':
            raise Exception("xsCreateContext-F-007: parameter 'max' not found")
        if min == '':
            raise Exception("xsCreateContext-F-009: parameter 'min' not found")
        if stdev == '':
            raise Exception("xsCreateContext-F-011: parameter 'stdev' not found")

        if notes == '':
            notes = 'none'

        info_file = settings['infoPath']
        binary = os.path.dirname(sys.argv[0]) + "/"  + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsCreateContext"
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsCreateContext-F-000: Can't find binary file " + binary)

        subprocess.call([binary, '-a', avg, '-c', count, '-d', stdev, '-e', end_shape, '-f', scope, '-i', info_file, '-m', min, '-n', set_name, '-o', notes, '-p', shape, '-t', term_list, '-u', uom, '-x', max, '-z', context_type ])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)

    except Exception, e:
        si.generateErrorResults(e)
