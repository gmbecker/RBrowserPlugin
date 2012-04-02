//https://raw.github.com/jskorpan/BoilerplateNp/master/npcommon/ScriptableObject.cpp
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <sys/stat.h>

#include "BasicPlugin.h"
//#include "plugin.h"


using namespace std;

WebREngine::WebREngine (NPP instance) 
{
  

  //this->m_getVersion_id = NPN_GetStringIdentifier("getVersion");
  //  this->m_getVersion_id = myNPNFuncs->getstringidentifier("getVersion");
  fprintf(stderr, "\nCreating WebREngine object. Instance:%lx", instance);fflush(stderr);
  this->instance = instance;
  fprintf(stderr, "\nLeaving WebREngine()\n");fflush(stderr);
}



void WebREngine::Deallocate()
{
}

void WebREngine::Invalidate()
{


}

bool WebREngine::HasMethod(NPIdentifier name)
{
  fprintf(stderr, "\nIn WebREngine::HasMethod");fflush(stderr);
  int ret = 0;
  fprintf(stderr, "myNPNFuncs: %lx", myNPNFuncs);fflush(stderr);
  fprintf(stderr, "myNPNFuncs->getstringidentifier: %lx", myNPNFuncs->getstringidentifier);fflush(stderr);
  if(name == myNPNFuncs->getstringidentifier("eval"))
    ret = 1;
  else if (name == myNPNFuncs->getstringidentifier("listCall"))
    ret = 1;
  else if (name == myNPNFuncs->getstringidentifier("exists"))
    ret = 1;
  else if (name == myNPNFuncs->getstringidentifier("get"))
    ret = 1;
  else if (name == myNPNFuncs->getstringidentifier("set"))
    ret = 1;
  fprintf(stderr, "\nLeaving WebREngine::HasMethod");fflush(stderr);
  return (bool) ret;
}


bool WebREngine::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  
  fprintf(stderr, "\nIn WebREngine::Invoke");fflush(stderr);
  

  SEXP Rargs[argCount];
  for(int i=0; i<argCount; i++)
    PROTECT(Rargs[i] = ConvertNPToR(args[i], this->instance));
  SEXP ans;
  int error = 0;
  int ret = 0;
  if(name == myNPNFuncs->getstringidentifier("eval"))
    {
      SEXP call;
      PROTECT(call = allocVector(LANGSXP, 2));
      SETCAR(call, Rf_install("parseEval"));
      SETCAR(CDR(call), Rargs[0]);
      PROTECT(ans = R_tryEval(call, R_GlobalEnv, &error));
      if(!error)
	ConvertRToNP(ans, this->instance,result);
      else
	ConvertRToNP(R_NilValue, this->instance, result);
      UNPROTECT(2);
    }
  else if (name == myNPNFuncs->getstringidentifier("listCall"))
    ret = 1;
  else if (name == myNPNFuncs->getstringidentifier("exists"))
    ret = 1;
  else if (name == myNPNFuncs->getstringidentifier("get"))
    ret = 1;
  else if (name == myNPNFuncs->getstringidentifier("set"))
    ret = 1;

  UNPROTECT(argCount);
  return false;
}

bool WebREngine::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return false;
}

bool WebREngine::HasProperty(NPIdentifier name)
{
  fprintf(stderr, "\nIn WebREngine::HasProperty");fflush(stderr);
	return false;
}

bool WebREngine::GetProperty(NPIdentifier name, NPVariant *result)
{
  fprintf(stderr, "\nIn WebREngine::GetProperty");fflush(stderr);
	return false;
}

bool WebREngine::SetProperty(NPIdentifier name, const NPVariant *value)
{
	return false;
}

bool WebREngine::RemoveProperty(NPIdentifier name)
{
	return false;
}

bool WebREngine::Enumerate(NPIdentifier **identifier, uint32_t *count)
{
    fprintf(stderr, "\nIn WebREngine::Enumerate");fflush(stderr);
	return false;
}

bool WebREngine::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return true;
}

NPObject *WebREngine::Allocate(NPP npp, NPClass *aClass)
{
  fprintf(stderr, "\nIn WebREngine::Allocate\n");fflush(stderr);
  
	NPObject *pObj = (NPObject *)new WebREngine(npp);
	return pObj;
	fprintf(stderr, "\nLeaving WebREngine::Allocate\n");fflush(stderr);
}

void WebREngine::Detatch (void)
{
	m_Instance = NULL;
}



void WebREngine::_Deallocate(NPObject *npobj)
{
	WebREngine *pObj = ((WebREngine *) npobj);

  // Call the virtual destructor.
	pObj->Deallocate ();
	delete pObj;
}

void WebREngine::_Invalidate(NPObject *npobj)
{
	((WebREngine*)npobj)->Invalidate();
}

bool WebREngine::_HasMethod(NPObject *npobj, NPIdentifier name)
{
	return ((WebREngine*)npobj)->HasMethod (name);
}

bool WebREngine::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((WebREngine*)npobj)->Invoke (name, args, argCount, result);
}

bool WebREngine::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((WebREngine*)npobj)->InvokeDefault (args, argCount, result);
}

bool WebREngine::_HasProperty(NPObject * npobj, NPIdentifier name)
{
	return ((WebREngine*)npobj)->HasProperty (name);
}

bool WebREngine::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
	return ((WebREngine*)npobj)->GetProperty (name, result);
}

bool WebREngine::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
	return ((WebREngine*)npobj)->SetProperty (name, value);
}

bool WebREngine::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
	return ((WebREngine*)npobj)->RemoveProperty (name);
}

bool WebREngine::_Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
	return ((WebREngine*)npobj)->Enumerate (identifier, count);
}

bool WebREngine::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((WebREngine*)npobj)->Construct (args, argCount, result);
}

 
NPClass WebREngine::_npclass = {                              
  NP_CLASS_STRUCT_VERSION,								                          
  WebREngine::Allocate,                                       
  WebREngine::_Deallocate,                                    
  WebREngine::_Invalidate,                                    
  WebREngine::_HasMethod,                                     
  WebREngine::_Invoke,                                        
  WebREngine::_InvokeDefault,                                 
  WebREngine::_HasProperty,                                   
  WebREngine::_GetProperty,                                   
  WebREngine::_SetProperty,                                   
  WebREngine::_RemoveProperty,                                
  WebREngine::_Enumerate,                                     
  WebREngine::_Construct                                      
};

