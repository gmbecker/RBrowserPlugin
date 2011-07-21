
library(RGtk2)
library(RFirefox)

initGtkMozEmbed()
mozemb = gtkMozNewBrowser()
gtkMozLoadURL(mozemb, "www.mozilla.org")
mynsiwb = gtkMozGetnsIWB(mozemb)
myctx = .Call("R_get_nsIScriptContext", mynsiwb)
evaluateJSInBrowser(myctx, "var x=5;")
evaluateJSInBrowser(myctx, "var x;alert(x);")
#the value persists but the variable pointing to it needs to be
#redefined in the new script (equivalent of a new script tag)
#this is odd but seems to just be how javascript works.

gtkMozLoadURL(mozemb, paste("file://",
                            system.file("testdev.html",
                                        package="RFirefox")
                            ))

mypv = readLines("tests/protovistest2.js")
myjs = paste("var div = document.getElementById('device2');div.innerHTML=<iframe src='", paste(getwd(), "/linkedSmoother.svg", sep=""), "/>';", sep="")
gtkMozLoadURL(mozemb, paste(getwd(), "/linkedSmoother.svg", sep=""))
evaluateJSInBrowser(myctx, myjs)
