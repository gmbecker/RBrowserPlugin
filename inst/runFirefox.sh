FF=/home/gmbecker/local/firefox5.0-debug/bin/firefox
FFOPTS="-g --no-remote -P dev"

if test -z "$R_HOME"
then
export R_HOME=/usr/lib64/R
fi

if test -z "$LD_ADDED"
then
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$R_HOME/lib
export LD_ADDED=1
fi

$FF $FFOPTS

