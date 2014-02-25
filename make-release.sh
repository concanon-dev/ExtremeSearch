#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
if [ -z $1 ]; then
  echo Usage: make-release x_y_z [hybrid], where x_y_z is the current version
  echo Example: make-release 3_0_0
  echo if hybrid is specified, it assumes the binaries are up to date for all platforms under release/xtreme/bin
  exit
fi

if [ "$OSTYPE" = "cygwin" ]; then 
  SourceDir='Win/x64'
  Platform='Windows'
elif [[ $OSTYPE == darwin1? ]]; then
  SourceDir='Mac'
  Platform='Darwin'
else
  SourceDir='Linux/x64'
  Platform='Linux'
fi
         
if [ "$2" != "hybrid" ]; then
  # Clean up any old files
  rm -rf release/xtreme
  rm obj/$SourceDir/*

  # Make empty tree structure
  mkdir -p release/xtreme/appserver/static
  mkdir -p release/xtreme/static
  mkdir -p release/xtreme/bin/$Platform/64bit
  mkdir -p release/xtreme/default/data/ui/nav
  mkdir -p release/xtreme/default/data/ui/views
  # mkdir -p release/xtreme/docs
  mkdir -p release/xtreme/lookups
  mkdir -p release/xtreme/metadata
  mkdir -p release/xtreme/contexts

  # Compile and build
  echo "Compiling (calling gcc-all.sh)"
  ./gcc-all.sh
  echo "Linking (calling make-all.sh)"
  ./make-all.sh

  # Fix version # in app.conf
  grep -v "^version=" default/app.conf > xxx
  newVersion=`echo $1 | sed -e 's/_/./g'`
  echo "version=$newVersion" >> xxx
  mv xxx default/app.conf
  echo "Updating version to: " $newVersion

  # Copy files
  cp -r appserver/* release/xtreme/appserver/
  cp -r static/* release/xtreme/static/
  cp version.info release/xtreme/
  cp bin/$SourceDir/* release/xtreme/bin/$Platform/64bit/
  cp -r default/* release/xtreme/default/
  # cp -r docs/* release/xtreme/docs/
  cp -r lookups/* release/xtreme/lookups/
  cp -r metadata/* release/xtreme/metadata/
  cp python/* release/xtreme/bin/
  cp contexts/* release/xtreme/contexts
fi

# Create tar files
cd release
if [ "$2" = "hybrid" ]; then
  tar -cvzf ess_hybrid_$1.tgz xtreme/*
elif [ "$OSTYPE" = "cygwin" ]; then 
  tar -cvzf ess_win_$1.tgz xtreme/*
elif [[ $OSTYPE == darwin1? ]]; then
  ln -s 64bit xtreme/bin/Darwin/32bit
  tar -cvzf ess_mac_$1.tgz xtreme/*
else
  tar -cvzf ess_linux_$1.tgz xtreme/*
fi
