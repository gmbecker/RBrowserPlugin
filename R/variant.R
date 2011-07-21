#
# This file provides facilities to work with nsXPTCVariant objects.
# These are used in XPTCalls using "reflection" (we don't actually need it
#  to do the call, but the user has to know what they are doing!)
# More specifically, these are used to specify arguments for use in
# dynamic calls that are not compiled.
#
#  One can create variants and set their values. At present, some care has to be
#  taken to specify the correct type of values. We can correct this in the future.


setClass("nsXPTCVariant", contains = "RC++Reference")

variant = nsXPTCVariant =
function(val, type, flags = 0L, duplicate = FALSE) # fix duplicate to be based on type.
{
  ans = .Call("R_new_nsXPTCVariant", PACKAGE = "RFirefox")
  if(nargs() == 0)
    return(ans)
  
  setVariant(ans, val,flags, type, duplicate)
  ans
}

setVariant =
function(x, val, type, flags = 0L, duplicate = FALSE) # fix duplicate to be based on type.
{
  .Call("R_set_nsXPTCVariant",
          x@ref, val, as(flags, "integer"),
           as(type, "integer"), as.logical(duplicate), PACKAGE = "RFirefox")
}


getVariant =
function(x)
{
   .Call("R_convertVariantToR", x@ref, PACKAGE = "RFirefox")
}

if(!isGeneric("type<-"))
 setGeneric("type<-",
           function(x, ..., value)
            standardGeneric("type<-"))

setMethod("type<-", "nsXPTCVariant",
           function(x, ..., value) {
             .Call("") # not there yet.
             x
           })
