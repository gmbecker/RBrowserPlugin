FF=/Applications/Firefox6.app/Contents/MacOS/firefox-bin
IS_MAC=1
FFOPTS="--no-remote -P dev"
ARCH=x86_64

if test -z "$R_HOME"
then
export R_HOME=/Library/Frameworks/R.framework/Resources
fi

if [ "$IS_MAC" -eq "1" ];
then
FFDEBUG=""
R_LIB_LOC=$R_HOME/lib/$ARCH
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$R_LIB_LOC
else
FFDEBUG="-g"
R_LIB_LOC=$R_HOME/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$R_LIB_LOC
FFOPTS="--no-remote $FFOPTS"
fi

echo "Hi there!"


$FF $FFDEBUG $FFOPTS
