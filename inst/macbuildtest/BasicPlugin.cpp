/* ***** BEGIN LICENSE BLOCK *****
 *
 * THIS FILE IS PART OF THE MOZILLA NPAPI SDK BASIC PLUGIN SAMPLE
 * SOURCE CODE. USE, DISTRIBUTION AND REPRODUCTION OF THIS SOURCE
 * IS GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS 
 * SOURCE IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.
 *
 * THE MOZILLA NPAPI SDK BASIC PLUGIN SAMPLE SOURCE CODE IS
 * (C) COPYRIGHT 2008 by the Mozilla Corporation
 * http://www.mozilla.com/
 *
 * Contributors:
 *  Josh Aas <josh@mozilla.com>
 *
 * ***** END LICENSE BLOCK ***** */

/*
 * This sample plugin uses the Cocoa event model and the Core Graphics
 * drawing model.
 */

#include "WebR.h"

/* Structure containing pointers to functions implemented by the browser. */
NPNetscapeFuncs *myNPNFuncs;

#define PLUGIN_NAME        "Test R Plugin"
#define PLUGIN_DESCRIPTION PLUGIN_NAME " Working up to WebR"
#define PLUGIN_VERSION     "1.0.0.0"

typedef struct InstanceData {
  NPP npp;
  NPWindow window;
  NPObject *scriptable;
  NPNetscapeFuncs *funcs;
} InstanceData;
static int isInitialized=0;


int initR( const char **args, int nargs)
{
  char **rargs;
  if(isInitialized)
    return 0;
  
  rargs = (char **) malloc(nargs * sizeof(char *));
  for(int i = 0 ; i < nargs; i++)
    rargs[i] = strdup(args[i]);
  fprintf(stderr, "Attempting to start embedded R.\n");fflush(stderr);
  Rf_initEmbeddedR(nargs, rargs);
  fprintf(stderr, "R initialization done.\n"); fflush(stderr);
  int error=0;
  SEXP call;
  PROTECT(call = allocVector(LANGSXP, 2));
  SETCAR(call, Rf_install("library"));
  SETCAR(CDR(call), Rf_install("RFirefox"));
  R_tryEval(call, R_GlobalEnv, &error);
  
  UNPROTECT(1);
  return error;
}




NPError OSCALL NP_Initialize(NPNetscapeFuncs* bFuncs
#ifdef XP_UNIX
                             , NPPluginFuncs* pFuncs
#endif
                             )
{
  fprintf(stderr, "in NP_Initialize");fflush(stderr);
  //fprintf(logfile, "in NP_Initialize");fflush(logfile);
  const char *arg[] = {"R", "--no-save"};
  initR( &arg[0] , 2);
  // Check the size of the provided structure based on the offset of the
  // last member we need.

#ifdef XP_UNIX
  if (pFuncs->size < (offsetof(NPPluginFuncs, setvalue) + sizeof(void*)))
    return NPERR_INVALID_FUNCTABLE_ERROR;
  
  SetNPPFuncs(pFuncs);
#endif
  myNPNFuncs = (NPNetscapeFuncs *)malloc(sizeof(NPNetscapeFuncs));
  CopyNPNFunctions(myNPNFuncs, bFuncs);

  return NPERR_NO_ERROR;
}

void SetNPPFuncs(NPPluginFuncs *pFuncs)
{
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

NPError NP_GetEntryPoints(NPPluginFuncs* pluginFuncs)
{
  //fprintf(logfile, "in NP_GetEntryPoints");fflush(logfile);
  SetNPPFuncs(pluginFuncs);
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
  
  if (srcFuncs->version >= NPVERS_MACOSX_HAS_COCOA_EVENTS) { // 23 
    dstFuncs->scheduletimer = srcFuncs->scheduletimer;  
    dstFuncs->unscheduletimer = srcFuncs->unscheduletimer;
  }

  if(srcFuncs->version >= NPVERS_HAS_URL_AND_AUTH_INFO) { // 21
    dstFuncs->getvalueforurl = srcFuncs->getvalueforurl;
    dstFuncs->setvalueforurl = srcFuncs->setvalueforurl;
    dstFuncs->getauthenticationinfo = srcFuncs->getauthenticationinfo;
  }
  
 
}




char*
NP_GetPluginVersion()
{
  return PLUGIN_VERSION;
}


const char*
NP_GetMIMEDescription()
{
  return "application/test-r:R:Test R Plugin";
}


NPError
NP_GetValue(void* future, NPPVariable aVariable, void* aValue) {
  switch (aVariable) {
    case NPPVpluginNameString:
      *((char**)aValue) = PLUGIN_NAME;
      break;
    case NPPVpluginDescriptionString:
      *((char**)aValue) = PLUGIN_DESCRIPTION;
      break;
    default:
      return NPERR_INVALID_PARAM;
      break;
  }
  return NPERR_NO_ERROR;
}


/* Local store of the browser UA string that we we paint into the plugin's window. */
//static CFStringRef browserUAString = NULL;


//void drawPlugin(NPP instance, NPCocoaEvent* event);

/* Function called once by the browser to shut down the plugin. */
NPError NP_Shutdown(void)
{
  //CFRelease(browserUAString);
  //browserUAString = NULL;
	return NPERR_NO_ERROR;
}

/* Called to create a new instance of the plugin. */
NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved)
{

 // Make sure we can render this plugin
  NPBool browserSupportsWindowless = false;
  myNPNFuncs->getvalue(instance, NPNVSupportsWindowless, &browserSupportsWindowless);
  if (!browserSupportsWindowless) {
    printf("Windowless mode not supported by the browser\n");
    return NPERR_GENERIC_ERROR;
  }

  myNPNFuncs->setvalue(instance, NPPVpluginWindowBool, (void*)false);

  
  // set up our our instance data
  InstanceData* instanceData = (InstanceData*)malloc(sizeof(InstanceData));
  if (!instanceData)
    return NPERR_OUT_OF_MEMORY_ERROR;
  memset(instanceData, 0, sizeof(InstanceData));
  instanceData->npp = instance;
  instanceData->scriptable = NULL;
  instanceData->funcs = myNPNFuncs;
  instance->pdata = instanceData;
 
 #ifdef XP_MACOSX
  /* Select the Core Graphics drawing model. */
  NPBool supportsCoreGraphics = false;
  if (myNPNFuncs->getvalue(instance, NPNVsupportsCoreGraphicsBool, &supportsCoreGraphics) == NPERR_NO_ERROR && supportsCoreGraphics) {
    myNPNFuncs->setvalue(instance, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreGraphics);
  } else {
    printf("CoreGraphics drawing model not supported, can't create a plugin instance.\n");
    return NPERR_INCOMPATIBLE_VERSION_ERROR;
  }

  /* Select the Cocoa event model. */
  NPBool supportsCocoaEvents = false;
  if (myNPNFuncs->getvalue(instance, NPNVsupportsCocoaBool, &supportsCocoaEvents) == NPERR_NO_ERROR && supportsCocoaEvents) {
    myNPNFuncs->setvalue(instance, NPPVpluginEventModel, (void*)NPEventModelCocoa);
  } else {
    printf("Cocoa event model not supported, can't create a plugin instance.\n");
    return NPERR_INCOMPATIBLE_VERSION_ERROR;
  }
#endif //XP_MACOSX
  /*
  if (!browserUAString) {
    const char* ua = myNPNFuncs->uagent(instance);
    if (ua) {
      browserUAString = CFStringCreateWithCString(kCFAllocatorDefault, ua, kCFStringEncodingASCII);
    }
  }
  */

  SEXP klass, ans, ptr;
  SEXP klass2, ans2, ptr2;
	
  PROTECT( klass = MAKE_CLASS( "PluginInstance" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( ptr = R_MakeExternalPtr( instance,
                                    Rf_install( "NPP" ),
                                    R_NilValue));
//	UNPROTECT(3);

  PROTECT( klass2 = MAKE_CLASS( "NPNFunctionsRef" ) );
  PROTECT( ans2 = NEW( klass2 ) );
  PROTECT( ptr2 = R_MakeExternalPtr( myNPNFuncs,
                                     Rf_install("NPNFuncs"),
                                     R_NilValue));
  SET_SLOT( ans2, Rf_install("ref"), ptr2);
  SET_SLOT( ans, Rf_install("funcs"), ans2);
  //finalizer here if needed
  
  SET_SLOT(ans, Rf_install("ref"), ptr);
  Rf_defineVar(Rf_install("PluginInstance"), ans, R_GlobalEnv);
  UNPROTECT(6);
  


  return NPERR_NO_ERROR;
}

/* Called to destroy an instance of the plugin. */
NPError NPP_Destroy(NPP instance, NPSavedData** save)
{
  free(instance->pdata);

  return NPERR_NO_ERROR;
}

/* Called to update a plugin instances's NPWindow. */
NPError NPP_SetWindow(NPP instance, NPWindow* window)
{
  InstanceData* currentInstance = (InstanceData*)(instance->pdata);

  currentInstance->window = *window;
  
  return NPERR_NO_ERROR;
}

NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
  *stype = NP_ASFILEONLY;
  return NPERR_NO_ERROR;
}

NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
  return NPERR_NO_ERROR;
}

int32_t NPP_WriteReady(NPP instance, NPStream* stream)
{
  return 0;
}

int32_t NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer)
{
  return 0;
}

void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
}

void NPP_Print(NPP instance, NPPrint* platformPrint)
{
  
}

int16_t NPP_HandleEvent(NPP instance, void* event)
{
  /*
  NPCocoaEvent* cocoaEvent = (NPCocoaEvent*)event;
  if (cocoaEvent && (cocoaEvent->type == NPCocoaEventDrawRect)) {
    drawPlugin(instance, (NPCocoaEvent*)event);
    return 1;
  }

  return 0;
  */
  return 1;
}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{

}


NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value) {
  fprintf(stderr, "In NPP_GetValue\n");fflush(stderr);
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

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

  //so I can get into gdb
  //sleep(5);
  fprintf(stderr, "\ninstance: %lx\n", instance);fflush(stderr);
  //fprintf(stderr, "\nmyNPNFuncs: %lx\n", myNPNFuncs);fflush(stderr);
  //fprintf(stderr, "\nmyNPNFuncs->createobject: %lx\n", myNPNFuncs->createobject);fflush(stderr);
	if(!((InstanceData*)instance->pdata)->scriptable)
	  {
      fprintf(stderr, "Attempting to create scriptable object");fflush(stderr);
      ((InstanceData*)instance->pdata)->scriptable = myNPNFuncs->createobject(instance, &WebREngine::_npclass);
      fprintf(stderr, "Scriptable object created %lx", ((InstanceData*)instance->pdata)->scriptable);fflush(stderr);
      myNPNFuncs->retainobject((NPObject *)((InstanceData*)instance->pdata)->scriptable);
      //myNPNFuncs->retainobject(scriptable);
      
      }
	
	*(NPObject **)value = (NPObject *)((InstanceData*)instance->pdata)->scriptable;

      }
      break;
      
    default:
      rv = NPERR_GENERIC_ERROR;
      break;
    }
  fprintf(stderr, "value set to: %lx", *((NPObject **)value));fflush(stderr);
  return rv;
}



NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
  return NPERR_GENERIC_ERROR;
}
#if 0
void drawPlugin(NPP instance, NPCocoaEvent* event)
{
  
  if (!browserUAString) {
    return;
  }

  InstanceData* currentInstance = (InstanceData*)(instance->pdata);
  CGContextRef cgContext = event->data.draw.context;
  if (!cgContext) {
    return;
  }

  float windowWidth = currentInstance->window.width;
  float windowHeight = currentInstance->window.height;

  /* Save the cgcontext gstate. */
  CGContextSaveGState(cgContext);

  /* We get a flipped context. */
  CGContextTranslateCTM(cgContext, 0.0, windowHeight);
  CGContextScaleCTM(cgContext, 1.0, -1.0);

  /* Draw a gray background for the plugin. */
  CGContextAddRect(cgContext, CGRectMake(0, 0, windowWidth, windowHeight));
  CGContextSetGrayFillColor(cgContext, 0.5, 1.0);
  CGContextDrawPath(cgContext, kCGPathFill);

  /* Draw a black frame around the plugin. */
  CGContextAddRect(cgContext, CGRectMake(0, 0, windowWidth, windowHeight));
  CGContextSetGrayStrokeColor(cgContext, 0.0, 1.0);
  CGContextSetLineWidth(cgContext, 6.0);
  CGContextStrokePath(cgContext);

  /* Draw the UA string using ATSUI. */
  CGContextSetGrayFillColor(cgContext, 0.0, 1.0);
  ATSUStyle atsuStyle;
  ATSUCreateStyle(&atsuStyle);
  CFIndex stringLength = CFStringGetLength(browserUAString);
  UniChar* unicharBuffer = (UniChar*)malloc((stringLength + 1) * sizeof(UniChar));
  CFStringGetCharacters(browserUAString, CFRangeMake(0, stringLength), unicharBuffer);
  UniCharCount runLengths = kATSUToTextEnd;
  ATSUTextLayout atsuLayout;
  ATSUCreateTextLayoutWithTextPtr(unicharBuffer,
                                  kATSUFromTextBeginning,
                                  kATSUToTextEnd,
                                  stringLength,
                                  1,
                                  &runLengths,
                                  &atsuStyle,
                                  &atsuLayout);
  ATSUAttributeTag contextTag = kATSUCGContextTag;
  ByteCount byteSize = sizeof(CGContextRef);
  ATSUAttributeValuePtr contextATSUPtr = &cgContext;
  ATSUSetLayoutControls(atsuLayout, 1, &contextTag, &byteSize, &contextATSUPtr);
  ATSUTextMeasurement lineAscent, lineDescent;
  ATSUGetLineControl(atsuLayout,
                    kATSUFromTextBeginning,
                    kATSULineAscentTag,
                    sizeof(ATSUTextMeasurement),
                    &lineAscent,
                    &byteSize);
  ATSUGetLineControl(atsuLayout,
                    kATSUFromTextBeginning,
                    kATSULineDescentTag,
                    sizeof(ATSUTextMeasurement),
                    &lineDescent,
                    &byteSize);
  float lineHeight = FixedToFloat(lineAscent) + FixedToFloat(lineDescent);  
  ItemCount softBreakCount;
  ATSUBatchBreakLines(atsuLayout,
                      kATSUFromTextBeginning,
                      stringLength,
                      FloatToFixed(windowWidth - 10.0),
                      &softBreakCount);
  ATSUGetSoftLineBreaks(atsuLayout,
                        kATSUFromTextBeginning,
                        kATSUToTextEnd,
                        0, NULL, &softBreakCount);
  UniCharArrayOffset* softBreaks = (UniCharArrayOffset*)malloc(softBreakCount * sizeof(UniCharArrayOffset));
  ATSUGetSoftLineBreaks(atsuLayout,
                        kATSUFromTextBeginning,
                        kATSUToTextEnd,
                        softBreakCount, softBreaks, &softBreakCount);
  UniCharArrayOffset currentDrawOffset = kATSUFromTextBeginning;
  int i = 0;
  while (i < softBreakCount) {
    ATSUDrawText(atsuLayout, currentDrawOffset, softBreaks[i], FloatToFixed(5.0), FloatToFixed(windowHeight - 5.0 - (lineHeight * (i + 1.0))));
    currentDrawOffset = softBreaks[i];
    i++;
  }
  ATSUDrawText(atsuLayout, currentDrawOffset, kATSUToTextEnd, FloatToFixed(5.0), FloatToFixed(windowHeight - 5.0 - (lineHeight * (i + 1.0))));
  free(unicharBuffer);
  free(softBreaks);

  /* Restore the cgcontext gstate. */
  CGContextRestoreGState(cgContext);

}
#endif //0
