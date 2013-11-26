if [ "$OSTYPE" = "cygwin" ]; then 
  GCC='x86_64-w64-mingw32-gcc -D _UNICODE'
  echo "$GCC="$GCC
  Platform='Win/x64'
elif [[ $OSTYPE == darwin1? ]]; then
  GCC="gcc -g"
  Platform='Mac'
else
  GCC="gcc -g"
  Platform='Linux/x64'
fi

F=`echo $1 | sed -e 's/\.c//g'`
$GCC -c ${F}.c  -o obj/$Platform/${1}.o
