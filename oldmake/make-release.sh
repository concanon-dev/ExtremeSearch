#
# (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
# Reproduction or unauthorized use is prohibited. Unauthorized
# use is illegal. Violators will be prosecuted. This software 
# contains proprietary trade and business secrets.            
#
if [ "$1" == "" ]; then
  echo "Usage: make-release x_y_z wyday|expiretime, where x_y_z is the current version and wyday|expiretime is the license_scheme"
  echo Example: make-release 3_0_0
  exit
fi

if [ "$2" == "" ]; then
  echo "Missing argument for LICENSE TYPE.  You must specify wyday or expiretime."
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
mkdir -p release/xtreme/datagen

# Compile and build
echo "Compiling (calling gcc-all.sh)"
./gcc-all.sh $2
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
cp -r datagen/* release/xtreme/datagen

# Set the LTYPE for public consumption
if [ "$2" == "wyday" ]; then
    LTYPE="p"
fi
if [ "$2" == "expiretime" ]; then
    LTYPE="s"
fi

# copy the right png file for the overall UI
cp pngfiles/ess_400_${LTYPE}.png release/xtreme/appserver/static/images/ess_400.png

cd release
if [ "$OSTYPE" = "cygwin" ]; then 
  tar -cvzf ess_win_$1_${LTYPE}.tgz xtreme/*
elif [[ $OSTYPE == darwin1? ]]; then
  ln -s 64bit xtreme/bin/Darwin/32bit
  tar -cvzf ess_mac_$1_${LTYPE}.tgz xtreme/*
else
  tar -cvzf ess_linux_$1_${LTYPE}.tgz xtreme/*
fi

