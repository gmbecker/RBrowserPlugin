library(XML)
library(gam)
storage = new.env()

glmSummaryToHTML = function(fit, returnChar = TRUE)
  {
    if("gam" %in% class(fit))
      coeff = as.matrix(summary(fit)$anova)
    else       
      coeff = summary(fit)$coefficients
    cont = newXMLNode("div")
    xmlAttrs(cont) = c(style="float:left;")
    coeffs = matrixToHTML(coeff, parent=cont)
    newXMLNode("br", parent = cont)
    p = newXMLNode("div", parent = cont, attrs = c(class = "glmsummarytext"))
    newXMLTextNode(parent = p, paste("AIC:", round(AIC(fit), 2)))
    newXMLNode("br", parent = p )
    newXMLTextNode(parent = p, sprintf("Null Deviance: %3.2f on %i degrees of freedom", fit$null.deviance, as.integer(fit$df.null)))
    newXMLNode("br", parent = p)
    newXMLTextNode(parent = p, sprintf("Residual Deviance: %3.2f on %f degrees of freedom", fit$deviance, fit$df.residua))   
    xmlAttrs(p) = c(class = "glmsummarytext")
    newXMLNode("br", parent=p)
    newXMLNode("br", parent=p)
  
    if(returnChar)
      as(cont, "character")
    else
      cont
  }

matrixToHTML = function(mat, parent = NULL)
  {
    if(is.null(parent))
      tab = newXMLNode("table")
    else
      tab = newXMLNode("table", parent = parent)
    r = newXMLNode("tr", parent = tab)
    newXMLNode("th", parent = r)
    sapply(colnames(mat), function(x, r) newXMLNode("th", parent = r, x), r = r)
    sapply(1:nrow(mat), 
          function(i, tab, mat)
          {
            r = mat[i,]
            name = row.names(mat)[i]
            row = newXMLNode("tr", parent=tab)
            newXMLNode("td", parent = row, name)
            
            sapply(r, function(x, row)
                   {
                     if (is.numeric(x))
                       x = round(x, 5)
                   newXMLNode("td", parent =row, x)
                     },
                   row = row
                   )
          }, tab=tab, mat = mat)
    tab
  }



setVars = function(args, e=storage)
  {

    args = as.numeric(args)
    print(paste("in setVars:", args))
    e$linFormula = as.formula(paste("mpg ~ ", paste (e$linTerms[args], collapse = "+"), sep=""))
    e$gamFormula = as.formula(paste("mpg ~ ", paste (e$gamTerms[args], collapse = "+"), sep=""))

    displayModel()

  }

showHideModel = function(show = 1, e = storage)
  {
    show = as.numeric(show)

    if(show != e$currentShow)
      {
        if(show == 1)
          {
            linval = "block"
            linwidth="400px"
            gamval = "none"
            gamwidth = "0px"
          } else if (show == 2){
            linval = "none"
            linwidth = "0px"
            gamval = "block"
            gamwidth="400px"
          } else {
            linval = "block"
            linwidth = "400px"
            gamval = "block"
            gamwidth="400px"
          }

        set_JS_Property(ScriptCon, e$lindivstyle, "display", linval)
        set_JS_Property(ScriptCon, e$lindivstyle, "width", linwidth)
        set_JS_Property(ScriptCon, e$gamdivstyle, "display", gamval)
        set_JS_Property(ScriptCon, e$gamdivstyle, "width", gamwidth)
        
        e$currentShow = show

        displayModel()
      }
  }

displayModel = function(e = storage)
  {
    print("in displayModel")
    if(e$currentShow %in% c(1, 3))
      {
        e$linFit = glm(e$linFormula, data=e$data)
        set_JS_Property(ScriptCon, e$lintable, "innerHTML", paste(glmSummaryToHTML(e$linFit), collapse="\n"))
      }
    if(e$currentShow %in% c(2,3))
      {
        
        e$gamFit = gam(e$gamFormula, data=e$data)
        print(summary(e$gamFit))
        set_JS_Property(ScriptCon, e$gamtable, "innerHTML", paste(glmSummaryToHTML(e$gamFit), collapse="\n"))
      }

    showDiagnostics()
  }

showDiagnostics = function(e = storage)
{
  print("in showDiagnostics")
  if(e$currentShow %in% c(1, 3))
    {
  #    dev.set(e$linDevNum)
      #clearRaphPlot(e$linDev)
      #plot(e$linFit$fitted, e$data[["mpg"]])
   #   plot(1:10)
    }
  if(e$currentShow %in% c(2, 3))
    {
    #  dev.set(e$gamDevNum)
      #clearRaphPlot(e$gamDev)
      #plot(e$gamFit$fitted, e$data[["mpg"]])
    }
     
TRUE
}



storage$linTerms = names(mtcars)[-1]
sm = sapply(mtcars[,-1], function(x) length(unique(x)) > 4)
storage$gamTerms = storage$linTerms
storage$gamTerms[sm] = paste("s(", storage$gamTerms[sm], ")", sep="")
storage$gamtable = getElementById(ScriptCon, "gamtable")
storage$lintable = getElementById(ScriptCon, "linregtable")
storage$lindiv = getElementById(ScriptCon, "linregcontainer")
storage$gamdiv = getElementById(ScriptCon, "gamcontainer")
storage$lindivstyle = get_JS_Property(ScriptCon, storage$lindiv, "style")
storage$gamdivstyle = get_JS_Property(ScriptCon, storage$gamdiv, "style")
storage$currentShow = 1
storage$data = mtcars
if(FALSE)
  {
storage$linDev = raphaelDev("linregplot")
storage$linDevNum = dev.cur()
storage$gamDev = raphaelDev("gamplog")
storage$gamDevNum = dev.cur()
}
setVars(1:10)
showHideModel(1)
