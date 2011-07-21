
if test -z $SPIDER_MONKEY
then
#export SPIDER_MONKEY=/home/gmbecker/gabe/checkedout/FirefoxSource192/js/src
export SPIDER_MONKEY=/home/gmbecker/local/spidermonkey-dbg/dist/lib
fi

if test -z "$R_HOME"
then
#export R_HOME=/home/gmbecker/local/lib64/R
#export R_HOME=/home/gmbecker/Downloads/R-2.12.0
export R_HOME=/usr/lib64/R
fi

if test -z "$LD_ADDED"
then
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$R_HOME/lib:$SPIDERMONKEY
export LD_ADDED=1
fi

export G_SLICE=always-malloc

#rm /home/gmbecker/.mozilla/firefox/n4sv79k8.dev/compreg.dat
#rm /home/gmbecker/.mozilla/firefox/n4sv79k8.dev/xpti.dat


/home/gmbecker/local/valgrind/bin/valgrind --smc-check=all --leak-check=yes /home/gmbecker/local/ff-valgrind/dist/bin/firefox -g  --no-remote -P dev

