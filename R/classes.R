
setClass("JSValueRef", contains = "RCReference")
setClass("NPNFunctionsRef", contains = "RCStructReference")
setClass("PluginInstance", representation=representation(funcs = "NPNFunctionsRef"), contains="RCStructReference")

setClass("NPObjectRef", contains="RCStructReference")
setClass("NPVariantRef", contains="RCStructReference")

setIs("NPVariantRef", "JSValueRef")

if(FALSE)
{
setClass("NPObjectRef", contains = "RCStructReference")
setClass("NPVariantRef", contains = "RCReference")
setClass("NPDOMWindowRef", contains = "NPVariantRef")



setClass('JSObjectRef', contains = "RCStructReference")
setClass('jsvalRef', contains = "RCStructReference")
setClass('JSContextRef', contains = "RCStructReference")


setClassUnion("JSRefUnion", c("jsvalRef", "NPVariantRef"))
setClass("JSValueRef", contains = "RCStructReference", representation= representation(value = "JSRefUnion", backend="character"))

setAs("NPVariantRef", "JSValueRef",
      function(object)
      {
        new("JSValueRef", value=object, backend="NPAPI")
      })

setAs("jsvalRef", "JSValueRef",
      function(object)
      {
        new("JSValueRef", value=object, backend="RFirefox")
      })

#XXX this doesn't appear to be working...
setIs("JSValueRef", "NPVariantRef")
setIs( "JSValueRef", "jsvalRef")
}
