#
# (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    try:

        newContext = ''
        newScope = 'private'
        oldContext = ''
        oldScope = 'private'
        semanticTerm = ''

        fromKeyword = ''
        inKeyword = ''
        toKeyword = ''
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower() == "to":
                    toKeyword="to"
                    inKeyword = ''
                elif arg.lower() == "in":
                    inKeyword="in"
                elif arg.lower() == "from":
                    fromKeyword = 'from'
                elif toKeyword == '':
                    if fromKeyword == '':
                        semanticTerm = arg
                    elif inKeyword == '':
                        oldContext = arg
                    else:
                        oldScope = arg
                        inKeyword = ''
                else:
                    if inKeyword == '':
                        newContext = arg
                    else:
                        newScope = arg
        else:
            raise Exception("xsCloneSemanticTerm-F-001: Usage: xsCloneSemanticTerm semanticTerm FROM oldContext [ IN scope ] TO newContext [ IN scope ]");

        if newSet == '' or newContext == '' or oldSet == '' or oldContext == '':
            raise Exception("xsCloneSemanticTerm-F-001: Usage: xsCloneSemanticTerm oldSemanticTerm IN oldContext TO newSemanticTerm IN newContext");

        binary = platform.system() + "/" + platform.architecture()[0] + "/xsCloneSemanticTerm"
        subprocess.call([binary, '-N', newScope, '-O', oldScope, '-n', newContext, '-o', oldContext, '-s', semanticTerm])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)

    except Exception, e:
        si.generateErrorResults(e)
