
callJavaScript = function( name, ..., object = getGlobalJSObject(), multipleArgs = FALSE, keepResult = TRUE, convertRet = "default", convertArgs = NULL)
  {
    args = list(...)

    if(missing(convertArgs))
      print("convertArgs is missing!!!")
   
    #check whether we are in Firefox or NPAPI-browser
    if(exists("ScriptCon"))
      #Need to update the RFirefox API if this is going to stay in
      #call_JS_Method(ScriptCon, object, name, args, multipleArgs, addRoot = keepResult)
      TRUE
    else
      {
        #NPAPI call code goes here!
        NP_Invoke(obj = object, name = name, convertRet = convertRet, convertArgs = convertArgs, keepResult = keepResult, argList = args)
      }
  }

evalJavaScript = function(script, scope = getGlobalJSObject(), keepResult = TRUE, convertRet="default")
  {
    if(length(script)>1)
      script = paste(script, collapse = "\n")
    if(exists("ScriptCon"))
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
        #function(object = getGlobalJSObject(), name, multipleArgs = FALSE, keepResult = TRUE, convertRet = FALSE, convertArgs, ...)
        callJavaScript(object = scope, name = "eval", script, convertRet = convertRet, keepResult = keepResult)
      }
  }

'jsProperty<-' = function(object, name, value, convertValue = "default")
  {
    if(length(name) > 1)
      {
        for(i in seq(along=name))
          jsProperty(object, name[i], convertValue = convertValue[i]) <- value[i]
        return(object)
      }
    
    if(exists("ScriptCon"))
      set_JS_Property(ScriptCon, object, name, value)
    else
      {
        #NPAPI property set code goes here
        NP_SetProperty(obj = object, name = name, value = value, convertValue = convertValue)
      }
    object
  }

jsProperty = function(object, name, convertRet = "default")
  {
    if(length(name) > 1)
      return(mapply(function(nm, conv) jsProperty(object, nm, conv), name, convertRet))
    if(exists("ScriptCon"))
      get_JS_Property(ScriptCon, object, name)
    else
      {
        #NPAPI property get code here
        NP_GetProperty(obj = object, name = name, convertRet = convertRet)
      }
  }

getGlobalJSObject = function()
  {
    if(exists("ScriptCon"))
      JS_GlobalObject(ScriptCon) #Need to figure out if this needs addRoot = TRUE or not!!!
    else
      {
        #NPAPI code goes here
        res = NP_GetGlobal()
        res
      }
  }

getPageElement = function(id, convertRet = "default")
  {
    if(length(id) > 1)
      return(mapply(getPageElement, id, convertRet))
    #When JS object is implemented
    JS[["document"]]$getElementById(id, convertRet)
    #window = getGlobalJSObject()
    #document = window[["document"]]
    #document$getElementById(id)
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
    if(length(event) > 1)
      return(mapply(function(e, f) addEventListener(target, e, f), event, rfun))
    TRUE
  }

removeEventListener = function(target, event)
  {
    TRUE
  }

raiseDOMEvent = function(target, type)
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
#setMethod("[[", "JSValueRef", function(x, i, ...)
setMethod("[[", c("NPVariantRef", "character", "missing"), function(x, i, ...)
          {
            jsProperty(x, i, ...)
          })


setMethod("[[<-", c("NPVariantRef", "character", "missing"), function(x, i, ..., value)
          {
            jsProperty(x, i, ...) <- value
          })

#setGeneric("$", function(x, name, ...) standardGeneric("$"))
#setMethod("$", "JSValueRef",
setMethod("$", "NPVariantRef",
          function(x, name)
          {

            #fun = function( ...,  keepResult = TRUE, returnRef = FALSE, convertArgs)
            fun = function( ...,  keepResult = TRUE, convertRet = "default", convertArgs = NULL)
              {
              
                args = list(...)
                if(length(args) > 1)
                  multipleArgs = TRUE
                else
                  multipleArgs = FALSE

                callJavaScript(object = x,name =  name, ... , multipleArgs = multipleArgs, keepResult = keepResult, convertRet = convertRet, convertArgs = convertArgs)
              }
            fun
          }
          )
