isRefClass = function(obj)
  {
    is(obj, "envRefClass")
  }

getFields = function(obj)
  {
    obj$getRefClass()$fields()
  }

getMethods = function(obj)
  {
    obj$getRefClass()$methods()
  }

getSlotNames = function(obj)
  {
    names(getSlots(class(obj)))
  }

checkForSlot = function(obj, name)
  {
    slots = getSlotNames(obj)
    return(name %in% slots)
  }
checkForField = function(obj, name)
  {
    fields = getFields(obj)
    return(name %in% names(fields))
  }

checkForMethod = function(obj, name)
  {
    methods = getMethods(obj)
    return(name %in% methods)
  }

checkForFieldorMethod = function(obj, name)
  {

    return(checkForField(obj, name) || checkForMethod(obj, name))
  }

getJSMethodObj = function(obj)
  {
    ret= NULL
    if(is(obj, "function"))
      ret = attr(obj, "NPRef")
    
    else if(is(obj, "JSValueRef"))
      ret = obj
    else
      stop("Unable to find a JSValueRef associated with obj")
    ret
  }

CheckForProperty = function(obj, prop)
  {
    tmp = TRUE
    ret = FALSE
    if (isRefClass(obj))
      ret = checkForFieldorMethod(obj, prop)
    else if (isS4(obj))
      ret = checkForSlot(obj, prop)

    
    if(!ret)
      {
        if( is(prop, "integer") || (is(prop, "numeric") && prop == floor(prop)))
          ret = prop > 0 && prop <= length(obj)
        else if (is(prop, "character"))
          ret = prop %in% names(obj)
        else
          stop(paste("Cannot specify properties to check for via class", class(prop)))
      }
    return(ret)
  }
