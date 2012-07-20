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
