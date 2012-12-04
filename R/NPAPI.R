setClass("NPObjectRef", contains="RCStructReference")
setClass("NPVariantRef", contains="RCStructReference")

NP_Invoke = function(...,
  plug = PluginInstance,
  obj = JS,
  name,
  convertRet = "default",
  convertArgs = NULL,
  keepResult = TRUE,
  argList)
  {
    if(!missing(argList))
      args = argList
    else
      args = list(...)
    #max in times below because some calls can have no arguments, but match.call fails on a primary argument of length 0
    if(is.null(convertArgs))
      convertArgs = rep("default", times=max(length(args), 1))


    convArgsEnum = getConvEnum(convertArgs, several.ok=TRUE)
    if(is.function(convertRet))
      convRetEnum = getConvEnum("reference")
    else
      convRetEnum = getConvEnum(convertRet)  

    ret = .Call("R_NPAPI_Invoke", plug, obj, name, args, convArgsEnum, convRetEnum, keepResult)
    if(is.function(convertRet))
      {
        ret = convertRet(ret)
        print("convertRet function detected!")
      }
    ret
  }

NP_GetProperty = function( obj = JS, name, convertRet = "default", plug = PluginInstance)
  {
    if(is.function(convertRet))
      convRetEnum = getConvEnum("reference")
    else
      convRetEnum = getConvEnum(convertRet)  
    
    ret = .Call("R_NPAPI_GetProperty", plug, obj, name, convRetEnum)
    if(is.function(convertRet))
        {
        ret = convertRet(ret)
        print("convRet function detected!")
      }
    ret
  }

NP_SetProperty = function( obj = JS, name, value, plug = PluginInstance, convertValue = "default")
  {
    
    .Call("R_NPAPI_SetProperty", plug, obj, name, value, getConvEnum(convertValue))
  }


RtoNPAPI = function(Robj, plug = PluginInstance)
  {
    .Call("R_ConvertRToNP", Robj, plug)
  }


NPtoR = function(NPobj, plug = PluginInstance)
  {
    .Call("R_ConvertNPtoR", NPObj, plug)
  }


doTest = function()
  {
    .Call("R_doTest", PluginInstance)
  }

NP_GetGlobal = function(plug = PluginInstance)
  {
    .Call("R_NP_GetGlobal", plug)
  }


getConvEnum = function(x, several.ok = FALSE)
  {
    if(!length(x)||!is.character(x))
      {
        x = "default"
      }
    x = match.arg(x, c("default", "reference", "copy"), several.ok = several.ok)
    sapply(x, function(y)
           {
             if(y == "reference")
               0L
             else if (y == "default")
               1L
             else if (y == "copy")
               2L
             else
               stop("invalid value for convertArgs")
           })
  }

