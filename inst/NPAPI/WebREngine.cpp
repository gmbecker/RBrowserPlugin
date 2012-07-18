//https://raw.github.com/jskorpan/BoilerplateNp/master/npcommon/ScriptableObject.cpp
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <sys/stat.h>

#include "WebR.h"
//#include "plugin.h"


using namespace std;


void C_doTest(NPP inst, NPNetscapeFuncs *funcs)
{

 NPObject *domwin = NULL;
  NPError res;
  res = funcs->getvalue(inst, NPNVWindowNPObject , &domwin);
  fprintf(stderr, "\nIn doTest. res: %d", res); fflush(stderr);
  NPVariant *vartmp = (NPVariant *) funcs->memalloc(sizeof(NPVariant));;
  NPVariant *ret = (NPVariant *) funcs->memalloc(sizeof(NPVariant));;
  char *buf = (char *) funcs->memalloc(4*sizeof(char));;
  sprintf(buf, "div5");
  STRINGZ_TO_NPVARIANT((const char *) buf, *vartmp);

  NPVariant *vartmp2 = (NPVariant *) funcs->memalloc(sizeof(NPVariant));;
  
  char *buf2 = (char *) funcs->memalloc(8*sizeof(char));;
  sprintf(buf2, "Success!");
  STRINGZ_TO_NPVARIANT((const char *) buf2, *vartmp2);
  
  bool set;
  res = funcs->invoke(inst, domwin, funcs->getstringidentifier("getElementById"), vartmp, 1, ret);
  fprintf(stderr, "\nIn doTest. res: %d", res); fflush(stderr);
  set = funcs->setproperty(inst, ret->value.objectValue, funcs->getstringidentifier("innerHTML"), vartmp2);
  fprintf(stderr, "\nIn doTest. set: %d", set); fflush(stderr);
  funcs->releaseobject(domwin);
  funcs->releasevariantvalue(vartmp);
  funcs->releasevariantvalue(vartmp2);
  funcs->releasevariantvalue(ret);
}


SEXP doGetVar(NPIdentifier name, NPP inst)
{
NPUTF8 *varName = (NPUTF8 *) myNPNFuncs->utf8fromidentifier(name);
 return innerGetVar((const char *) varName, inst);
}

SEXP innerGetVar(const char * varName, NPP inst)
{
  //NPUTF8 *varName = (NPUTF8 *) myNPNFuncs->utf8fromidentifier(name);  
  fprintf(stderr, "\nLooking for R object : %s", (const char *)varName);fflush(stderr);
  if(!varName || !varName[0]) {
    return R_UnboundValue;
  }
  /*
  SEXP ans, call, ptr;
  int err = 0;
  PROTECT(ptr = call = allocVector(LANGSXP, 2));
  SETCAR(ptr, Rf_install("get"));
  ptr = CDR(ptr);
  //SETCAR(ptr, mkString((const char *) varName));
    SETCAR(ptr, varName);
  PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &err, inst));
  if(err)
  ans = R_UnboundValue;*/
  //ans = Rf_findVar( Rf_install((const char *)varName), R_GlobalEnv);
  //UNPROTECT(1);
  SEXP ans;
  ans = rQueue.requestRLookup(varName);
  return ans;
}

int doVarLookup(NPIdentifier name, bool func, NPP inst)
{ 
  int ret;
  int err = 0;

  SEXP ans;
  PROTECT(ans = doGetVar(name, inst));

   if(ans == R_UnboundValue)
     {
       fprintf(stderr, "\nNo R object found.");fflush(stderr);
       ret = 0;
     } else {
     
	//XXX If it is a promise we need the actual value. Will this come back to bite us by violating lazy loading?
	if(TYPEOF(ans) == PROMSXP)
	  //	  ans = PRVALUE(ans);
	  ans = rQueue.requestRCall(ans, R_GlobalEnv, &err, inst);
	// Methods only correspond to functions! normal variables are properties
	if(TYPEOF(ans) == CLOSXP)
	  {
	    //ret = func;
	    fprintf(stderr, "\nR function found.");fflush(stderr);
	  }
	else
	  {
	    fprintf(stderr, "\nNon-function object found.");fflush(stderr);
	    //ret = (!func);
	  }
	ret = 1;
      }
   UNPROTECT(1);
   return ret;
 }

WebREngine::WebREngine (NPP instance) 
{
  

  //this->m_getVersion_id = NPN_GetStringIdentifier("getVersion");
  //  this->m_getVersion_id = myNPNFuncs->getstringidentifier("getVersion");
  fprintf(stderr, "\nCreating WebREngine object. Instance:%lx", (unsigned long int) instance);fflush(stderr);
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
  int ret = 0;
  if(name == myNPNFuncs->getstringidentifier("eval"))
    ret = 1;
  else if (name == myNPNFuncs->getstringidentifier("listCall"))
    ret = 1;
  else if (name == myNPNFuncs->getstringidentifier("C_doTest"))
    ret = 1;
  else if (name == myNPNFuncs->getstringidentifier("getRef"))
    ret = 1;
  else
   {
      //direct access API
     //We are doing everything with properties instead.   
     //ret = doVarLookup(name, true);
   }
  return (bool) ret;
}

bool WebREngine::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  fprintf(stderr, "\nIn WebREngine::Invoke - %s", myNPNFuncs->utf8fromidentifier(name));fflush(stderr);
  SEXP Rargs[argCount];
  convert_t convert = CONV_DEFAULT;
  /*
  if(argCount && args[0].type == NPVariantType_Object && myNPNFuncs->hasproperty(this->instance, args[0].value.objectValue, myNPNFuncs->getstringidentifier("namedArrayForR")))
    {
      return doNamedCall(this->instance, name, args, argCount, result);
    }
  */

    for(uint32_t i=0; i<argCount; i++)
      {
	PROTECT(Rargs[i] = R_NilValue); 
	//when calling R functions directly we DO want arguments to be converted.
	ConvertNPToR((NPVariant *) &(args[i]), this->instance, myNPNFuncs, CONV_DEFAULT, &Rargs[i]);
      }
    SEXP ans;
    SEXP call, ptr; 
    int error = 0;
    int addProt = 0;
    if(name == myNPNFuncs->getstringidentifier("eval"))
      {
	
	PROTECT(call = allocVector(LANGSXP, 2));
	SETCAR(call, Rf_install("parseEval"));
	SETCAR(CDR(call), Rargs[0]);
	PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));
	addProt = 2;
      }
    else if (name == myNPNFuncs->getstringidentifier("listCall"))
      {
	PROTECT(ptr = call = allocVector(LANGSXP, 3));
	SETCAR(call, Rf_install("do.call"));
      ptr = CDR(call);
      SETCAR(ptr, Rargs[0]); //The function
      ptr = CDR(ptr);
      SETCAR(ptr, Rargs[1]); //the args list
      PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));
      addProt = 2;	
    }
  else if (name == myNPNFuncs->getstringidentifier("C_doTest"))
    C_doTest(this->instance, myNPNFuncs);
  else if (name == myNPNFuncs->getstringidentifier("getRef"))
    {/*
      SEXP  call, ptr;
      int error;  
      PROTECT(ptr = call = allocVector(LANGSXP, 2));
      SETCAR(ptr, Rf_install("get"));
      ptr = CDR(ptr);
      SETCAR(ptr, Rargs[0]);
      PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));
      //error =  ConvertRToNP(val, this->instance, myNPNFuncs, result, false);
      addProt = 2;
     */
      PROTECT(ans = innerGetVar(CHAR(STRING_ELT(Rargs[0], 0)),this->instance));
      convert = CONV_REF;
      addProt = 1;
    }    
  else
    {
      
      SEXP ptr;
      //NPString strname = myNPNFuncs->utf8fromidentifier(name);
      //const char *ccharname =  NPStringToConstChar(strname);
      char *charname = (char* )myNPNFuncs->utf8fromidentifier(name);
      PROTECT(ptr = call = allocVector(LANGSXP, argCount  + 1));
      SETCAR(ptr, Rf_install( (const char *) charname) );
      myNPNFuncs->memfree(charname);
      for(uint32_t i=0; i < argCount; i++)
	{
	  ptr = CDR( ptr );
	  SETCAR(ptr, Rargs[i]);
	}

      fprintf(stderr, "\nDirect API call: ");fflush(stderr);
      Rf_PrintValue(call);
      PROTECT(ans = rQueue.requestRCall( call, R_GlobalEnv, &error, this->instance));
      
      addProt = 2;
      

    }
   /*
  else if (name == myNPNFuncs->getstringidentifier("exists"))
    ret = 1;
  else if (name == myNPNFuncs->getstringidentifier("get"))
    ret = 1;
  else if (name == myNPNFuncs->getstringidentifier("set"))
    ret = 1;
  */
  if(!error)
    ConvertRToNP(ans, this->instance, myNPNFuncs, result, convert);
  else
    {
      fprintf(stderr, "\n Error: R function call failed.");fflush(stderr);
    ConvertRToNP(R_NilValue, this->instance, myNPNFuncs, result, CONV_DEFAULT);
    }
  UNPROTECT(argCount + addProt);
  return !error;
}

bool WebREngine::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return false;
}

bool WebREngine::HasProperty(NPIdentifier name)
{
  fprintf(stderr, "\nIn WebREngine::HasProperty");fflush(stderr);
  bool ret;
  if(name == myNPNFuncs->getstringidentifier("eval"))
    ret = 0;
  else if(name == myNPNFuncs->getstringidentifier("listcall"))
    ret = 0;
  else if(name == myNPNFuncs->getstringidentifier("getRef"))
    ret = 0;
  else if(name == myNPNFuncs->getstringidentifier("serving"))
    ret = 1;
  else
    ret = doVarLookup(name, false, this->instance);
  return ret;
}

bool WebREngine::GetProperty(NPIdentifier name, NPVariant *result)
{
  bool ret;  
  if(name == myNPNFuncs->getstringidentifier("serving"))
    {
      INT32_TO_NPVARIANT(rQueue.serving, *result);
      ret = 1;
    }
  else
    {

      fprintf(stderr, "\nIn WebREngine::GetProperty");fflush(stderr);
      SEXP val;
      PROTECT(val = doGetVar(name, this->instance));
      
     
      
      if(TYPEOF(val) == CLOSXP)
	ret =  ConvertRToNP(val, this->instance, myNPNFuncs, result, CONV_DEFAULT);
      else
	
	ret =  ConvertRToNP(val, this->instance, myNPNFuncs, result, CONV_DEFAULT) ;
      UNPROTECT(1);
    }
  return ret;
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

