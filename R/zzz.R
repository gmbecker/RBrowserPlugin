# Temporary while we don't use a NAMESPACE.
.First.lib =
function(lib, pkg)
{
 library.dynam("RFirefox", pkg, lib)
 .Call("R_XPCOMGlueStartup")
 # srv = .Call("R_GetServiceManager")
}
