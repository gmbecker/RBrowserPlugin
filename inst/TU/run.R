library(RGCCTranslationUnit)
tu = parseTU("xptcvariant.cc.001t.tu")
k = getClassNodes(tu)
names(k)
enums = getEnumerations(tu)
names(enums)
rk = resolveType(k, tu)
names(rk)
names(rk[["XPTConstValue"]])
rk[["XPTConstValue"]]
class(rk[["XPTConstValue"]])
getClass(class(rk[["nsXPTCVariant"]]))
rk[["nsXPTCVariant"]]@baseClasses
rk[["nsXPTCVariant"]]@ancestorClasses
names(rk[["nsXPTCVariant"]]@methods)
names(rk[["nsXPTCVariant"]]@fields)


m = getClassMethods(k[["nsXPTCVariant"]])
names(m)
rm = resolveType(m, tu)

sapply(rm, function(x) length(x$parameters))


o = RGCCTranslationUnit:::createClassBindings(k[["nsXPTCVariant"]], tu)

