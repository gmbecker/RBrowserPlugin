if(FALSE)
  {
setClass("RC++NativeReference", representation(ref =  "externalptr"))

setClass("nsPIDOMWindow", contains = "RC++NativeReference")

setClass("JavaScriptRef", contains = "RC++NativeReference")

setClass("nsINode", contains = "JavaScriptRef")
setClass("nsIDocument", contains = "nsINode")

setAs("JavaScriptRef", "nsIDocument",
      function(from)
        nsIDocument(from))

setClass("XPCOMInterfaceInfo",
          representation(name = "character",
                         parent = "character",
                         iid = "character", #XXX use class from Ruuid package.
                         methods = "list",
                         constants = "list",
                         isScriptable = "logical",
                         isFunction = "logical"))

setClass("XPTConstant",
          representation(name = "character",
                         type = "ANY",
                         value = "ANY"
                         ))

#added by Gabe
setClass("nsIWebBrowser", contains = "RC++NativeReference")
setClass("nsIScriptContext", contains = "RC++NativeReference")
setClass("MozGtkBrowser", contains = "RC++NativeReference")
setClass("JSContextRef", contains = "RC++NativeReference")

}


setClass("NPObjectRef", contains = "RCStructReference")
setClass("NPVariantRef", contains = "RCReference")
setClass("NPDOMWindowRef", contains = "NPVariantRef")
setClass("NPNFunctionsRef", contains = "RCStructReference")
setClass("PluginInstance", representation=representation(funcs = "NPNFunctionsRef"), contains="RCStructReference")

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
