
if test -z $SPIDER_MONKEY
then
export SPIDER_MONKEY=/home/gmbecker/gabe/checkedout/FirefoxSource192/js/src
fi

if test -z $R_HOME
then
export R_HOME=/home/gmbecker/Downloads/R-2.12.0
fi

if test -z $LD_ADDED
then
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$R_HOME/lib:$SPIDERMONKEY
export LD_ADDED=1
fi

export G_SLICE=always-malloc

#rm /home/gmbecker/.mozilla/firefox/n4sv79k8.dev/compreg.dat
#rm /home/gmbecker/.mozilla/firefox/n4sv79k8.dev/xpti.d
 /home/gmbecker/local/ff-valgrind/dist/bin/firefox  --no-remote -P

