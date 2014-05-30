#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
if [ "$OSTYPE" = "cygwin" ]; then 
  if [ "$1" = "x86" ]; then
    gcc='i686-w64-mingw32-gcc.exe -D _UNICODE'
    Platform='Win/x86'
    LicenseLib='LimeLM/Win/x86/TurboActivate.lib -lws2_32'
  else
    gcc='x86_64-w64-mingw32-gcc -D _UNICODE'
    Platform='Win/x64'
    LicenseLib='LimeLM/Win/x64/TurboActivate.lib -lws2_32'
  fi
elif [[ $OSTYPE == darwin1? ]]; then
  gcc=gcc
  Platform='Mac'
  LicenseLib='-Wl,-rpath,@executable_path/. -LLimeLM/Mac -lTurboActivate'
else
  gcc=gcc
  Platform='Linux/x64'
  LicenseLib='-Wl,-R,$ORIGIN -LLimeLM/Linux/x64 -lTurboActivate'
fi

$gcc -o bin/$Platform/xsprelicense -Wall -O2 -lm obj/$Platform/xsprelicense.o obj/$Platform/saLicenseMain.o obj/$Platform/saSignal.o obj/$Platform/saLicensing.o $LicenseLib

