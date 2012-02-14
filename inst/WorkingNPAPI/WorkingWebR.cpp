/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
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
 * the provisions above, a recipient may use your version of this file underb
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "WorkingWebR.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <R.h>
#include <Rdefines.h>
#include <Rembedded.h>
#include <plugin.h>

//extern "C" {

#define PLUGIN_NAME        "WebR Plug-in"
#define PLUGIN_DESCRIPTION PLUGIN_NAME " WebR"
#define PLUGIN_VERSION     "0.1.0.0"

static NPNetscapeFuncs* sBrowserFuncs = NULL;

typedef struct InstanceData {
  NPP npp;
  NPWindow window;
} InstanceData;

/*
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

  const char* uaString = sBrowserFuncs->uagent(npp);
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
*/

int initR( const char **args, int nargs);

static int isInitialized=0;
NP_EXPORT(NPError)
NP_Initialize(NPNetscapeFuncs* bFuncs, NPPluginFuncs* pFuncs)
{
  sBrowserFuncs = bFuncs;

  // Check the size of the provided structure based on the offset of the
  // last member we need.
  if (pFuncs->size < (offsetof(NPPluginFuncs, setvalue) + sizeof(void*)))
    return NPERR_INVALID_FUNCTABLE_ERROR;

  const char *arg = "R";
    
  initR( &arg , 1);

  NP_GetEntryPoints(pFuncs);
  return NPERR_NO_ERROR;
}

  NPError NP_GetEntryPoints(NPPluginFuncs *pFuncs)
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


  return NPERR_NO_ERROR;
  }

NP_EXPORT(char*)
NP_GetPluginVersion()
{
  return PLUGIN_VERSION;
}

NP_EXPORT(const char*)
NP_GetMIMEDescription()
{
  return "application/WebR:R:WebR plugin";
}

NP_EXPORT(NPError)
NP_GetValue(void* future, NPPVariable aVariable, void* aValue) {
  fprintf(stderr, "in NP_GetValue\n");fflush(stderr);
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
  fprintf(stderr, "In NP_Shutdown");fflush(stderr);
  return NPERR_NO_ERROR;
}

NPError
NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved) {
  fprintf(stderr, "In NPP_New\n");fflush(stderr);
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
  fprintf(stderr, "In NPP_Destroy");fflush(stderr);
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
  fprintf(stderr, "In NPP_HandleEvent");fflush(stderr);
  /*
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
  */
  return 0;
}

void
NPP_URLNotify(NPP instance, const char* URL, NPReason reason, void* notifyData) {

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

//} //extern "C"
