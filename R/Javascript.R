
evaluateJSInBrowser = function(browser, jscript)
  {
    if (class(browser) == "MozGtkBrowser")
      browser = gtkMozGetnsIWB(browser)

    if(length(jscript) > 1)
      jscript = paste(jscript, collapse="\n")
    
    .Call("R_EvaluateScriptInBrowser", browser, jscript, nchar(jscript))
  }

evaluateJSInContext = function(context, jscript)
  {
    if (length(jscript) > 1)
      jscript = paste(jscript, collaps = "\n")

    .Call("R_EvaluateScriptInContext", context, jscript, nchar(jscript))
  }


getnsIScriptContext = function(nsIWB)
  {
    .Call("R_get_nsIScriptContext", nsIWB)
  }

extensionGetContext = function()
  {
    .Call("R_ExtensionGetContext")
  }


getJSValAsInt = function(jsval)
  {
    .Call("R_getJSVALInt", jsval)
  }

callMethodTest = function(cx, parentasjsv, fname, nargs, args, retval)
  {
    .Call("R_CallMethodTest", cx, parentasjsv, fname, nargs, args, retval)
  }
