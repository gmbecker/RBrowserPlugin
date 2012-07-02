setClass("NPObjectRef", contains="RCStructReference")
setClass("NPVariantRef", contains="RCStructReference")
NP_Invoke = function(..., plug = PluginInstance, obj = NP_GetGlobal, name, convertRet = FALSE, convertArgs, keepResult = TRUE, argList)
  {
    if(!missing(argList))
      args = argList
    else
      args = list(...)
    if(missing(convertArgs))
      convertArgs = rep(TRUE, times=length(args))
    .Call("R_NPAPI_Invoke", plug, obj, name, args, convertArgs, convertRet, keepResult)
  }

NP_GetProperty = function( obj = NP_GetGlobal(), name, convertRet = FALSE, plug = PluginInstance)
  {
    .Call("R_NPAPI_GetProperty", plug, obj, name, convertRet)
  }

NP_SetProperty = function( obj = NP_GetGlobal(), name, value, plug = PluginInstance, convertValue = TRUE)
  {
    
    .Call("R_NPAPI_SetProperty", plug, obj, name, value, convertValue = TRUE)
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
