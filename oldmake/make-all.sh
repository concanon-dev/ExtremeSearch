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

cp LimeLM/TurboActivate.dat bin/$Platform

./make-xsAggregateAutoRegression.sh
./make-xsAggregateCorrelation.sh
./make-xsAggregateLinearRegression.sh
./make-xsAggregateQuadRegression.sh
./make-xsAggregateSpearmanCorrelation.sh
./make-xsApplyAutoRegression.sh
./make-xsApplyAutoRegressionFromFile.sh
./make-xsApplyLinearRegression.sh
./make-xsApplyLinearRegressionFromFile.sh
./make-xsApplyQuadRegression.sh
./make-xsApplyQuadRegressionFromFile.sh
./make-xsCloneContext.sh
./make-xsCloneConcept.sh
./make-xsCreateContext.sh
./make-xsCreateConcept.sh
./make-xsDeleteContext.sh
./make-xsDeleteConcept.sh
./make-xsDiscoverTrend.sh
./make-xsDisplayContext.sh
./make-xsDisplayConcept.sh
./make-xsDisplayWhere.sh
./make-xsFindBestConcept.sh
./make-xsFindMembership.sh
if [ "$OSTYPE" != "cygwin" ]; then
./make-xsGenerateData.sh
fi
./make-xsGetCompatibility.sh
./make-xsGetDistance.sh
./make-xsGetWhereCix.sh
./make-xsLicense.sh
./make-xsListContexts.sh
./make-xsListConcepts.sh
./make-xsListUOM.sh
./make-xsOverlayContext.sh
./make-xsPerformAutoRegression.sh
./make-xsPerformCorrelation.sh
./make-xsPerformLinearRegression.sh
./make-xsPerformQuadRegression.sh
./make-xsPerformSpearmanCorrelation.sh
# ./make-xsUpdateContext.sh
./make-xsWhere.sh
./make-xspreautoregress.sh
./make-xsprecorrelate.sh
./make-xspredict.sh
./make-xsprelicense.sh
./make-xsprequadregress.sh
./make-xspreregress.sh
./make-xsprespearmancorrelate.sh
./make-xsrepredict.sh
