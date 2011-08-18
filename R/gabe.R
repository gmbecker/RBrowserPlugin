setClass('JSObjectRef', contains = "RCStructReference")
setClass('jsvalRef', contains = "RCStructReference")
setClass('JSContextRef', contains = "RCStructReference")

STRING_TO_JSVAL = function(con, jstring, addRoot = FALSE)
  {
    .Call("R_STRING_TO_JSVAL", con, jstring,  addRoot)
  }


JSVALPTR_TO_JSVAL = function(ptr)
  {
    .Call("jsvalptr_to_jsval", ptr)
  }

JS_ValueToObject = function(con, val, ret)
  {
    .Call("R_JS_ValueToObject", con , val , ret)
  }

JS_ValueToString = function(con, val)
  {
    .Call("R_JS_ValueToJSString", con , val)
  }

JS_ConvertValue = function(con, val, type, ret)
{
  .Call("R_JS_ConvertValue", con, val, type, ret)
}

JS_AddRoot = function(con, ptr)
{
  if (class(ptr) == "jsvalRef")
    type = 0L
  else if (class(ptr) == "JSObjectRef")
    type = 1L
  else if (class(ptr) == "JSStringRef")
    type = 2L
  else
    stop(paste("ptr has unsupported class:", class(ptr)))
  res = .Call("R_JS_AddRoot", con, ptr, type)
  fin = function(exptr)
  {
    .Call("R_Do_Finalizer", con, exptr, type)
  }
  reg.finalizer(ptr@ref, fin, TRUE)
  res
}

JS_RemoveRoot = function(con, ptr)
{
  if (class(ptr) == "jsvalRef")
    type = 0L
  else if (class(ptr) == "JSObjectRef")
    type = 1L
  else if (class(ptr) == "JSStringRef")
    type = 2L
  else
    stop(paste("ptr has unsupported class:", class(ptr)))
  .Call("R_JS_RemoveRoot", con, ptr, type)
}

JS_GetGlobalObject = function(con, addRoot = FALSE, ...)
  {
    glob = .Call("R_JS_GetGlobalObject", con)
    if(addRoot)
      JS_AddRoot(con, glob)
    glob
  }

jsVal = function(addRoot = FALSE)
  {
    toret = .Call( "R_jsVal", ScriptCon)
    if(addRoot)
      JS_AddRoot(ScriptCon, toret)
    toret

  }

JS_EvaluateScript = function(con, parent = NULL, script, numchars, title, linenum, ret = jsVal())
  {
    if (is.null(parent))
      parent = JS_GetGlobalObject(con)
    .Call("R_EvaluateScript", con, parent, script, as.integer(numchars), title, linenum, ret)
    return(TRUE)
  }
