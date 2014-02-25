if [ "$OSTYPE" = "cygwin" ]; then 
  if [ "$1" = "x86" ]; then
    GCC='i686-w64-mingw32-gcc.exe -D _UNICODE'
    Platform='Win/x86'
  else
    GCC='x86_64-w64-mingw32-gcc -D _UNICODE'
    Platform='Win/x64'
  fi
  echo "GCC="$GCC
elif [[ $OSTYPE == darwin1? ]]; then
  GCC="gcc -g"
  Platform='Mac'
else
  GCC="gcc -g"
  Platform='Linux/x64'
fi

if [ ! -d obj/$Platform ]; then
  mkdir -p obj/$Platform
fi

$GCC -c OLDsaLoadContext.c               -o obj/$Platform/OLDsaLoadContext.o
$GCC -c fix-intersplunk.c                -o obj/$Platform/fix-intersplunk.o
$GCC -c saAutoRegression.c               -o obj/$Platform/saAutoRegression.o
$GCC -c saContext.c                      -o obj/$Platform/saContext.o
$GCC -c saContextCreate.c                -o obj/$Platform/saContextCreate.o
$GCC -c saCSV.c                          -o obj/$Platform/saCSV.o
$GCC -c saDebug.c                        -o obj/$Platform/saDebug.o
$GCC -c saGeoLiteCity.c                  -o obj/$Platform/saGeoLiteCity.o
$GCC -c saHash.c                         -o obj/$Platform/saHash.o
$GCC -c saHedge.c                        -o obj/$Platform/saHedge.o
$GCC -c saInsertUniqueValue.c            -o obj/$Platform/saInsertUniqueValue.o
$GCC -c saLicenseMain.c                  -o obj/$Platform/saLicenseMain.o
$GCC -c saLicensing.c                    -o obj/$Platform/saLicensing.o
$GCC -c saLinearCorrelation.c            -o obj/$Platform/saLinearCorrelation.o
$GCC -c saLinearRegression.c             -o obj/$Platform/saLinearRegression.o
$GCC -c saListDir.c                      -o obj/$Platform/saListDir.o
$GCC -c saMatrixArgs.c                   -o obj/$Platform/saMatrixArgs.o
$GCC -c saOpenFile.c                     -o obj/$Platform/saOpenFile.o
$GCC -c saParser.c                       -o obj/$Platform/saParser.o
$GCC -c saQuadRegression.c               -o obj/$Platform/saQuadRegression.o
$GCC -c saSignal.c                       -o obj/$Platform/saSignal.o
$GCC -c saSemanticTerm.c                 -o obj/$Platform/saSemanticTerm.o
$GCC -c saSplunk.c                       -o obj/$Platform/saSplunk.o
$GCC -c saSpearmanCorrelation.c          -o obj/$Platform/saSpearmanCorrelation.o
$GCC -c saStatistics.c                   -o obj/$Platform/saStatistics.o
$GCC -c test.c                           -o obj/$Platform/test.o
$GCC -c test2.c                          -o obj/$Platform/test2.o
$GCC -c xsAggregateAutoRegression.c      -o obj/$Platform/xsAggregateAutoRegression.o
$GCC -c xsAggregateCorrelation.c         -o obj/$Platform/xsAggregateCorrelation.o
$GCC -c xsAggregateLinearRegression.c    -o obj/$Platform/xsAggregateLinearRegression.o
$GCC -c xsAggregateQuadRegression.c      -o obj/$Platform/xsAggregateQuadRegression.o
$GCC -c xsAggregateSpearmanCorrelation.c -o obj/$Platform/xsAggregateSpearmanCorrelation.o
$GCC -c xsApplyAutoRegression.c          -o obj/$Platform/xsApplyAutoRegression.o
$GCC -c xsApplyAutoRegressionFromFile.c  -o obj/$Platform/xsApplyAutoRegressionFromFile.o
$GCC -c xsApplyLinearRegression.c        -o obj/$Platform/xsApplyLinearRegression.o
$GCC -c xsApplyLinearRegressionFromFile.c -o obj/$Platform/xsApplyLinearRegressionFromFile.o
$GCC -c xsApplyQuadRegression.c          -o obj/$Platform/xsApplyQuadRegression.o
$GCC -c xsApplyQuadRegressionFromFile.c  -o obj/$Platform/xsApplyQuadRegressionFromFile.o
$GCC -c xsCloneContext.c                 -o obj/$Platform/xsCloneContext.o
$GCC -c xsCloneSemanticTerm.c            -o obj/$Platform/xsCloneSemanticTerm.o
$GCC -c xsconvert.c                      -o obj/$Platform/xsconvert.o
$GCC -c xsCreateContext.c                -o obj/$Platform/xsCreateContext.o
$GCC -c xsCreateSemanticTerm.c           -o obj/$Platform/xsCreateSemanticTerm.o
$GCC -c xsDeleteContext.c                -o obj/$Platform/xsDeleteContext.o
$GCC -c xsDeleteSemanticTerm.c           -o obj/$Platform/xsDeleteSemanticTerm.o
$GCC -c xsDiscoverTrend.c                -o obj/$Platform/xsDiscoverTrend.o
$GCC -c xsDisplayContext.c               -o obj/$Platform/xsDisplayContext.o
$GCC -c xsDisplaySemanticTerm.c          -o obj/$Platform/xsDisplaySemanticTerm.o
$GCC -c xsDisplayWhere.c                 -o obj/$Platform/xsDisplayWhere.o
$GCC -c xsFindBestSemanticTerm.c         -o obj/$Platform/xsFindBestSemanticTerm.o
$GCC -c xsFindMembership.c               -o obj/$Platform/xsFindMembership.o
$GCC -c xsGetCompatibility.c             -o obj/$Platform/xsGetCompatibility.o
$GCC -c xsGetDistance.c                  -o obj/$Platform/xsGetDistance.o
$GCC -c xsLicense.c                      -o obj/$Platform/xsLicense.o
$GCC -c xsListContexts.c                 -o obj/$Platform/xsListContexts.o
$GCC -c xsListSemanticTerms.c            -o obj/$Platform/xsListSemanticTerms.o
$GCC -c xsOverlayContext.c               -o obj/$Platform/xsOverlayContext.o
$GCC -c xsPerformAutoRegression.c        -o obj/$Platform/xsPerformAutoRegression.o
$GCC -c xsPerformCorrelation.c           -o obj/$Platform/xsPerformCorrelation.o
$GCC -c xsPerformLinearRegression.c      -o obj/$Platform/xsPerformLinearRegression.o
$GCC -c xsPerformQuadRegression.c        -o obj/$Platform/xsPerformQuadRegression.o
$GCC -c xsPerformSpearmanCorrelation.c   -o obj/$Platform/xsPerformSpearmanCorrelation.o
# $GCC -c xsUpdateContext.c                -o obj/$Platform/xsUpdateContext.o
$GCC -c xsWhere.c                        -o obj/$Platform/xsWhere.o
$GCC -c xspreautoregress.c               -o obj/$Platform/xspreautoregress.o
$GCC -c xsprecorrelate.c                 -o obj/$Platform/xsprecorrelate.o
$GCC -c xspredict.c                      -o obj/$Platform/xspredict.o
$GCC -c xsprelicense.c                   -o obj/$Platform/xsprelicense.o
$GCC -c xsprequadregress.c               -o obj/$Platform/xsprequadregress.o
$GCC -c xspreregress.c                   -o obj/$Platform/xspreregress.o
$GCC -c xsprespearmancorrelate.c         -o obj/$Platform/xsprespearmancorrelate.o
$GCC -c xsrepredict.c                    -o obj/$Platform/xsrepredict.o

if [ "$OSTYPE" = "cygwin" ]; then 
$GCC -c strsep.c                         -o obj/$Platform/strsep.o
fi
