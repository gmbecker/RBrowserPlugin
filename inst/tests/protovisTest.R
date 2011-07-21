library(webvis)
library(RGtk2)
library(RFirefox)

initGtkMozEmbed()
mozembed = gtkMozNewBrowser()
gtkMozLoadURL(mozembed, paste("file://",
                              system.file("testdev.html",
                                          package="RFirefox"),
                              sep=""))

plot.webvis2 = function (x, y = NULL, type = "dot", width = 300, height = 200, 
    add.grid = TRUE, add.axes = TRUE, scale.min = NULL, browser, ...) 
{
    if (is.null(y) && is.vector(x)) {
        data <- data.frame(y = x, x = 1:length(x))
    }
    else {
        data <- data.frame(x = x, y = y)
    }
    wv <- pv.panel(width = width + 50, height = height + 50, 
        left = 50, bottom = 50, right = 50, top = 50)
    wv <- if (type == "bar") {
        wv + pv.bar(wv = wv, data = data, ...)
    }
    else if (type == "line") {
        wv + pv.line(wv = wv, data = data, scale.min = scale.min, 
            ...)
    }
    else if (type == "dot") {
        wv + pv.dot(wv = wv, data = data, scale.min = scale.min, 
            ...)
    }
    else if (type == "pie") {
        wv + pv.wedge(wv = wv, data = data, ...)
    }
    else if (type == "area") {
        wv + pv.area(wv = wv, data = data, scale.min = scale.min, 
            ...)
    }
    if (type != "pie") {
        if (add.grid) {
            wv <- wv + (pv.rule(wv = wv, data = as.numeric(sprintf("%1.02f", 
                seq(if (!esse(scale.min)) min(data$y) else scale.min, 
                  max(data$y), length.out = (height/20)))), axis = "y", 
                stroke.style = "rgba(128,128,128,.2)") + pv.label(anchor = "left", 
                text.name = "y"))
            wv <- wv + (pv.rule(wv = wv, data = data.frame(x = as.numeric(sprintf("%.2f", 
                seq(min(data$x), max(data$x), length.out = (width/20))))), 
                axis = "x", stroke.style = "rgba(128,128,128,.2)") + 
                pv.label(anchor = "bottom", text.name = "x", 
                  text.angle = -pi/4, text.align = "right"))
        }
        if (add.axes) {
            wv <- wv + pv.rule(wv = wv, bottom = 0)
            wv <- wv + pv.rule(wv = wv, left = 0)
        }
    }
    evaluateJSInBrowser(browser,as.character(unfold.webvis(wv))) 
  }
#I'm pretty sure the problem is we are not living in a
#<script type="text/javascript+protovis"> when our code is
#excuting. Not sure what to do about that yet.

#from http://vis.stanford.edu/protovis/docs/start.html
#. The second <script> element is your visualization! Pay close attention to the type attribute: text/javascript+protovis. This allows you to use expression closures in browsers other than Firefox. If you only want to use Firefox, or if you prefer to write proper functions, you can use the standard text/javascript instead. 

plot.webvis2(x=10*rnorm(20), y=10*rnorm(20), width=500, height=300, type="dot",
  browser = mozembed)

plot.webvis(x=10*rnorm(20), y=10*rnorm(20), width=500, height=300, type="dot",
  browser = mozembed)
