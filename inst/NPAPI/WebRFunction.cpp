#include "WebR.h"


RFunction::RFunction (NPP instance) 
{
  

  //this->m_getVersion_id = NPN_GetStringIdentifier("getVersion");
  //  this->m_getVersion_id = myNPNFuncs->getstringidentifier("getVersion");
  fprintf(stderr, "\nCreating RFunction object. Instance:%lx", instance);fflush(stderr);
  this->instance = instance;
  this->object = NULL;
  this->converter = NULL;
  fprintf(stderr, "\nLeaving RFunction()\n");fflush(stderr);
}

void RFunction::Deallocate()
{
  if(this->object)
    {
      R_ReleaseObject(this->object);
      this->object=NULL;
    }
}

void RFunction::Invalidate()
{


}

bool RFunction::HasMethod(NPIdentifier name)
{
  int ret = 0;
  if(name == this->funcs->getstringidentifier("toString"))
    ret = 1;
  
  return (bool) ret;
}


bool RFunction::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  if (name == this->funcs->getstringidentifier("toString")) 
    {
      fprintf(stderr, "\nIn tostring method of an RFunction\n");fflush(stderr);
      //From NPN_ReleaseVariantValue docs: NPN_ReleaseVariantValue() will call NPN_ReleaseObject() on NPVariants of type NPVARIANTTYPE_OBJECT, and NPN_FreeMem() on NPVariants of type NPVARIANTTYPE_STRING. 
      NPUTF8 *strdat = (NPUTF8*) this->funcs->memalloc(21+1);
      //strdat = (NPUTF8*)"[Internal R Object]";
      memcpy(strdat, "[Internal R Function]", 21+1);
      NPString str ={ strdat, 21};
      result->type = NPVariantType_String;
      result->value.stringValue = str;      
      return true;
    }
    
  return false;
}

bool RFunction::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  fprintf(stderr, "\nDirectly invoking on RFunction object");fflush(stderr);
  SEXP Rargs[argCount];
  for(int i=0; i<argCount; i++)
    {
      PROTECT(Rargs[i] = R_NilValue); 
      //when calling R functions directly we DO want arguments to be converted.
      ConvertNPToR((NPVariant *) &(args[i]), this->instance, this->funcs, true, &Rargs[i]);
    }
  SEXP ans;
  SEXP call; 
  int error = 0;
  int addProt = 0;
  
  SEXP ptr;
  PROTECT(ptr = call = allocVector(LANGSXP, argCount  + 1));
  SETCAR(ptr, (SEXP) this->object );
  for(int i=0; i < argCount; i++)
    {
      ptr = CDR( ptr );
      SETCAR(ptr, Rargs[i]);
    }
  
  fprintf(stderr, "\nDirect API call: ");fflush(stderr);
  Rf_PrintValue(call);
  PROTECT(ans = R_tryEval( call, R_GlobalEnv, &error));
  
  addProt = 2;
  if(!error)
    ConvertRToNP(ans, this->instance, this->funcs, result, false);
  else
    ConvertRToNP(R_NilValue, this->instance, this->funcs, result, false);

  UNPROTECT(argCount + addProt);

  return false;
}

bool RFunction::HasProperty(NPIdentifier name)
{
  return false;
}

bool RFunction::GetProperty(NPIdentifier name, NPVariant *result)
{
   return false;
}

bool RFunction::SetProperty(NPIdentifier name, const NPVariant *value)
{
  //Emulate object[[name]]<-value
	return false;
}

bool RFunction::RemoveProperty(NPIdentifier name)
{
	return false;
}

bool RFunction::Enumerate(NPIdentifier **identifier, uint32_t *count)
{
    fprintf(stderr, "\nIn RFunction::Enumerate");fflush(stderr);
	return false;
}

bool RFunction::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return true;
}

NPObject *RFunction::Allocate(NPP npp, NPClass *aClass)
{
  fprintf(stderr, "\nIn RFunction::Allocate\n");fflush(stderr);
  
	NPObject *pObj = (NPObject *)new RFunction(npp);
	return pObj;
	fprintf(stderr, "\nLeaving RFunction::Allocate\n");fflush(stderr);
}

void RFunction::Detatch (void)
{
	m_Instance = NULL;
}



void RFunction::_Deallocate(NPObject *npobj)
{
	RFunction *pObj = ((RFunction *) npobj);

  // Call the virtual destructor.
	pObj->Deallocate ();
	delete pObj;
}

void RFunction::_Invalidate(NPObject *npobj)
{
	((RFunction*)npobj)->Invalidate();
}

bool RFunction::_HasMethod(NPObject *npobj, NPIdentifier name)
{
	return ((RFunction*)npobj)->HasMethod (name);
}

bool RFunction::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RFunction*)npobj)->Invoke (name, args, argCount, result);
}

bool RFunction::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RFunction*)npobj)->InvokeDefault (args, argCount, result);
}

bool RFunction::_HasProperty(NPObject * npobj, NPIdentifier name)
{
	return ((RFunction*)npobj)->HasProperty (name);
}

bool RFunction::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
	return ((RFunction*)npobj)->GetProperty (name, result);
}

bool RFunction::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
	return ((RFunction*)npobj)->SetProperty (name, value);
}

bool RFunction::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
	return ((RFunction*)npobj)->RemoveProperty (name);
}

bool RFunction::_Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
	return ((RFunction*)npobj)->Enumerate (identifier, count);
}

bool RFunction::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RFunction*)npobj)->Construct (args, argCount, result);
}


NPClass RFunction::_npclass = {                              
  NP_CLASS_STRUCT_VERSION,								                          
  RFunction::Allocate,                                       
  RFunction::_Deallocate,                                    
  RFunction::_Invalidate,                                    
  RFunction::_HasMethod,                                     
  RFunction::_Invoke,                                        
  RFunction::_InvokeDefault,                                 
  RFunction::_HasProperty,                                   
  RFunction::_GetProperty,                                   
  RFunction::_SetProperty,                                   
  RFunction::_RemoveProperty,                                
  RFunction::_Enumerate,                                     
  RFunction::_Construct                                      
};

 
bool RFunction_GetProp(RFunction *Robj, NPIdentifier name, NPNetscapeFuncs *funcs, NPVariant *result, bool check)
{
  SEXP obj, call, ptr, ans;
  //do we need to proect here?
  obj = Robj->object;
  int waserr = 0;
  bool toret = 0;
  PROTECT(ptr = call = allocVector(LANGSXP, 3));
  
  //try [[
  SETCAR(ptr, Rf_install("[["));
  ptr = CDR( ptr );
  SETCAR(ptr, obj);
  ptr = CDR( ptr );
  if(funcs->identifierisstring(name))
    SETCAR( ptr , Rf_install( (const char *) funcs->utf8fromidentifier(name)));
  else
    SETCAR( ptr , ScalarReal( (int) funcs->intfromidentifier(name)));
  
  PROTECT(ans = R_tryEval( call, R_GlobalEnv, &waserr));
  if(!waserr && !IsMissing(ans, true))
    {
      //non-missing, non-null result. stop looking
      toret = 1;
    } else {
  //try $
    ptr = call;
    SETCAR(ptr, Rf_install("$"));
    ans = R_tryEval( call, R_GlobalEnv, &waserr);
    if(!waserr && !IsMissing(ans, true))
      toret = 1;
    else
      {
	//try @
	ptr = call;
	SETCAR(ptr, Rf_install("@"));
	ans = R_tryEval( call, R_GlobalEnv, &waserr);
	if(!waserr && !IsMissing(ans, false))
	  toret = 1;
	else
	  {
	    ans = NEW_NUMERIC(1);
	    REAL(ans)[0] = NA_REAL;
	  }
      }
  }
  
  ConvertRToNP(ans, Robj->instance, funcs, result, false);
  if(!check)
    return true;
  else
    return toret;
}
