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


