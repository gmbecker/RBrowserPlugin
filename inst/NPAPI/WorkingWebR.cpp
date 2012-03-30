
#include <WebR.h>

// Register mime types and description for UNIX
// (Windows declares it in resources)
// Plugin's mime types
#define MIME_TYPE_DESCRIPTION "application/r:R:WebR Plugin"
const char* NP_GetMIMEDescription() {
  return MIME_TYPE_DESCRIPTION;
}
NPError NP_GetEntryPoints(NPPluginFuncs* pFuncs);
// Plugin's name and description
NPError OSCALL NP_GetValue(void* instance, NPPVariable variable, void* out) {
   fprintf(stderr, "In NP_GetValue\n");fflush(stderr);

  NPError rv = NPERR_NO_ERROR;

  switch (variable) 
    {
    case NPPVpluginNameString:
      *((char **)out) = "WebR Plugin";
      break;
      
    case NPPVpluginDescriptionString:
      *((char **)out) = "WebR plugin";
      break;      
    default:
      rv = NPERR_GENERIC_ERROR;
      break;
    }
  
  return rv;
  
 return NPERR_NO_ERROR;
}
static int isInitialized=0;
int initR( const char **args, int nargs);
// Initializes plugin

NPError NP_Initialize(NPNetscapeFuncs* npnfuncs, NPPluginFuncs* nppfuncs) {
  fprintf(stderr, "In NP_Initialize");fflush(stderr);

if (nppfuncs->size < (offsetof(NPPluginFuncs, setvalue) + sizeof(void*)))
    return NPERR_INVALID_FUNCTABLE_ERROR;

  const char *arg = "R";
    
  
  initR( &arg , 1);
  myNPNFuncs = (NPNetscapeFuncs *)malloc(sizeof(NPNetscapeFuncs));
  CopyNPNFunctions(myNPNFuncs, npnfuncs);
  NP_GetEntryPoints(nppfuncs);
  return NPERR_NO_ERROR;
}

//http://code.firebreath.org/browse/FireBreath/src/NpapiCore/NpapiTypes.cpp?r2=fff31b14375229e4980e98a228250ab20db1dfe7&r1=983c31dfa9f348902c523c2304d777f3552ebc0b&r=09bf0acf08470e56ec9170fcc83935fe4a332443
void CopyNPNFunctions(NPNetscapeFuncs *dstFuncs, NPNetscapeFuncs *srcFuncs)
{
  dstFuncs->size = srcFuncs->size;
  dstFuncs->version = srcFuncs->version;
  dstFuncs->geturl = srcFuncs->geturl;
  dstFuncs->posturl = srcFuncs->posturl;
  dstFuncs->requestread = srcFuncs->requestread;
  dstFuncs->newstream = srcFuncs->newstream;
  dstFuncs->write = srcFuncs->write;
  dstFuncs->destroystream = srcFuncs->destroystream;
  dstFuncs->status = srcFuncs->status;
  dstFuncs->uagent = srcFuncs->uagent;
  dstFuncs->memalloc = srcFuncs->memalloc;
  dstFuncs->memfree = srcFuncs->memfree;
  dstFuncs->memflush = srcFuncs->memflush;
  dstFuncs->reloadplugins = srcFuncs->reloadplugins;
  dstFuncs->geturlnotify = srcFuncs->geturlnotify;
  dstFuncs->posturlnotify = srcFuncs->posturlnotify;
  dstFuncs->getvalue = srcFuncs->getvalue;
  dstFuncs->setvalue = srcFuncs->setvalue;
  dstFuncs->invalidaterect = srcFuncs->invalidaterect;
  dstFuncs->invalidateregion = srcFuncs->invalidateregion;
  dstFuncs->forceredraw = srcFuncs->forceredraw;
  dstFuncs->getstringidentifier = srcFuncs->getstringidentifier;
  dstFuncs->getstringidentifiers = srcFuncs->getstringidentifiers;
  dstFuncs->getintidentifier = srcFuncs->getintidentifier;
  dstFuncs->identifierisstring = srcFuncs->identifierisstring;
  dstFuncs->utf8fromidentifier = srcFuncs->utf8fromidentifier;
  dstFuncs->intfromidentifier = srcFuncs->intfromidentifier;
  dstFuncs->createobject = srcFuncs->createobject;
  dstFuncs->retainobject = srcFuncs->retainobject;
  dstFuncs->releaseobject = srcFuncs->releaseobject;
  dstFuncs->invoke = srcFuncs->invoke;
  dstFuncs->invokeDefault = srcFuncs->invokeDefault;
  dstFuncs->evaluate = srcFuncs->evaluate;
  dstFuncs->getproperty = srcFuncs->getproperty;
  dstFuncs->setproperty = srcFuncs->setproperty;
  dstFuncs->removeproperty = srcFuncs->removeproperty;
  dstFuncs->hasproperty = srcFuncs->hasproperty;
  dstFuncs->hasmethod = srcFuncs->hasmethod;
  dstFuncs->releasevariantvalue = srcFuncs->releasevariantvalue;
  dstFuncs->setexception = srcFuncs->setexception;
  dstFuncs->construct = srcFuncs->construct;
}


// Set table of functions called by browser.
NPError NP_GetEntryPoints(NPPluginFuncs* pFuncs) {
  fprintf(stderr, "In NP_GetEntryPoints");fflush(stderr);
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
  fprintf(stderr, "In NPP_GetValue\n");fflush(stderr);
  //  if(instance == NULL)
  //  return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;

  switch (variable) 
    {
    case NPPVpluginWindowBool:
      *((bool *)value) = true;
      break;
      
    case NPPVpluginNameString:
      *((char **)value) = "WebR Plugin";
      break;
      
    case NPPVpluginDescriptionString:
      *((char **)value) = "WebR plugin";
      break;
      
    case NPPVpluginScriptableNPObject:
      {
	fprintf(stderr, "case NPPVpluginScriptableNPObject");fflush(stderr);
	/*CPlugin * plugin = (CPlugin *)instance->pdata;
	if(plugin == NULL)
	  return NPERR_GENERIC_ERROR;
	fprintf(stderr, "plugin not NULL\n");fflush(stderr);
	
	if (!plugin->isInitialized())
	  {
	    return NPERR_GENERIC_ERROR;
	  }
	*((NPObject **)value) = plugin->GetScriptableObject();
	*/
	if(!instance->pdata)
	  {
	    //Not a good permanent fix!!
	    fprintf(stderr, "myNPNFuncs->createobject %lx\n&WebREngine::_npclass %lx", myNPNFuncs->createobject, &WebREngine::_npclass);fflush(stderr);
	      instance->pdata = myNPNFuncs->createobject(instance, &WebREngine::_npclass); 
		//instance->pdata = (NPObject *) new WebREngine(instance);
	    fprintf(stderr, "Scriptable object created %lx", instance->pdata);fflush(stderr);
	    myNPNFuncs->retainobject((NPObject *) instance->pdata);
	  }
	

	*((NPObject **)value) = (NPObject *) instance->pdata;
      }
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
