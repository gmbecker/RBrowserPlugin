
raphaelCDev = function(id = "raph_content", dim = c(400, 400), storage = new.env())
  {
    assign("points", list(), envir=storage)
    assign("lines", list(), envir=storage)
    assign("rects", list(), envir=storage)
    assign("polylines", list(), envir=storage)
    assign("texts", list(), envir=storage)

    script = paste("Raphael('", id, "',", dim[1], " , ", dim[2], ");", sep="")
    print("Raphael C device attempting to create paper")
    tmp = evalJavaScript(script = script, keepResult = TRUE, convertRet = function(x) as(x, "JSPaperRef"))
    assign("paper", tmp, env = storage)
    print("Raphael C device paper created")
    .Call("R_GD_raphaelDevice", storage, PluginInstance, as.integer(dim))
    assign("devnum", dev.cur(), storage)
    list(getPoints = function() get("points", storage),
         getLines = function() get("lines", storage),
         getPolyLines = function() get("polylines", storage),
         getTexts = function() get("texts", storage),
         getRects = function() get("rects", storage),
         devnum = storage$devnum,
         storage = storage
         )
  }

raphNewPage = function(env)
  {
    pap = env$paper
    pap$clear()
    env$points = list()
    env$lines = list()
    env$polylines = list()
    env$texts = list()
    TRUE
  }

doColors = function(jsvar, context)
  {
    stroke = as(context$col, "RGB")
    fill = as(context$fill, "RGB")

    if(!isTransparent(stroke))
      jsvar$attr("stroke", getColorHex(stroke))
    if(!isTransparent(fill))
      jsvar$attr("fill", getColorHex(fill))
   TRUE
  }

getColorHex = function(col)
  {
    paste(c("#", paste(as.character.hexmode(col2rgb( col ) ), collapse="") ), collapse = "")
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
    match.arg(type, c("points", "lines", "texts", "polylines", "rects", "polygons"))
    switch(type,
           points = .Call("R_NPAPI_Remove_All", PluginInstance, dev$getPoints()),
           lines = .Call("R_NPAPI_Remove_All", PluginInstance, dev$getLines()),
           texts = .Call("R_NPAPI_Remove_All", PluginInstance, dev$getTexts()),
           polylines = .Call("R_NPAPI_Remove_All", PluginInstance, dev$getPolyLines()),
           polygons = .Call("R_NPAPI_Remove_All", PluginInstance, dev$getPolyLines()),
           rects = .Call("R_NPAPI_Remove_All", PluginInstance, dev$getRects())
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

raphStrWidth = function(txt, paper, size)
  {
    txt = paper$text(0, 0, txt);
    
    txt$attr("font-size", size);
    bbox = txt$getBBox();
    ret = as.numeric(bbox[["width"]])
    txt$remove()
    ret
  }

setGeneric("raphAttr", function(el, name, ...) standardGeneric("raphAttr"))
setMethod("raphAttr", c(el="list"), 
          function(el, name, ...) lapply(el, function(x) attr(x,name)))
setMethod("raphAttr", c(el="JSRaphaelRef"),
          function(el, name, ...) el$attr(name))
setMethod("raphAttr", c(el="ANY"),
          function(el, name, ...) stop(paste("Cannot call raphAttr on element of class", class(el))))

setGeneric("raphAttr<-", function(el, name,value,  ...) standardGeneric("raphAttr<-"))
setMethod("raphAttr<-", c(el="list"), 
          function(el, name, value, ...) lapply(el, function(x, ...) x$attr(name,value, ...), ...))
setMethod("raphAttr<-", c(el="JSRaphaelRef"),
          function(el, name, value, ...) x$attr(name, value, ...))
setMethod("raphAttr<-", c(el="ANY"),
          function(el, name, value) stop(paste("Cannot call raphAttr<- on element of class", class(el))))

