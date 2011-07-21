library(RFirefox)
#library(SpiderMonkey)
print("file has been sourced")


changeData = function(dset)
  {
    myxvar <<- availx[,as.numeric(dset)]
    myxname <<- availnames[ as.numeric(dset)]
    xrange <<- range(myxvar)
    cascadeChanges(1)
  }



cascadeChanges = function(pos)
  {
    
    if (pos == 1)
      doMainPlot(lamda)

    if (pos == 2)
      doResidPlot()
  }

doMainPlot = function(lam)
  {
    if (lam != lamda)
      lamda <<- lam

    xs = c( unique( myxvar ) , seq( min( myxvar ) - .1 , max( myxvar ) + .1 , length.out = 100 ) )
    xrange = max(myxvar) - min(myxvar)
    band = lam / 500 * xrange / 5
    set_JS_Property( ScriptCon , mydiv , "innerHTML" , paste("Bandwidth: ",  round( band , digits = 2 ) ) )
    fit <<- ksmooth(myxvar, mtcars$mpg, band = band, x.points = xs)

    dev.set(maindev)
    clearRaphPlot(dev1)
    plot(myxvar, mtcars$mpg, main="Kernel Smoother", ylab = "mpg", xlab = myxname)
    lines(fit)
    #dev1 <<- raphaelDev("mainplot")
    cascadeChanges(2)
  }

doResidPlot = function(dummy = 1)
{
  dev.set(resdev)
  clearRaphPlot(dev2)
  xinds = sapply(myxvar, function(x) which(x==fit$x)[1])
  yhat = fit$y[ xinds ]
  res = mtcars$mpg - yhat
  plot(myxvar, res, main = "Residuals", xlab=myxname)
}

availnames = c("disp", "hp", "drat", "qsec")
availx = mtcars[, availnames]

#mydiv = jsVal()
#scr = "document.getElementById('dispval');"  
#JS_EvaluateScript(ScriptCon, JS_GetGlobalObject(ScriptCon, returnInputs = FALSE), scr, nchar(scr), "getdiv", 1, mydiv)
#JS_AddRoot(ScriptCon, mydiv)
mydiv = getElementById(ScriptCon, "dispval")
dev1 <- raphaelDev("mainplot", dim = c( 450 , 300 ) )
maindev = dev.cur()
dev2 <- raphaelDev("residplot", dim = c( 450, 300 ) ) 
resdev = dev.cur()
print( paste( "maindev" , maindev ) )
print( paste( "resdev" , resdev ) )
lamda = 50
fit = list()
changeData(1)
