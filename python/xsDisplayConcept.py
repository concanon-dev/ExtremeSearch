#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, time
import splunk.Intersplunk as si

if __name__ == '__main__':

    x = 'NO'
    addScope = 1
    addSemicolon = 0
    fromKeyword = ''
    inKeyword = ''
    scope = ''
    setSeparator = ";"
    setString = ''
    synonyms = 'synonyms.csv'
    context = ''
    usingKeyword = ''
    try:
        if len(sys.argv) >1:
            for arg in sys.argv[1:]:
                if arg.lower() == "from":
                    fromKeyword = "from"
                elif arg.lower() == "using":
                    usingKeyword = "using"
                elif arg.lower() == "in":
                    inKeyword = "in"
                else:
                    if arg.endswith(setSeparator) and arg != setSeparator:
                        comma = arg.find(setSeparator)
                        arg = arg[0:comma]
                        addSemicolon = 1
                    if fromKeyword == "from":
                        context = context + arg
                        fromKeyword = ''
                        addScope = 1
                    elif inKeyword == "in":
                        scope = scope + arg
                        inKeyword = ''
                        addScope = 0
                    elif usingKeyword == "using":
                        synonyms = arg
                        usingKeyword = ''
                    elif arg != setSeparator:
                        if setString == '':
                            setString = arg
                        else:
                            if setString[len(setString)-1:len(setString)] == ';':
                                setString = setString + arg
                            else:
                                setString = setString + "," + arg
                    else:
                        addSemicolon = 1
                        x = "YES"
                    if addSemicolon == 1:
                        context = context + ";"
                        setString = setString + ";"                     
                        if addScope == 1:
                            scope = scope + "none"
                            addScope = 0
                        scope = scope + ";"
                        addSemicolon = 0
        else:
            raise Exception("xsDisplayConcept-F-001: Usage: xsDisplayConcept [hedge]* term FROM context [IN scope] [; [hedge]* term FROM context [IN scope] ]* [USING synonyms]") 

        if addScope == 1:
            scope = scope + "none"

        synonyms = "../lookups/" + synonyms

        binary = os.path.dirname(sys.argv[0]) + "/"  + "/etc/apps/xtreme/bin/" +  platform.system() + "/" + platform.architecture()[0] + "/xsDisplayConcept"
        if (platform.system() == 'Windows'):
            binary = binary + ".exe"
        if not os.path.isfile(binary):
            raise Exception("xsDisplayConcept-F-000: Can't find binary file " + binary)

        subprocess.call([binary, '-c', context, '-l',  setString, '-p', scope, '-s', synonyms ])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(1.0)

    except Exception, e:
        si.generateErrorResults(e)
