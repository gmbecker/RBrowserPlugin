setClass("RJavaScriptCanvasMethods", contains = "RDevDescMethods")

setClass("QueryableDevDesc", contains = "DevDesc",
         representation = list(getPoints = "function", getLines = "function",
           getPolyLines = "function", addPoint = "function", addLine = "function",
           addPolyLine = "function")
         )
setClass("QueryableDevDescPtr", contains = "DevDescPtr",
         representation = list(getPoints = "function", getLines = "function",
           getPolyLines = "function", addPoint = "function", addLine = "function",
           addPolyLine = "function")
         )

setAs("DevDesc", "QueryableDevDesc",
      function(from)
      {
        toRet = new("QueryableDevDesc")
        sapply(slotNames(from), function(nm)
               slot(toRet, nm) <- slot(from, nm))
        toRet
      })

setAs("DevDescPtr", "QueryableDevDescPtr",
      function(from)
      {
        toRet = new("QueryableDevDescPtr")
        sapply(slotNames(from), function(nm)
               slot(toRet, nm) <- slot(from, nm))
        toRet
      })
        



raphaelDev = function(id = "raph_content", dim = c(600, 400),
  col = "black", fill = "transparent", ps=10, wrapup=NULL, jscon = ScriptCon, storage = new.env() )
  {
    library(RGraphicsDevice)

    assign("points", list(), envir=storage)
    assign("lines", list(), envir=storage)
    assign("rects", list(), envir=storage)
    assign("polylines", list(), envir=storage)
    assign("texts", list(), envir=storage)
    
    funs = as(dummyDevice(), "RJavaScriptCanvasMethods")
    
                                        # circle, line, rect
                                        # text
                                        # polygon, polyline
                                        # strWidth, metricInfo, strWidthUTF8,

                                        # size
                                        # newPage
    

                                      
    funs@circle = function(x, y, r, context, dev) {
      #retval = jsVal()
      cnum = length(get("points", storage))
      script = paste("var paper; paper.circle(", x , "," , y , ",", r, ");", sep="")
      #JS_EvaluateScript(jscon, JS_GetGlobalObject(jscon, returnInputs=FALSE),
       #                 script, nchar(script), "circle", 1, retval)
      retval = call_JS_Method(jscon, get("paper", env = storage),
        "circle", list( x , y , r ))
      #JS_AddRoot(jscon, retval) Already AddRooted
      #XXX finalizer
      assign("points", c(get("points", storage), retval), envir = storage)
      TRUE
    }
    funs@rect = function(x1, y1, x2, y2, context, dev)
      {
        retval = call_JS_Method(jscon, get("paper", env = storage), "rect", list( x2 , y2 , x2 - x1 , y2 - y1 ))
        assign("rects", c(get("rects", storage), retval), envir = storage)
        TRUE
      }
    funs@line = function(x1, y1, x2, y2, context, dev)
      {
        path = paste("M", x1, y1, "l", x2-x1, y2 - y1)
        retval = call_JS_Method(jscon , get("paper", env = storage ) , "path", path)
        assign("lines", c(get("lines", storage), retval), envir = storage)
        TRUE
      }

    funs@text = function(x, y, txt, rot, hadj, context, dev)
      {
        
        retval = call_JS_Method( jscon , get( "paper" , env = storage ) , "text" , list( x , y , txt ) )
        
        size = as.integer(max(10, context$ps) * context$cex)
        call_JS_Method(jscon, retval, "attr", list("font-size", size), addRoot = FALSE)
        assign("texts", c(get("texts", storage), retval), storage)
        TRUE
      }
    funs@initDevice = function(dev) {
       # The all important parameter to set ipr to get the plot region with adequate margins
    dev$ipr = rep(1/72.27, 2)
    dev$cra = rep(c(6, 13)/12) * 10
    dev$startps = 10
    dev$canClip = TRUE
    dev$canChangeGamma = TRUE
    dev$startgamma = 1
    dev$startcol = as("black", "RGBInt")

    script = paste("Raphael('", id, "',", dim[1], " , ", dim[2], ");", sep="")
    tmp = jsVal()
    JS_EvaluateScript(jscon, JS_GetGlobalObject(jscon, returnInputs=FALSE),
                      script, nchar(script), "Raphael init", 1, tmp)
    JS_AddRoot(jscon, tmp)
    assign("paper", tmp, env = storage)
    return(TRUE)
  }
    funs@polyline = function(n, x, y, context, dev)
      {
        
        x = x[1:n]
        y = y[1:n]
        #more storage but much faster
        diffsx = x[ 2 : n ] - x[ -n ]
        diffsy = y[ 2 : n ] - y[ -n ] 

        path = paste( paste( "M" , x[1], y[1]),
          paste( "l" , diffsx , diffsy , collapse = " "),
          collapse = " ")
        retval = call_JS_Method(jscon, get("paper", env=storage),
          "path", path)
        assign("polylines", c(get("polylines", storage), retval), envir = storage)
        return(TRUE)
      }
    # No implementations for
    funs@mode = NULL
    funs@metricInfo = NULL
    funs@activate = NULL
    funs@deactivate = NULL
    funs@locator = NULL
    funs@onExit = NULL
    
    dev = graphicsDevice(funs, dim, col, fill, ps)

    dev$ipr = rep(1/72.27, 2)
    dev$cra = rep(c(6, 13)/12) * 10
    #dev = as(dev, "QueryableDevDescPtr")
    #dev@getPoints = function() points
    #don't really care about the DevDesc. may extend it later, for now I return a list with what I want in it
    #print(class(dev))
    #dev
    list(getPoints = function() get("points", storage),
         getLines = function() get("lines", storage),
         getPolyLines = function() get("polylines", storage),
         getTexts = function() get("texts", storage),
         storage = storage
         )
  }

  
clearRaphPlot = function(dev)
  {
    rem = function(x)
      {
        call_JS_Method(ScriptCon, x, "remove", list(), addRoot = FALSE)
        #JS_RemoveRoot(ScriptCon, tmp)
        #JS_RemoveRoot(ScriptCon, x) Finalizer should take care of this.
      }

    sapply(dev$getPoints(), rem)
    sapply(dev$getLines(), rem)
    sapply(dev$getPolyLines(), rem)
    sapply(dev$getTexts(), rem)
    assign("points", list(), env = dev$storage)
    assign("lines", list(), env = dev$storage)
    assign("polylines", list(), env = dev$storage)
    assign("texts", list(), env = dev$storage)
  }
