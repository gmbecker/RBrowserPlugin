
setClass("JSValueRef", contains = "RCReference")
setClass("NPNFunctionsRef", contains = "RCStructReference")
setClass("PluginInstance", representation=representation(funcs = "NPNFunctionsRef"), contains="RCStructReference")

setClass("NPObjectRef", contains="RCStructReference")
setClass("NPVariantRef", contains="RCStructReference")

setIs("NPVariantRef", "JSValueRef")

setClass("JSRaphaelRef", contains="JSValueRef")
setClass("JSPaperRef", contains="JSValueRef")
setClass("JSDOMRef", contains="JSValueRef")

