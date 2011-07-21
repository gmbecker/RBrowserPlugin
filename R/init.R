.onLoad = function(libname, pkgname)
  {
    assignInNamespace("calljsmeth", getNativeSymbolInfo("R_Call_JS_Method")$address, "RFirefox") 
  }
