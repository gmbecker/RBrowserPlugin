#
# polyline.
# Fonts
# Fix the point size in the gcontext being 0.
# Explore other ways of rendering text.
# Polygon is very slow
#

library(RGraphicsDevice)

   #  A class to represent the functions as being specific to a JavaScript Canvas device
setClass("RJavaScriptCanvasMethods", contains = "RDevDescMethods")

asJSFunction =
  #
  # This takes the javascript code and puts it into a function
  # adding extra material to check that the JavaScript canvas is supported
  # and obtaining the context from the HTML canvas element.
  #
  #  code - the character vector giving the Javascript code for drawing the plot.
  #
  #  name - the name of the JavaScript function to be defined.
  #
  #  addContext - logical indicating whether to add extra code to determine
  #               if the JavaScript engine has support for the canvas
  #               and for obtaining the context object used for drawing.
  #
  #  canvasId - the value of the id attribute identifying the canvas element in the
  #             HTML document
  #
function(code, name = "rdraw", addContext = TRUE, canvasId = "device")
{
  paste(c(paste("function", name, "()"), "{", 
         if(addContext)
           paste("    ", 
                 c(sprintf("var canvas = document.getElementById('%s');", canvasId),
                   "if(! canvas.getContext) return;",
                    "var ctx = canvas.getContext('2d');",
                    "CanvasTextFunctions.enable(ctx);")),
         paste("     ", code), "}"), collapse = "\n")
}


setContext =
  # 
  #  set the current values for the JavaScript context based on the
  #  values from R's graphic context.
  #
function(context)
{
   col = as(context$col, "RGB")
   fill = as(context$fill, "RGB")   

   c(if(!isTransparent(col))
         sprintf("ctx.strokeStyle = '%s';", col),
     if(!isTransparent(fill))
         sprintf("ctx.fillStyle = '%s';", fill),
     sprintf("ctx.lineWidth = '%d';", context$lwd),
     sprintf("ctx.lineJoin = '%s';", jsLineJoin(context$ljoin)),
     sprintf("ctx.lineCap = '%s';", jsLineCap(context$lend))   
    )
}

jsLineJoin =
    # map the R value for a R_GE_linejoin value to a string that
    # we use in JavaScript for the lineJoin field of the JS context.
function(x)
{
      if(length(names(x)) == 0)
        x = as(x, 'R_GE_linejoin')

      tolower(gsub("(GE_|_JOIN)", "", names(x)))
}

jsLineCap =
    # map the R value for a R_GE_lineend value to a string that
    # we use in JavaScript for the lineCap field of the JS context.  
function(x)
{
     if(length(names(x)) == 0)
       x = as(x, 'R_GE_lineend')  

     tolower(gsub("(GE_|_CAP)", "", names(x)))
}


writeCode =
  #
  # Write the list of plots to a file/connection.
  #  pages is a list and each element is a character vector giving the
  #  JavaScript code for that plot.
  #  We call asJSFunction() for each of these, adding the computations to
  #  get the context from the specified <canvas> element(s) in the HTML document
  #  If there are multiple plots, the caller should supply either a vector of canvas identifiers
  #  or we will use canvas1, canvas2, canvas3, ...
  #
function(pages, file, canvasId = 'canvas')
{
      if(length(pages) == 0) {
        warning("no content produced for plot")
        return(character())
      }
  
      if(length(pages) == 1) {
        cat(asJSFunction(pages[[1]], canvasId = canvasId), "\n\n", sep = "\n", file = file)
        "rdraw"
      } else {
        if(!inherits(file, "connection")) {
          con = file(file, "w")
          on.exit(close(con))
        } else
          con = file


        if(length(canvasId) < length(pages))
          canvasId = c(canvasId, paste('canvas', seq(length(canvasId) + 1, length(pages))))
        
        funNames = paste("rdraw", seq(along = pages), sep = "")
        mapply(function(code, id, canvasId)
                    cat(asJSFunction(code, canvasId = canvasId),
                          "\n\n", sep = "\n", file = con),
                pages, funNames, canvasId)
        funNames
      }
}



###############################################

jsCanvas =
  # The function that creates the graphics device, assembling the
  #  graphical functions that share a common state where the code is written.
  #
  #  file can be a file name or a connection.
  #  It can also be an expression in which case we will return the generated code.
  #  ... is currently for wrapup, but we may want to use if for additional parameter settings.
  #
function(file, dim = c(1000, 800), col = "black", fill = "transparent", ps = 10, wrapup = writeCode, ...)
{
  pages = list()
  commands = character()
  add = function(x)
    commands <<- c(commands, x)

  endPage = function()
    {
      if(length(commands)) {
         pages[[ length(pages) + 1 ]] <<- commands
         commands <<- character()
      }
    }


  funs = as(dummyDevice(), "RJavaScriptCanvasMethods")
  
# circle, line, rect
# text
# polygon, polyline
# strWidth, metricInfo, strWidthUTF8,

# size
# newPage


    # No implementations for
  funs@mode = NULL
  funs@metricInfo = NULL
  funs@activate = NULL
  funs@deactivate = NULL
  funs@deactivate = NULL
  funs@locator = NULL
  funs@onExit = NULL

  
  funs@line = function(x1, y1, x2, y2, context, dev) {
    add(c("// line",
          "ctx.beginPath();",
          setContext(context),
          sprintf("ctx.moveTo(%s, %s);", as.integer(x1), as.integer(y1)),
          sprintf("ctx.lineTo(%s, %s);", as.integer(x2), as.integer(y2)),
          "ctx.stroke();"))
  }

  funs@rect = function(x1, y1, x2, y2, context, dev) {
    op = if(!isTransparent(context$fill))  "fillRect" else "strokeRect"

    add(c("// rect",
          setContext(context),
          sprintf("ctx.%s(%d, %d, %d, %d);",
                     op,
                     as.integer(min(x1, x2)), as.integer(min(y1, y2)),
                     abs(as.integer(x2 - x1)), abs(as.integer(y2 - y1)))))
  }

  funs@circle = function(x1, y1, r, context, dev) {

    add(c("// circle",
          "ctx.beginPath()",
          setContext(context),
          sprintf("ctx.arc(%d, %d, %d, 0, 2 * Math.PI, true);", as.integer(x1), as.integer(y1), as.integer(r)),
          if(isTransparent(context$fill)) "ctx.fill()" else "ctx.stroke()"))
  }

  funs@text = function(x1, y1, txt, rot, hadj, context, dev) {

    add(c("// text",
                                        #        'ctx.fillStyle = "Black";', # fillText()
           setContext(context),
           sprintf("ctx.drawText('%s', %d, %d, %d, '%s');",
                    "sans", as.integer(max(10, context$ps) * context$cex),
                     as.integer(x1), as.integer(y1), txt)))
  }

  funs@strWidth = function(str, gcontext, dev) {
     nchar(str) * max(10, gcontext$ps) * gcontext$cex
  }

  funs@newPage = function(gcontext, dev) {
     endPage()
  }


  funs@close = function(dev) {
      endPage()
      if(!is.language(file))      
        wrapup(pages, file, ...)
  }


  funs@polyline =
      #
      # XXX Connecting all the successive points. i.e. Polygon.
      #
  function(n, x, y, gcontext, dev) {

    x = x[1:n]
    y = y[1:n]
    add(c(paste("// polyline", n),
        "ctx.beginPath();",
        setContext(gcontext),
            # move to the first point and then connect all the subsequent ones.
        sprintf("ctx.moveTo(%s, %s);", as.integer(x[1]), as.integer(y[1])),
        sprintf("ctx.lineTo(%s, %s);", as.integer(x[-1]), as.integer(y[-1])),
        "ctx.stroke();"))
  }

  funs@polygon =
    function(n, x, y, gcontext, dev)
      {
        x = x[1:n]
        y = y[1:n]
        
        add(c(paste("// polygon", n),
              "ctx.beginPath();",
              setContext(gcontext),
              sprintf("ctx.moveTo(%s, %s);", as.integer(x[1]), as.integer(y[1])),
              sprintf("ctx.lineTo(%s, %s);", as.integer(x[-1]), as.integer(y[-1])),
              "ctx.closePath()",
              if(isTransparent(gcontext$fill)) "ctx.stroke();" else "ctx.fill();"))
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
  }

  dev = graphicsDevice(funs, dim, col, fill, ps)

  dev$ipr = rep(1/72.27, 2)
  dev$cra = rep(c(6, 13)/12) * 10
  
  if(is.language(file)) {
     eval(file, sys.parent())
     dev.off()
     endPage()
     pages
  } else
     dev
}


###############  HTML Section ##################

htmlCanvas =
  #
  # The idea here is to create an HTML document with the code inside it.
  # We can do this if we get the code back which we can do by passing the plotting expressions to
  # this function. But we also have to handle the asynchronous case where we call dev.off()
  # and it writes the code.  To do this, we specify a different value of wrapup for the
  # jsCanvas() function.
  #
function(file, dim = c(1000, 800), template = "template.html")
{
  jsCanvas(file, dim, wrapup = htmlWrapup, template = template, dim)
}

htmlWrapup =
  #
  #
  # XXX Fix up the width and height of the canvas elements. 
  #
  #
function(pages, file, template = "template.html", dim = c())
{
  library(XML)

    # Read the template
  doc = htmlParse(template)

    # get the id's of the canvases
  canvas = getNodeSet(doc, "//canvas")
  canvasId = sapply(canvas, xmlGetAttr, "id")

  if(length(dim))
    sapply(canvas, function(x) xmlAttrs(x) = c(width = dim[1], height = dim[2]))

    # Now get the JavaScript code as one big block
    # We arrange for writeCode() to write to a text connection.
  con = textConnection(NULL, "w", local = TRUE)
  funNames = writeCode(pages, con, canvasId = canvasId)
  jsCode = textConnectionValue(con)

 
  head = getNodeSet(doc, "//head")[[1]]
    # Don't use CDATA nodes. HTML browser's don't like them.
    # XX We should escape &, <, > etc.
  newXMLNode("script", attrs = c(type = "application/x-javascript"),
              paste(jsCode, collapse = "\n"),
              parent = head)

    # Fix up empty <script> nodes as Firefox doesn't like <script ... />
    # but wants <script....></script>
  emptyScripts = getNodeSet(head, ".//script[string(.) = '']")
  sapply(emptyScripts, function(x) newXMLTextNode( " " , parent = x))
  
    # Add the onload to call the functions.
  body = getNodeSet(doc, "//body")[[1]]
  xmlAttrs(body) = c("onload" = paste(funNames, "()", collapse = "; "))
  
  saveXML(doc, file)
}
