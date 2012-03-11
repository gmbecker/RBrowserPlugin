displayDF = function(row)
  {
div = getElementById("dfgrid")



  }


glmSummaryToHTML = function(fit, returnChar = TRUE)
  {
    coeff = summary(fit)$coefficients
    cont = newXMLNode("div")
    xmlAttrs(cont) = c(style="float:left;")
    coeffs = matrixToHTML(coeff, parent=cont)
    newXMLNode("br", parent = cont)
    p = newXMLNode("div", parent = cont, attrs = c(class = "glmsummarytext"))
    newXMLTextNode(parent = p, paste("AIC:", round(AIC(fit), 2)))
    newXMLNode("br", parent = p )
    newXMLTextNode(parent = p, sprintf("Null Deviance: %3.2f on %i degrees of freedom", fit$null.deviance, fit$df.null))
    newXMLNode("br", parent = p)
    newXMLTextNode(parent = p, sprintf("Residual Deviance: %3.2f on %i degrees of freedom", fit$deviance, fit$df.residual))    
    xmlAttrs(p) = c(class = "glmsummarytext")
    newXMLNode("br", parent=p)
    newXMLNode("br", parent=p)
    newXMLTextNode(parent = p, "Classification Table")
    newXMLNode("br", parent=p)
    newXMLTextNode(parent = p, "Cutoff value: .5")
    tab = table(true=as.logical(fit$y), fitted = fit$fitted.values > .5)
    matrixToHTML(tab, parent = cont)
  
    if(returnChar)
      as(cont, "character")
    else
      cont
  }

showLogistic = function()
  {
    set_JS_Property(ScriptCon, logdivstyle, "display", "block")
    logHidden <<- FALSE
    
  }

hideLogistic = function()
  {
    set_JS_Property(ScriptCon, logdivstyle, "display", "none")
    logHidden <<- TRUE
  }

showLinReg = function()
  {
    set_JS_Property(ScriptCon, lindivstyle, "display", "block")
    linHidden <<- FALSE
  }

hideLinReg = function()
  {
    set_JS_Property(ScriptCon, lindivstyle, "display", "none")
    linHidden <<- TRUE
  }

handleMethodChoice = function(val)
  {
    print(paste("in handleMethodChoice: ", val, "class(val):", class(val)))
    
    if(val == "1")
      {
        updateLinReg()
        hideLogistic()
        logHidden <<- TRUE
      }
    else if (val == "2")
      {
        updateLogReg()
        hideLinReg()
        linHidden <<- TRUE
      }
    else if (val == "3")
      {
        updateLinReg()
        updateLogReg()
      }
  }

updateLinReg = function()
  {
    if(linHidden)
      showLinReg()
    
    linfit = glm(admit ~ ., data = modelframe)
    linsum = ""
    #lincon = textConnection("linsum", open = "w")
    #print(lincon)
    #HTML(summary(linfit), file=tmpfile, append=FALSE)
    #print(length(linsum))
    linsumnode = glmSummaryToHTML(linfit)
    
    set_JS_Property(ScriptCon, lintable, "innerHTML", linsumnode)
    #close(lincon)
  }

updateLogReg = function()
  {
    if (logHidden)
      showLogistic()
   
    logfit = glm(admit ~ ., data = modelframe, family=binomial(logit))
    #logsum = ""
    #logcon = textConnection("logsum", open= "w")
    #cat("testing 123", file=logcon)
    #print(nchar(logsum))
    #HTML(summary(logfit), file=tmpfile, append=FALSE, encoding="UTF-8")
    #logsum = readLines(tmpfile)
    logsum = as(glmSummaryToHTML(logfit), "character")
    logsum = paste(logsum, collapse="\n")
    print(nchar(logsum))
    set_JS_Property(ScriptCon, logtable, "innerHTML", logsum)
    #close(logcon)
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

    

#data retrieved from the ucla ATS stats tutorial website
library(RFirefox)
library(R2HTML)
library(XML)
adm = read.csv("/home/gmbecker/gabe/checkedout/GabeResearch/Packages/RFirefox/inst/examples/gradAdmisdat.csv")
modelframe = adm

lindiv = getElementById(ScriptCon, "linregcontainer")
lindivstyle = get_JS_Property(ScriptCon, lindiv, "style")

lintable = getElementById(ScriptCon, "linregtable")
logdiv = getElementById(ScriptCon, "logisticcontainer")
logdivstyle = get_JS_Property(ScriptCon, logdiv, "style")


logtable = getElementById(ScriptCon, "logittable")
linHidden = FALSE
rlogHidden = TRUE
tmpfile = tempfile()
handleMethodChoice("1")


