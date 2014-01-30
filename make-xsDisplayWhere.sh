#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
if [ "$OSTYPE" = "cygwin" ]; then 
  gcc='x86_64-w64-mingw32-gcc -D _UNICODE'
  Platform='Win/x64'
  LicenseLib=LimeLM/Win/x64/TurboActivate.lib 
elif [[ $OSTYPE == darwin1? ]]; then
  gcc=gcc
  Platform='Mac'
  LicenseLib='-Wl,-rpath,@executable_path/. -LLimeLM/Mac -lTurboActivate'
else
  gcc=gcc
  Platform='Linux/x64'
  LicenseLib='-Wl,-R,$ORIGIN -LLimeLM/Linux/x64 -lTurboActivate'
fi

$gcc -o bin/$Platform/xsDisplayWhere -Wall -O2 -lm obj/$Platform/xsDisplayWhere.o obj/$Platform/saHedge.o obj/$Platform/saParser.o obj/$Platform/saSemanticTerm.o obj/$Platform/saContext.o obj/$Platform/saCSV.o obj/$Platform/saSignal.o obj/$Platform/saOpenFile.o obj/$Platform/saHash.o obj/$Platform/saSplunk.o obj/$Platform/saLicensing.o $LicenseLib
