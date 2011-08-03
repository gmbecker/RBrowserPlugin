.onLoad = function(libname, pkgname)
  {
    print("RFirefox loaded") #XXX remove this for release
  #  assignInNamespace("calljsmeth", getNativeSymbolInfo("R_Call_JS_Method")$address, ns = "RFirefox") 
  }
