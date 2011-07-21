
set_JS_Property = function(con, obj, name, val)
  {
    .Call("R_SetPropertyCh", con, obj, name, val)
  }

get_JS_Property = function(con, obj, name)
  {
    toret = jsVal()
    JS_AddRoot(con, toret)
    .Call("R_GetPropertyCh", con, obj, name, toret)
    toret
  }

calljsmeth <- NULL

call_JS_Method = function(con, obj, name, args, addRoot = TRUE, multArgs = TRUE)
  {
    if (!is(args, "list"))
      {
        if(multArgs)
          args = as.list(args)
        else
          list(args)
      }
    if(class(obj) == "jsvalRef")
      isval = TRUE
    else
      isval = FALSE
    
    types = sapply(args,
      function(x)
      {
        if(is(x, "character"))
          1L
        else if (is(x, "integer"))
          2L
        else if (is(x, "numeric"))
          3L
        else if (class(x) == "jsvalRef")
          4L
        else
          stop("unsupported class detected: ", class(x))
      })
    out = jsVal()
    JS_AddRoot(con, out)
    #.Call("R_Call_JS_Method", con, obj, name, args, types, isval, out)
    .Call(calljsmeth, con, obj, name, args, types, isval, out)
    if(!addRoot)
      {
        rm(out)
        return(TRUE)
      } else {
        return(out)
      }
  }

getElementById = function(ScriptCon, id)
  {
    src = paste("document.getElementById('", id, "');", sep="")
    toret = jsVal()
    
    JS_EvaluateScript(ScriptCon, JS_GetGlobalObject(ScriptCon, returnInputs=FALSE), src, nchar(src), paste("geting", id), 1, toret)
    JS_AddRoot(ScriptCon, toret)
    toret
  }
