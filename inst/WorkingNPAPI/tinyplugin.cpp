#include <npapi.h>
#include <npruntime.h>
#include <npfunctions.h>
#include <stdio.h>
#include <R.h>
#include <Rdefines.h>
#include <Rembedded.h>
#include <plugin.h>

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
static int isInitialized=0;
int initR( const char **args, int nargs);
// Initializes plugin
NPError NP_Initialize(NPNetscapeFuncs* npnfuncs, NPPluginFuncs* nppfuncs) {
if (nppfuncs->size < (offsetof(NPPluginFuncs, setvalue) + sizeof(void*)))
    return NPERR_INVALID_FUNCTABLE_ERROR;

  const char *arg = "R";
    
  initR( &arg , 1);

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
  pFuncs->getvalue = NPP_GetValue;
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


NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value) {
  fprintf(stderr, "In NPP_GetValue");fflush(stderr);
	if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;

  if(instance == NULL)
    return NPERR_GENERIC_ERROR;
  
  CPlugin * plugin = (CPlugin *)instance->pdata;
  if(plugin == NULL)
    return NPERR_GENERIC_ERROR;

  switch (variable) 
	{
		case NPPVpluginWindowBool:
			*((bool *)value) = true;
			break;
	
		case NPPVpluginNameString:
			*((char **)value) = "Boilerplate Plugin";
			break;
  
		case NPPVpluginDescriptionString:
			*((char **)value) = "Boilerplate web plugin";
			break;

		case NPPVpluginScriptableNPObject:
      fprintf(stderr, "case NPPVpluginScriptableNPObject");fflush(stderr);
			if (!plugin->isInitialized())
			{
				return NPERR_GENERIC_ERROR;
			}
			
			*((NPObject **)value) = plugin->GetScriptableObject();

			break;
  
		default:
			rv = NPERR_GENERIC_ERROR;
			break;
  }
  
  return rv;
  
}

NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value) {
  fprintf(stderr, "In NPP_SetValue");fflush(stderr);
  return NPERR_GENERIC_ERROR;
}

int initR( const char **args, int nargs)
{
  char **rargs;
  unsigned int i;
  
  if(isInitialized)
    return 0;
  
  rargs = (char **) malloc(nargs * sizeof(char *));
  for(i = 0 ; i < nargs; i++)
    rargs[i] = strdup(args[i]);
  fprintf(stderr, "Attempting to start embedded R.\n");fflush(stderr);
  Rf_initEmbeddedR(nargs, rargs);
  fprintf(stderr, "R initialization done.\n"); fflush(stderr);
  
  return 0;
}
