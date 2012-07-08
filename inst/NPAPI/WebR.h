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
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef WebRPlugin_h_
#define WebRPlugin_h_

#include "signal.h"
#include "npapi.h"
#include "npfunctions.h"
#include <npruntime.h>
#include <stdio.h>
#include "pthread.h"

#define CSTACK_DEFNS 1

#ifndef XP_MACOSX
#include <R.h>
#include <Rdefines.h>
#include <Rembedded.h>
#include <Rinterface.h>
#else
#include <R/R.h>
#include <R/Rdefines.h>
#include <R/Rembedded.h>
#include <R/Rinterface.h>
#endif


void SetNPPFuncs(NPPluginFuncs *pFuncs);
int initR( const char **args, int nargs);

extern "C" {
NPError NP_Initialize(NPNetscapeFuncs* bFuncs
#ifdef XP_UNIX
                      ,NPPluginFuncs* pFuncs
#endif
);

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* pFuncs);


NPError NP_Shutdown();

NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved);

NPError NPP_Destroy(NPP instance, NPSavedData** save);
NPError NPP_SetWindow(NPP instance, NPWindow* window);
NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype);
NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason);
int32_t NPP_WriteReady(NPP instance, NPStream* stream);
int32_t NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer);
void    NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname);
void    NPP_Print(NPP instance, NPPrint* platformPrint);
int16_t NPP_HandleEvent(NPP instance, void* event);
void    NPP_URLNotify(NPP instance, const char* URL, NPReason reason, void* notifyData);
NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value);
NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value);
}
extern NPNetscapeFuncs *myNPNFuncs;
extern FILE *logfile;
extern pthread_mutex_t rMutex;
extern pthread_attr_t rThreadAttrs;
class WebREngine : public NPObject
{
protected:
    // Class member functions that do the real work
    void Deallocate();
    void Invalidate();
    bool HasMethod(NPIdentifier name);
    bool Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
    bool InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result);
    bool HasProperty(NPIdentifier name);
    bool GetProperty(NPIdentifier name, NPVariant *result);
    bool SetProperty(NPIdentifier name, const NPVariant *value);
    bool RemoveProperty(NPIdentifier name);
    bool Enumerate(NPIdentifier **identifier, uint32_t *count);
    bool Construct(const NPVariant *args, uint32_t argCount, NPVariant *result);

public:
		WebREngine (NPP instance);
		void Detatch (void);

		// This is the method used to create the NPObject
    // This method should not be called explicitly
    // Instead, use NPN_CreateObject
    static NPObject* Allocate(NPP npp, NPClass *aClass);

    /////////////////////////////
    // Static NPObject methods //
    /////////////////////////////
    static void _Deallocate(NPObject *npobj);
    static void _Invalidate(NPObject *npobj);
    static bool _HasMethod(NPObject *npobj, NPIdentifier name);
    static bool _Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
    static bool _InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
    static bool _HasProperty(NPObject * npobj, NPIdentifier name);
    static bool _GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result);
    static bool _SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value);
    static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
    static bool _Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count);
    static bool _Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
 
    static NPClass _npclass;
 
  NPP instance; //need this accessable for R<->NP conversions
protected:
    NPP m_Instance;
	
private:

};


class RObject : public NPObject
{
protected:
    // Class member functions that do the real work
    void Deallocate();
    void Invalidate();
    bool HasMethod(NPIdentifier name);
    bool Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
    bool InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result);
    bool HasProperty(NPIdentifier name);
    bool GetProperty(NPIdentifier name, NPVariant *result);
    bool SetProperty(NPIdentifier name, const NPVariant *value);
    bool RemoveProperty(NPIdentifier name);
    bool Enumerate(NPIdentifier **identifier, uint32_t *count);
    bool Construct(const NPVariant *args, uint32_t argCount, NPVariant *result);

public:
  RObject (NPP instance);
    		void Detatch (void);

		// This is the method used to create the NPObject
    // This method should not be called explicitly
    // Instead, use NPN_CreateObject
    static NPObject* Allocate(NPP npp, NPClass *aClass);

    /////////////////////////////
    // Static NPObject methods //
    /////////////////////////////
    static void _Deallocate(NPObject *npobj);
    static void _Invalidate(NPObject *npobj);
    static bool _HasMethod(NPObject *npobj, NPIdentifier name);
    static bool _Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
    static bool _InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
    static bool _HasProperty(NPObject * npobj, NPIdentifier name);
    static bool _GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result);
    static bool _SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value);
    static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
    static bool _Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count);
    static bool _Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
 
  static NPClass _npclass;
    
  SEXP object;
  NPObject *converter;
  NPP instance;
  //Do we really need this? we seem to in order to be able to create references from the R side...
  NPNetscapeFuncs *funcs;
protected:
  NPP m_Instance;
	
    
};


class RFunction : public NPObject
{
protected:
    // Class member functions that do the real work
    void Deallocate();
    void Invalidate();
    bool HasMethod(NPIdentifier name);
    bool Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
    bool InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result);
    bool HasProperty(NPIdentifier name);
    bool GetProperty(NPIdentifier name, NPVariant *result);
    bool SetProperty(NPIdentifier name, const NPVariant *value);
    bool RemoveProperty(NPIdentifier name);
    bool Enumerate(NPIdentifier **identifier, uint32_t *count);
    bool Construct(const NPVariant *args, uint32_t argCount, NPVariant *result);

public:
  RFunction (NPP instance);
    		void Detatch (void);

		// This is the method used to create the NPObject
    // This method should not be called explicitly
    // Instead, use NPN_CreateObject
    static NPObject* Allocate(NPP npp, NPClass *aClass);

    /////////////////////////////
    // Static NPObject methods //
    /////////////////////////////
    static void _Deallocate(NPObject *npobj);
    static void _Invalidate(NPObject *npobj);
    static bool _HasMethod(NPObject *npobj, NPIdentifier name);
    static bool _Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
    static bool _InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
    static bool _HasProperty(NPObject * npobj, NPIdentifier name);
    static bool _GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result);
    static bool _SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value);
    static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
    static bool _Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count);
    static bool _Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
 
  static NPClass _npclass;
    
  SEXP object;
  NPObject *converter;
  NPP instance;
  //Do we really need this? we seem to in order to be able to create references from the R side...
  NPNetscapeFuncs *funcs;
protected:
  NPP m_Instance;
	

};




bool ConvertRToNP(SEXP val, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret, bool convertRes);
bool RVectorToNP(SEXP vec, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret);
bool ConvertNPToR(NPVariant *var, NPP inst, NPNetscapeFuncs *funcs, bool retRef, SEXP *_ret) ;
bool NPArrayToR(NPVariant *arr, int len, int simplify, NPP inst, NPNetscapeFuncs *funcs, SEXP *_ret);
SEXP makeNPVarRef(NPVariant *ref);
void CopyNPNFunctions(NPNetscapeFuncs *dstFuncs, NPNetscapeFuncs *srcFuncs);
SEXP MakeNPRefForR(NPVariant *obj);
void MakeRRefForNP(SEXP obj, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret);
bool RObject_GetProp( RObject *obj, NPIdentifier name, NPNetscapeFuncs *funcs, NPVariant *result, bool check);
bool IsMissing(SEXP obj, bool nullAlso);
bool CheckSEXPForJSRef(SEXP obj, NPP inst);
void makeRGlobals(NPP inst);



 
class RCallQueue
{ 
 public:
  SEXP requestRCall(SEXP toEval, SEXP env, int *err, NPP inst);
  SEXP requestRLookup(const char *name);
  void waitInQueue(uint64_t spot);
  void advanceQueue(uint64_t spot);
  uint64_t enterQueue(); 
  void init();
private:
  
  void lock();
  void unlock();

 
 private:
  int isLocked;
  uint64_t serving;
  uint64_t lastInQueue;
};

  extern RCallQueue rQueue;


SEXP innerGetVar(const char * varName, NPP inst);
SEXP doGetVar(NPIdentifier name, NPP inst);
 void* doRCall(void *in);
  void* doRLookup(void *in);

#endif // WebR.h
