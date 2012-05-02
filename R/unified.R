
callJavaScript = function(object = getGlobalObject(), name, multipleArgs = FALSE, keepResult = TRUE, convertRet = FALSE, convertArgs, ...)
  {
    args = list(...)
    if(missing(convertArgs))
      convertArgs = rep(TRUE, times=length(args))

    #check whether we are in Firefox or NPAPI-browser
    if(exists(ScriptCon))
      #Need to update the RFirefox API if this is going to stay in
      call_JS_Method(ScriptCon, object, name, args, multipleArgs, addRoot = keepResult)
    else
      {
        #NPAPI call code goes here!
        NP_Invoke(obj = object, name = name, convertRet = convertRet, convertArgs = convertArgs, keepResult = keepResult, argList = args)
      }
  }

evalJavaScript = function(scope = getGlobalObject(), script, keepResult = TRUE, returnRef=FALSE)
  {
    if(length(script)>1)
      script = paste(script, collapse = "\n")
    if(exists(ScriptCon))
      {
        toret = jsVal()
        JS_EvaluateScript(ScriptCon, scope, script, nchar(script), "evalingJS", 1, toret)
        if(keepResult)
          {
            JS_AddRoot(ScriptCon, toret)
            toret
          } else {
            TRUE
          }
      } else {
        #NPAPI eval code goes here
        NULL
      }
  }

'jsProperty<-' = function(object, name, value, convertValue = TRUE)
  {
    if(exists(ScriptCon))
      set_JS_Property(ScriptCon, object, name, value)
    else
      {
        #NPAPI property set code goes here
        NP_SetProperty(obj = object, name = name, value = value, convertValue = convertValue)
      }
    object
  }

jsProperty = function(object, name, convertRet = FALSE)
  {
    if(exists(ScriptCon))
      get_JS_Property(ScriptCon, object, name)
    else
      {
        #NPAPI property get code here
        NP_GetProperty(obj = object, name = name, convertRet = convertRet)
      }
  }

getGlobalObject = function()
  {
    if(exists(ScriptCon))
      JS_GlobalObject(ScriptCon) #Need to figure out if this needs addRoot = TRUE or not!!!
    else
      {
        #NPAPI code goes here
        NP_GetGlobalRef()
      }
  }

getPageElement = function(id, returnRef = FALSE)
  #Note: returnRef defaults to FALSE but the default behavior for DOM elements is references anyway...
  {
    evalJavaScript(paste("document.getElementById(", id, ");"), returnRef = returnRef)
  }


addPageElement = function(parent, type, id, properties = list())
  {
    TRUE
  }

removePageElement = function(id, object=NULL)
  {

    if(is.null(object))
      object = getPageElement(id)

    #Code to remove element.
  }

addEventListener = function(target, event, rfun)
  {
    TRUE
  }

removeEventListener = function(target, event)
  {
    TRUE
  }

fireDOMEvent = function(target, type)
  {


    TRUE
  }

toR = function(object)
  {

    TRUE
  }


setMethod("names", "JSValueRef", function(x)
          {
            TRUE
          })

#Name of arguments???
setMethod("[[", "JSValueRef", function(x, i, ...)
          {
            jsProperty(x, i, j, ...)
          })

#setGeneric("$", function(x, name, ...) standardGeneric("$"))
setMethod("$", "JSValueRef",
          function(x, name)
          {
            fun = function( keepResult = TRUE, returnRef = FALSE, ...)
              {
                args = list(...)
                if(length(args) > 1)
                  multipleArgs = TRUE
                else
                  multipleArgs = FALSE
                
                callJavaScript(x, name,args, multipleArgs = multipleArgs, keepResult = keepResult, returnRef = returnRef)
              }
            fun
          }
          )
