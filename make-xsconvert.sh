#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
if [ "$OSTYPE" = "cygwin" ]; then 
  gcc='x86_64-w64-mingw32-gcc -D _UNICODE'
  Platform='Win/x64'
elif [[ $OSTYPE == darwin1? ]]; then
  gcc=gcc
  Platform='Mac'
else
  gcc=gcc
  Platform='Linux/x64'
fi

$gcc -o python/xsconvert -Wall -O2 -lm obj/$Platform/xsconvert.o obj/$Platform/saContext.o obj/$Platform/saContextCreate.o obj/$Platform/saConcept.o  obj/$Platform/OLDsaLoadContext.o obj/$Platform/saOpenFile.o obj/$Platform/saSplunk.o obj/$Platform/saCSV.o 
