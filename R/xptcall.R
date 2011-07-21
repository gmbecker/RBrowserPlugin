
.xpcomCall = .xptCall =
function(obj, method, ..., .args = list(...), inout, info = NULL, types)
{
    #XXX get the nsIInterfaceInfo for this method if available (i.e. if the xpt is present)
    # and use that to compute the types, which are in and out params
   outIndices = seq(length = length(inout))[!inout]
     # Perhaps create the array of variants and fill them in via R code and pass that directly.
   .Call("R_XPT_InvokeByIndex", obj, as.integer(method), .args, outIndices) # inout, types, outIndices)
}
