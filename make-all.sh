if [ "$OSTYPE" = "cygwin" ]; then 
  if [ "$1" = "x86" ]; then
    Platform='Win/x86'
    LicenseFile='LimeLM/Win/x86/TurboActivate.dll'
  else
    Platform='Win/x64'
    LicenseFile='LimeLM/Win/x64/TurboActivate.dll'
  fi
elif [[ $OSTYPE == darwin1? ]]; then
  Platform='Mac'
  LicenseFile='LimeLM/Mac/libTurboActivate.dylib'
else
  Platform='Linux/x64'
  LicenseFile='LimeLM/Linux/x64/libTurboActivate.so'
fi

if [ ! -d bin/$Platform ]; then
  mkdir -p bin/$Platform
fi

cp $LicenseFile bin/$Platform

if [ "$OSTYPE" = "cygwin" ]; then 
  if [ "$1" = "x86" ]; then
    Platform='Win/x86'
    cp LimeLM/Win/x86/TurboActivate.dll bin/$Platform
  else
    Platform='Win/x64'
    cp LimeLM/Win/x64/TurboActivate.dll bin/$Platform
  fi
elif [[ $OSTYPE == darwin1? ]]; then
  Platform='Mac'
  cp LimeLM/Mac/libTurboActivate.dylib bin/$Platform
else
  Platform='Linux/x64'
  cp LimeLM/Linux/x64/libTurboActivate.so bin/$Platform
fi

./make-xsCloneContext.sh
./make-xsCloneSemanticTerm.sh
./make-xsCreateContext.sh
./make-xsDeleteContext.sh
./make-xsDisplayContext.sh
./make-xsDisplaySemanticTerm.sh


cp LimeLM/TurboActivate.dat bin/$Platform
