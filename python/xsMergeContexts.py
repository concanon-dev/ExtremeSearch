#
# Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    try:

        contextA = ''
        contextB = ''
        contextC = ''
        scopeA = ''
        scopeB = ''
        scopeC = 'private'

        andKeyword = ''
        inKeyword = ''
        intoKeyword = ''
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower() == "and":
                    andKeyword="and"
                    inKeyword = ''
                elif arg.lower() == "in":
                    inKeyword = "in"
                elif arg.lower() == "into":
                    intoKeyword="into"
                    inKeyword = ''
                elif intoKeyword == '':
                    if andKeyword == '':
                        if inKeyword == '':
                            contextA = arg
                        else:
                            scopeA = arg
                            inKeyword = ''
                    else:
                        if inKeyword == '':
                            contextB = arg
                        else:
                            scopeB = arg
                            inKeyword = ''
                else:
                    if inKeyword == '':
                        contextC = arg
                    else:
                        scopeC = arg
        else:
            raise Exception("xsMergeContexts-F-001: Usage: xsMergeContexts contextA [IN scopeA] AND contextB [IN scopeA] INTO contextC [IN scopeC]");

        if contextA == '' or contextB  == '' or contextC  == '':
            raise Exception("xsMergeContexts-F-001: Usage: xsMergeContexts contextA [IN scopeA] AND contextB [IN scopeA] INTO contextC [IN scopeC]");

        binary = os.path.dirname(sys.argv[0]) + "/" +  platform.system() + "/" + platform.architecture()[0] + "/xsMergeContexts"
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsMergeContexts-F-000: Can't find binary file " + binary)

        subprocess.call([binary, '-A', scopeA, '-B', scopeB, '-C', scopeC, '-a', contextA, '-b', contextB, '-c', contextC])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)

    except Exception, e:
        si.generateErrorResults(e)
