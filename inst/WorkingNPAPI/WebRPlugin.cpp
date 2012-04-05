/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is Rdistributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Josh Aas <josh@mozilla.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "BasicPlugin.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <unistd.h>

#define PLUGIN_NAME        "Test R Plugin"
#define PLUGIN_DESCRIPTION PLUGIN_NAME " Working up to WebR"
#define PLUGIN_VERSION     "1.0.0.0"

NPNetscapeFuncs *myNPNFuncs;

typedef struct InstanceData {
  NPP npp;
  NPWindow window;
  NPObject *scriptable;
  NPNetscapeFuncs *funcs;
} InstanceData;

static void
drawWindow(InstanceData* instanceData, GdkDrawable* gdkWindow)
{
  NPWindow window = instanceData->window;
  int x = window.x;
  int y = window.y;
  int width = window.width;
  int height = window.height;

  NPP npp = instanceData->npp;
  if (!npp)
    return;

  const char* uaString = myNPNFuncs->uagent(npp);
  if (!uaString)
    return;

  GdkGC* gdkContext = gdk_gc_new(gdkWindow);

  // draw a grey background for the plugin frame
  GdkColor grey;
  grey.red = grey.blue = grey.green = 32767;
  gdk_gc_set_rgb_fg_color(gdkContext, &grey);
  gdk_draw_rectangle(gdkWindow, gdkContext, TRUE, x, y, width, height);

  // draw a 3-pixel-thick black frame around the plugin
  GdkColor black;
  black.red = black.green = black.blue = 0;
  gdk_gc_set_rgb_fg_color(gdkContext, &black);
  gdk_gc_set_line_attributes(gdkContext, 3, GDK_LINE_SOLID, GDK_CAP_NOT_LAST, GDK_JOIN_MITER);
  gdk_draw_rectangle(gdkWindow, gdkContext, FALSE, x + 1, y + 1,
                     width - 3, height - 3);

  // paint the UA string
  PangoContext* pangoContext = gdk_pango_context_get();
  PangoLayout* pangoTextLayout = pango_layout_new(pangoContext);
  pango_layout_set_width(pangoTextLayout, (width - 10) * PANGO_SCALE);
  pango_layout_set_text(pangoTextLayout, uaString, -1);
  gdk_draw_layout(gdkWindow, gdkContext, x + 5, y + 5, pangoTextLayout);
  g_object_unref(pangoTextLayout);

  g_object_unref(gdkContext);
}

static int isInitialized=0;
int initR( const char **args, int nargs);

NP_EXPORT(NPError)
NP_Initialize(NPNetscapeFuncs* bFuncs, NPPluginFuncs* pFuncs)
{
  fprintf(stderr, "in NP_Initialize");fflush(stderr);

  const char *arg[] = {"R", "--no-save"};
  initR( &arg[0] , 2);
  // Check the size of the provided structure based on the offset of the
  // last member we need.
  if (pFuncs->size < (offsetof(NPPluginFuncs, setvalue) + sizeof(void*)))
    return NPERR_INVALID_FUNCTABLE_ERROR;

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

  myNPNFuncs = (NPNetscapeFuncs *)malloc(sizeof(NPNetscapeFuncs));
  CopyNPNFunctions(myNPNFuncs, bFuncs);

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


NP_EXPORT(char*)
NP_GetPluginVersion()
{
  return PLUGIN_VERSION;
}

NP_EXPORT(const char*)
NP_GetMIMEDescription()
{
  return "application/test-r:R:Test R Plugin";
}

NP_EXPORT(NPError)
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

NP_EXPORT(NPError)
NP_Shutdown()
{
  return NPERR_NO_ERROR;
}

NPError
NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved) {
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
 
  
  SEXP klass, ans, ptr;
  SEXP klass2, ans2, ptr2;
  PROTECT( klass = MAKE_CLASS( "PluginInstance" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( ptr = R_MakeExternalPtr( instance,
                                    Rf_install( "NPP" ),
                                    R_NilValue));
  
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
  UNPROTECT(3);
      

   return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData** save) {
  InstanceData* instanceData = (InstanceData*)(instance->pdata);
  free(instanceData);
  //NPN_MemFree(instance->pdata);
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
  InstanceData *instanceData = (InstanceData*)(instance->pdata);
  XEvent *nativeEvent = (XEvent*)event;

  if (nativeEvent->type != GraphicsExpose)
    return 0;

  XGraphicsExposeEvent *expose = &nativeEvent->xgraphicsexpose;
  instanceData->window.window = (void*)(expose->drawable);

  GdkNativeWindow nativeWinId = (XID)(instanceData->window.window);
  GdkDrawable* gdkWindow = GDK_DRAWABLE(gdk_window_foreign_new(nativeWinId));  
  drawWindow(instanceData, gdkWindow);
  g_object_unref(gdkWindow);

  return 1;
}

void
NPP_URLNotify(NPP instance, const char* URL, NPReason reason, void* notifyData) {

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

NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value) {
  return NPERR_GENERIC_ERROR;
}


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
