#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
import sys, subprocess, os, platform, shutil
import splunk.Intersplunk as si

if __name__ == '__main__':

    src=''    
    dest=''
    try:
        if len(sys.argv) == 3:
            src = sys.argv[1]
            dest = sys.argv[2]
        else:
            raise Exception("xsRenameContext-F-001: Usage: xsRenameContext oldContext newContext");

        if src.find(".") != -1 or src.find("/") != -1:
            raise Exception("xsRenameContext-F-003: oldContext can not contain a '/' or '.'");
        if dest.find(".") != -1 or dest.find("/") != -1:
            raise Exception("xsRenameContext-F-005: newContext can not contain a '/' or '.'");

        src = "../contexts/" + src + ".context"
        dest = "../contexts/" + dest + ".context"

        if os.path.exists(src):            
            if os.path.exists(dest):
                raise Exception("xsRenameContext-F-007: The new Context already exists: " + dest);
            shutil.move(src, dest)
        else:
            raise Exception("xsRenameContext-F-007: No such Context exists: " + src);

        binary = platform.system() + "/" + platform.architecture()[0] + "/xsDisplayContext"
        subprocess.call([binary, '-f', dest])

        if platform.system() == 'Windows':
            sys.stdout.flush()
            time.sleep(0.5)
            
    except Exception, e:
        si.generateErrorResults(e)
