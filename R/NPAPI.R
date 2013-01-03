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
    convArgsFuns = mapply(function(enum, obj)
      {
        if(enum == 3)
          getJSMethodObj(obj)
        else
          NA
      }, convArgsEnum, convertArgs)
    if(is.function(convertRet))
      convRetEnum = getConvEnum("custom")
    else
      convRetEnum = getConvEnum(convertRet)  

    ret = .Call("R_NPAPI_Invoke", plug, obj, name, args, convArgsEnum, convArgsFuns, convRetEnum, keepResult)
    if(is.function(convertRet))
      {
        print("convertRet function detected!")
        ret = convertRet(ret)
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


getConvEnumOld = function(x, several.ok = FALSE)
  {
    

    if(!length(x)||!is.character(x))
      {
        x = "default"
      }

    x = match.arg(x, c("default", "reference", "copy", "custom"), several.ok = several.ok)
    sapply(x, function(y)
           {
             if (y == "default")
               0L
             else if(y == "reference")
               1L
             else if (y == "copy")
               2L
             else if (y == "custom")
               3L
             else
               stop("invalid value for convertArgs")               
           })
  }

getConvEnum = function(x, several.ok = FALSE)
  {
    if(!length(x))
      x = "default"
    if(is.character(x))
      {
        x = match.arg(x, c("default", "reference", "copy", "custom"), several.ok = several.ok)
        sapply(x, function(i) switch(i, default=0L, reference=1L, copy=2L, custom=3L))
      }
    else
      {
        sapply(x, function(y)
               {
                 if(is.character(y))
                   {
                     y = match.arg(y, c("default", "reference", "copy", "custom"), several.ok = FALSE)
                   }
                 else if (is.function(y))
                   y = "custom"
                 else
                   stop("invalid value for convertArgs")
                 switch(y, default=0L, reference=1L, copy=2L, custom=3L)
               })
      }
  }
      

    
    
