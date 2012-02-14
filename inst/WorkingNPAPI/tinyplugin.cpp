#include <npapi.h>
#include <npruntime.h>
#include <npfunctions.h>
#include <stdio.h>
// Register mime types and description for UNIX
// (Windows declares it in resources)
// Plugin's mime types
#define MIME_TYPE_DESCRIPTION "application/sample-plugin:file-extension:Description"
const char* NP_GetMIMEDescription() {
  return MIME_TYPE_DESCRIPTION;
}
NPError NP_GetEntryPoints(NPPluginFuncs* pFuncs);
// Plugin's name and description
NPError OSCALL NP_GetValue(void*, NPPVariable, void* out) {
  return NPERR_NO_ERROR;
}
// Initializes plugin
NPError NP_Initialize(NPNetscapeFuncs* npnfuncs, NPPluginFuncs* nppfuncs) {
  if(npnfuncs == NULL)
    return NPERR_INVALID_FUNCTABLE_ERROR;
  fprintf(stderr, "Plugin Initialized:");fflush(stderr);
  // TODO save npnfuncs pointer
  NP_GetEntryPoints(nppfuncs);
  return NPERR_NO_ERROR;
}
// Set table of functions called by browser.
NPError NP_GetEntryPoints(NPPluginFuncs* pFuncs) {
  if (pFuncs == NULL)
    return NPERR_INVALID_FUNCTABLE_ERROR;
  pFuncs->version       = (NP_VERSION_MAJOR << 8) | NP_VERSION_MINOR;
  pFuncs->newp          = NPP_New;
  pFuncs->javaClass     = NULL;
  return NPERR_NO_ERROR;
}
// new plugin instance
NPError NPP_New(NPMIMEType    pluginType,
                NPP instance, uint16_t mode,
                int16_t argc,  char *argn[],
                char *argv[], NPSavedData *saved) {
  fprintf(stderr, "NPP_New");fflush(stderr);
  return NPERR_NO_ERROR;
}
