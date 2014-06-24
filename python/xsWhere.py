#
# Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time 
import splunk.Intersplunk as si
import splunk.clilib.cli_common as cli_common

if __name__ == '__main__':

    try:
        alfacut='0.2'
        cixname='WhereCIX'
        cixtype='avg'
        scalar='0.025'
        synonyms = 'synonyms'
        where=' '

        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower().startswith("alfacut="):
                    eqsign = arg.find('=')
                    alfacut = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("cixname="):
                    eqsign = arg.find('=')
                    cixname = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("cixtype="):
                    eqsign = arg.find('=')
                    cixtype = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("scalar="):
                    eqsign = arg.find('=')
                    scalar = arg[eqsign+1:len(arg)]
                elif arg.lower().startswith("synonyms="):
                    eqsign = arg.find('=')
                    synonyms = arg[eqsign+1:len(arg)]
                else:
                    where=where+' '+arg
        else:
            raise Exception("xsWhere-F-001: Usage: xsWhere [alfacut=value] field [IN context] is [modifier, ...] concept [and, or]"); 

        if where == ' ':
            raise Exception("xsWhere-F-003: No query")

        binary = os.path.dirname(sys.argv[0]) + "/" +  platform.system() + "/" + platform.architecture()[0] + "/xsWhere" 
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsWhere-F-000: Can't find binary file " + binary)

        subprocess.call([binary, '-a', alfacut, '-c', cixtype, '-n', cixname, '-p', scalar, '-s', synonyms, '-w', where])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)

    except Exception, e:
        si.generateErrorResults(e)
