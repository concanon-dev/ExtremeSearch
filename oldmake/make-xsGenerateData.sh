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

$gcc -o bin/$Platform/xsGenerateData -Wall obj/$Platform/xsGenerateData.o obj/$Platform/saDice.o obj/$Platform/saCSV.o obj/$Platform/saDoubleField.o obj/$Platform/saFloatField.o obj/$Platform/saField.o obj/$Platform/saIntField.o obj/$Platform/saOutput.o obj/$Platform/saProperties.o obj/$Platform/saStringField.o obj/$Platform/saTimeField.o obj/$Platform/saSignal.o obj/$Platform/saLicensing.o  $LicenseLib -lm
