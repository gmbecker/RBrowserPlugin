#include <WebR.h>
#include <npfunctions.h>
#include <Rembedded.h>
#include <RNPAPI.h>

extern "C" {
  
#define PLUGIN_NAME        "WebR Plug-in"
#define PLUGIN_DESCRIPTION PLUGIN_NAME " Plugin for embedding R in browsers"
#define PLUGIN_VERSION     "0.1.0.0"
  
  typedef struct InstanceData {
    NPP npp;
    NPWindow window;
  } InstanceData;
  
  static NPNetscapeFuncs* sBrowserFuncs = NULL;
  static int isInitialized = 0;
  int initR( const char **args, int nargs);

  NP_EXPORT(NPError)
  NP_Initialize(NPNetscapeFuncs* bFuncs, NPPluginFuncs* pFuncs)
  {
    const char *arg = "R";
    
    initR( &arg , 1);
    
    pFuncs->newp = NPP_New;
    pFuncs->destroy = NPP_Destroy;
    pFuncs->setwindow = NPP_SetWindow;
    pFuncs->newstream = NPP_NewStream;
    pFuncs->destroystream = NPP_DestroyStream;
    pFuncs->asfile = NPP_StreamAsFile;
    pFuncs->writeready = NPP_WriteReady;
    pFuncs->write = NPP_Write;
    pFuncs->print = NPP_Print;
    pFuncs->event = NPP_HandleEvent;
    pFuncs->urlnotify = NPP_URLNotify;
    pFuncs->getvalue = NPP_GetValue;
    pFuncs->setvalue = NPP_SetValue;
  }
  
  NP_EXPORT(NPError)
  NP_Shutdown()
  {
    Rf_endEmbeddedR(0);
    return NPERR_NO_ERROR;
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
  
  
  NP_EXPORT(char*)
  NP_GetPluginVersion()
  {
    return (char *) PLUGIN_VERSION;
  }
  
  NP_EXPORT(const char*)
  NP_GetMIMEDescription()
  {
    return "application/R-engine:R:WebR plugin";
  }
  
  
  
  NPError
  NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved) {
    // Make sure we can render this plugin
    NPBool browserSupportsWindowless = false;
    sBrowserFuncs->getvalue(instance, NPNVSupportsWindowless, &browserSupportsWindowless);
    if (!browserSupportsWindowless) {
      printf("Windowless mode not supported by the browser\n");
      return NPERR_GENERIC_ERROR;
    }
    
    sBrowserFuncs->setvalue(instance, NPPVpluginWindowBool, (void*)false);
    
    // set up our our instance data
    InstanceData* instanceData = (InstanceData*)malloc(sizeof(InstanceData));
    if (!instanceData)
      return NPERR_OUT_OF_MEMORY_ERROR;
    memset(instanceData, 0, sizeof(InstanceData));
    instanceData->npp = instance;
    instance->pdata = instanceData;
    
    return NPERR_NO_ERROR;
  }
  
  NPError
  NPP_Destroy(NPP instance, NPSavedData** save) {
    InstanceData* instanceData = (InstanceData*)(instance->pdata);
    free(instanceData);
    return NPERR_NO_ERROR;
  }
  
  
  NPError
  NPP_SetWindow(NPP instance, NPWindow* window) {
    InstanceData* instanceData = (InstanceData*)(instance->pdata);
    instanceData->window = *window;
    return NPERR_NO_ERROR;
  }
  
  NPError
  NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype) {
    return NPERR_GENERIC_ERROR;
  }

  NPError
  NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason) {
    return NPERR_GENERIC_ERROR;
  }
  
  int32_t
  NPP_WriteReady(NPP instance, NPStream* stream) {
    return 0;
  }
  
  int32_t
  NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer) {
    return 0;
  }
  
  void
  NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname) {
    
  }
  
  void
  NPP_Print(NPP instance, NPPrint* platformPrint) {
    
  }
  
  int16_t
  NPP_HandleEvent(NPP instance, void* event) {
    return 0;
  }
  
  
  void
  NPP_URLNotify(NPP instance, const char* URL, NPReason reason, void* notifyData) {
    
  }
  
  NP_EXPORT(NPError)
  NPP_GetValue(NPP instance, NPPVariable variable, void *value) {
    return NPERR_GENERIC_ERROR;
  }
  
  NPError
  NPP_SetValue(NPP instance, NPNVariable variable, void *value) {
    return NPERR_GENERIC_ERROR;
  }
  

}
