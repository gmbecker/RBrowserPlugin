if(FALSE)
{
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




raphaelCDev = function(id = "raph_content", dim = c(600, 400), storage = new.env())
  {
    assign("points", list(), envir=storage)
    assign("lines", list(), envir=storage)
    assign("rects", list(), envir=storage)
    assign("polylines", list(), envir=storage)
    assign("texts", list(), envir=storage)

    script = paste("Raphael('", id, "',", dim[1], " , ", dim[2], ");", sep="")

    tmp = evalJavaScript(script = script, keepResult = TRUE)
    assign("paper", tmp, env = storage)

    .Call("R_GD_raphaelDevice", storage, PluginInstance)

    list(getPoints = function() get("points", storage),
         getLines = function() get("lines", storage),
         getPolyLines = function() get("polylines", storage),
         getTexts = function() get("texts", storage),
         storage = storage
         )
  }


raphaelDev = function(id = "raph_content", dim = c(600, 400),
  col = "black", fill = "transparent", ps=10, wrapup=NULL, jscon = ScriptCon, storage = new.env(), allocSize = 5000 )
  {
    print("raphaelDev")
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
      retval = storage$paper$circle(x,y,r)
      doColors(retval, context)
      assign("points", c(get("points", storage), retval), envir = storage)
      TRUE
    }
    funs@rect = function(x1, y1, x2, y2, context, dev)
      {
        print(paste("x1: ", x1, " x2: ", x2, " y1: ", y1, " y2: ", y2))
        if(x1 > x2)
          {
            tmp = x1
            x1 = x2
            x2 = tmp
          }
        if(y1 > y2)
          {
            tmp = y1
            y1 = y2
            y2 = tmp
          }
        retval = storage$paper$rect(x1, y1, x2-x1, y2 - y1)
        doColors(retval, context)
        assign("rects", c(get("rects", storage), retval), envir = storage)
        TRUE
      }
    funs@line = function(x1, y1, x2, y2, context, dev)
      {
        path = paste("M", x1, y1, "l", x2-x1, y2 - y1)
        retval = storage$paper$path(path)
        doColors(retval, context)
        assign("lines", c(get("lines", storage), retval), envir = storage)
        TRUE
      }

    funs@text = function(x, y, txt, rot, hadj, context, dev)
      {
        print(paste("In text. txt is: ", txt))
        retval = storage$paper$text(x, y, txt)
        size = as.integer(max(10, context$ps) * context$cex)
        retval$attr("font-size", size)
        doColors(retval, context)
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

    tmp = evalJavaScript(script = script, keepResult = TRUE)
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
        retval = storage$paper$path(path)
        doColors(retval, context)

        assign("polylines", c(get("polylines", storage), retval), envir = storage)

        return(TRUE)
      }
  #}

    funs@newPage = function(context, devdesc)
      {
        print("In raphaelDev::newPage")
        storage$paper$clear()
        storage$lines = list()
        storage$points = list()
        storage$polylines = list()
        storage$text = list()
        storage$rects = list()
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
    print(paste("\nIn raphaelDev. dev.cur():", dev.cur()))
    assign("devnum", dev.cur(), storage)
    list(getPoints = function() get("points", storage),
         getLines = function() get("lines", storage),
         getPolyLines = function() get("polylines", storage),
         getTexts = function() get("texts", storage),
         storage = storage,
         devnum =  get("devnum", storage)
         )
  }


if(FALSE)
  {
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

}

doColors = function(jsvar, context)
  {
    stroke = as(context$col, "RGB")
    fill = as(context$fill, "RGB")
    if(!isTransparent(stroke))
      jsvar$attr("stroke", stroke)
    if(!isTransparent(fill))
      jsvar$attr("fill", fill)
   TRUE
  }

getContextColors = function(context)
  {
    col = as(context$col, "RGB")
    fill = as(context$fill, "RGB")
    ret = list(stroke = NULL, fill = NULL)
    if(!isTransparent(col))
      ret$stroke = col
    else
      warning("transparent colors not currently supported.")
    if(!isTransparent(fill))
      ret$fill = fill
    else
      warning("transparent colors not currently supported.")

    ret
  }


raphRemoveAll = function(type="points", dev)
  {
    match.arg(type, c("points", "lines", "texts", "polylines"))
    switch(type,
           points = .Call("R_NPAPI_Remove_All", PluginInstance, dev$getPoints()),
           lines = .Call("R_NPAPI_Remove_All", PluginInstance, dev$getLines()),
           texts = .Call("R_NPAPI_Remove_All", PluginInstance, dev$getTexts()),
           polylines = .Call("R_NPAPI_Remove_All", PluginInstance, dev$getPolyLines())
           )
    assign(type, list(), dev$storage)
  }

raphRemoveElements = function(type="points", indexes, dev)
{
   match.arg(type, c("points", "lines", "texts", "polylines"))
  sapply(indexes, function(i, lst) lst[[i]]$remove(), lst = get(type, dev$storage))

  #remove chosen elements from the list of currently drawn elements
  assign(type, get(type, dev$storage)[-indexes], dev$storage)

}
}
