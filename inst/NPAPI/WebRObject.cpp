#include "WebR.h"


RObject::RObject (NPP instance) 
{
  this->instance = instance;
  this->object = NULL;
  this->converter = NULL;
}

void RObject::Deallocate()
{
  if(this->object)
    {
      R_ReleaseObject(this->object);
      this->object=NULL;
    }
}

void RObject::Invalidate()
{
}

bool RObject::HasMethod(NPIdentifier name)
{
  int ret = 0;
  if(name == this->funcs->getstringidentifier("convert"))
    ret = 1;
  else if(name == this->funcs->getstringidentifier("toString"))
    ret = 1;
  
  return (bool) ret;
}


bool RObject::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  if(name == this->funcs->getstringidentifier("convert"))
    {
      if(this->converter != NULL)
	{
	  fprintf(stderr, "\nUser assigned JavaScript converters are not supported at this time. If you need this functionality please contact the maintainer.\n");fflush(stderr);
	  return false;
	}
      //true indicates we want conversion from RObject to normalJavaScript object where possible.
      ConvertRToNP(this->object, this->instance, this->funcs, result, CONV_DEFAULT); 
      return true;
    } else if (name == this->funcs->getstringidentifier("toString")) 
    {
      fprintf(stderr, "\nIn tostring method of an RObject\n");fflush(stderr);
      //From NPN_ReleaseVariantValue docs: NPN_ReleaseVariantValue() will call NPN_ReleaseObject() on NPVariants of type NPVARIANTTYPE_OBJECT, and NPN_FreeMem() on NPVariants of type NPVARIANTTYPE_STRING. 
      NPUTF8 *strdat = (NPUTF8*) this->funcs->memalloc(19+1);
      //strdat = (NPUTF8*)"[Internal R Object]";
      memcpy(strdat, "[Internal R Object]", 19+1);
      NPString str ={ strdat, 19};
      result->type = NPVariantType_String;
      result->value.stringValue = str;     
      return true;
    }
    
  return false;
}

bool RObject::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  return false;
}

bool RObject::HasProperty(NPIdentifier name)
{
  bool ret= true;
  fprintf(stderr, "\nIn RObject::HasProperty");fflush(stderr);
  //we need to return false for things that need to be methods.
  if(name == this->funcs->getstringidentifier("convert"))
    ret = 0;
  else if(name == this->funcs->getstringidentifier("toString"))
    ret = 0;
  else if(name == this->funcs->getstringidentifier("valueOf"))
    ret = 0;
  else if(name == this->funcs->getstringidentifier("isRObject"))
    ret = 1;
  else
    {
      SEXP call, ans, ptr;
      int error=0;
      PROTECT(ptr = call = allocVector(LANGSXP, 3));
      SETCAR(ptr, Rf_install("CheckForProperty"));
      ptr = CDR(ptr);
      SETCAR(ptr, this-> object);
      ptr = CDR(ptr);
      if(this->funcs->identifierisstring(name))
	SETCAR(ptr, ScalarString(mkChar(this->funcs->utf8fromidentifier(name))));
      else
	SETCAR(ptr, ScalarInteger(this->funcs->intfromidentifier(name)));
      PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));
      UNPROTECT(2);
      //Need to check if the R property exists or not.... currently we just return true.
      if(!error)
	ret = LOGICAL(ans)[0];
      else
	ret = false;
    }
  return ret;
}

bool RObject::GetProperty(NPIdentifier name, NPVariant *result)
{
  if(name == this->funcs->getstringidentifier("isRObject"))
    {
      BOOLEAN_TO_NPVARIANT(true, *result); 
      return true;
    }
  //Emulate object[[name]], object$name, object@name in that order
  fprintf(stderr, "\nIn RObject::GetProperty");fflush(stderr);
  SEXP call, ptr, ans;
  int error;
  PROTECT(ptr = call = allocVector(LANGSXP, 3));
  SETCAR(ptr, Rf_install("[["));
  ptr = CDR(ptr);
  SETCAR(ptr, this->object);
  ptr = CDR(ptr);
  if(this->funcs->identifierisstring(name))
    SETCAR(ptr, ScalarString(mkChar(this->funcs->utf8fromidentifier(name))));
  else
    SETCAR(ptr, ScalarInteger(this->funcs->intfromidentifier(name)));
  
  PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));
  bool ret = ConvertRToNP(ans, this->instance, this->funcs, result, CONV_DEFAULT);
  UNPROTECT(2);
  //ConvertRToNP(ans, Robj->instance, funcs, result, false);
  return ret;
}

bool RObject::SetProperty(NPIdentifier name, const NPVariant *value)
{
  //Emulate object[[name]]<-value
	return false;
}

bool RObject::RemoveProperty(NPIdentifier name)
{
	return false;
}

bool RObject::Enumerate(NPIdentifier **identifier, uint32_t *count)
{
    fprintf(stderr, "\nIn RObject::Enumerate");fflush(stderr);
	return false;
}

bool RObject::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return true;
}

NPObject *RObject::Allocate(NPP npp, NPClass *aClass)
{
	NPObject *pObj = (NPObject *)new RObject(npp);
	return pObj;
}

void RObject::Detatch (void)
{
	m_Instance = NULL;
}



void RObject::_Deallocate(NPObject *npobj)
{
	RObject *pObj = ((RObject *) npobj);

  // Call the virtual destructor.
	pObj->Deallocate ();
	delete pObj;
}

void RObject::_Invalidate(NPObject *npobj)
{
	((RObject*)npobj)->Invalidate();
}

bool RObject::_HasMethod(NPObject *npobj, NPIdentifier name)
{
	return ((RObject*)npobj)->HasMethod (name);
}

bool RObject::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RObject*)npobj)->Invoke (name, args, argCount, result);
}

bool RObject::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RObject*)npobj)->InvokeDefault (args, argCount, result);
}

bool RObject::_HasProperty(NPObject * npobj, NPIdentifier name)
{
	return ((RObject*)npobj)->HasProperty (name);
}

bool RObject::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
	return ((RObject*)npobj)->GetProperty (name, result);
}

bool RObject::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
	return ((RObject*)npobj)->SetProperty (name, value);
}

bool RObject::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
	return ((RObject*)npobj)->RemoveProperty (name);
}

bool RObject::_Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
	return ((RObject*)npobj)->Enumerate (identifier, count);
}

bool RObject::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RObject*)npobj)->Construct (args, argCount, result);
}


NPClass RObject::_npclass = {                              
  NP_CLASS_STRUCT_VERSION,								                          
  RObject::Allocate,                                       
  RObject::_Deallocate,                                    
  RObject::_Invalidate,                                    
  RObject::_HasMethod,                                     
  RObject::_Invoke,                                        
  RObject::_InvokeDefault,                                 
  RObject::_HasProperty,                                   
  RObject::_GetProperty,                                   
  RObject::_SetProperty,                                   
  RObject::_RemoveProperty,                                
  RObject::_Enumerate,                                     
  RObject::_Construct                                      
};


//special type of object for S4

RS4Object::RS4Object (NPP instance) 
{
  this->instance = instance;
  this->object = NULL;
  this->converter = NULL;
}

void RS4Object::Deallocate()
{
  if(this->object)
    {
      R_ReleaseObject(this->object);
      this->object=NULL;
    }
}

void RS4Object::Invalidate()
{
}

bool RS4Object::HasMethod(NPIdentifier name)
{
  int ret = 0;
  if(name == this->funcs->getstringidentifier("convert"))
    ret = 1;
  else if(name == this->funcs->getstringidentifier("toString"))
    ret = 1;
  
  return (bool) ret;
}


bool RS4Object::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  if(name == this->funcs->getstringidentifier("convert"))
    {
      if(this->converter != NULL)
	{
	  fprintf(stderr, "\nUser assigned JavaScript converters are not supported at this time. If you need this functionality please contact the maintainer.\n");fflush(stderr);
	  return false;
	}
      //true indicates we want conversion from RS4Object to normalJavaScript object where possible.
      ConvertRToNP(this->object, this->instance, this->funcs, result, CONV_DEFAULT); 
      return true;
    } else if (name == this->funcs->getstringidentifier("toString")) 
    {
      fprintf(stderr, "\nIn tostring method of an RS4Object\n");fflush(stderr);
      //From NPN_ReleaseVariantValue docs: NPN_ReleaseVariantValue() will call NPN_ReleaseObject() on NPVariants of type NPVARIANTTYPE_OBJECT, and NPN_FreeMem() on NPVariants of type NPVARIANTTYPE_STRING. 
      NPUTF8 *strdat = (NPUTF8*) this->funcs->memalloc(22+1);
      //strdat = (NPUTF8*)"[Internal R Object]";
      memcpy(strdat, "[Internal R S4 Object]", 22+1);
      NPString str ={ strdat, 22};
      result->type = NPVariantType_String;
      result->value.stringValue = str;     
      return true;
    }
    
  return false;
}

bool RS4Object::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  return false;
}

bool RS4Object::HasProperty(NPIdentifier name)
{
  bool ret= true;
  fprintf(stderr, "\nIn RS4Object::HasProperty");fflush(stderr);
  //we need to return false for things that need to be methods.
  if(name == this->funcs->getstringidentifier("convert"))
    ret = 0;
  else if(name == this->funcs->getstringidentifier("toString"))
    ret = 0;
  else if(name == this->funcs->getstringidentifier("valueOf"))
    ret = 0;
  else if(name == this->funcs->getstringidentifier("isRObject"))
    ret = 1;
  else
    {
      ret = (bool) R_has_slot(this->object, ScalarString(mkChar(this->funcs->utf8fromidentifier(name))));
      /*
      PROTECT(ptr = call = allocVector(LANGSXP, 3));
      SETCAR(ptr, Rf_install("CheckForProperty"));
      ptr = CDR(ptr);
      SETCAR(ptr, this-> object);
      ptr = CDR(ptr);
      if(this->funcs->identifierisstring(name))
	SETCAR(ptr, ScalarString(mkChar(this->funcs->utf8fromidentifier(name))));
      else
	SETCAR(ptr, ScalarInteger(this->funcs->intfromidentifier(name)));
      PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));
      UNPROTECT(2);
      //Need to check if the R property exists or not.... currently we just return true.
      if(!error)
	ret = LOGICAL(ans)[0];
      else
	ret = false;
      */
    }
  return ret;
}

bool RS4Object::GetProperty(NPIdentifier name, NPVariant *result)
{
  if(name == this->funcs->getstringidentifier("isRObject"))
    {
      BOOLEAN_TO_NPVARIANT(true, *result); 
      return true;
    }
  //Emulate object[[name]], object$name, object@name in that order
  fprintf(stderr, "\nIn RS4Object::GetProperty");fflush(stderr);
  SEXP call, ptr, ans;
  int error;
  PROTECT(ptr = call = allocVector(LANGSXP, 3));
  SETCAR(ptr, Rf_install("@"));
  ptr = CDR(ptr);
  SETCAR(ptr, this->object);
  ptr = CDR(ptr);

  SETCAR(ptr, ScalarString(mkChar(this->funcs->utf8fromidentifier(name))));
  
  PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));
  bool ret = ConvertRToNP(ans, this->instance, this->funcs, result, CONV_DEFAULT);
  UNPROTECT(2);
  //ConvertRToNP(ans, Robj->instance, funcs, result, false);
  return ret;
}

bool RS4Object::SetProperty(NPIdentifier name, const NPVariant *value)
{
  //Emulate object@name <- value
  bool ret = this->HasProperty(name);
  if(ret)
    {
      SEXP tmp, ptr, call, tmp2;
      int err = 0;
      PROTECT(call = ptr = allocVector(LANGSXP, 4));
      //SETCAR(ptr, Rf_install("@<-"));
      SETCAR(ptr, Rf_install("checkSlotAssignment"));
      
      ptr = CDR(ptr);
      SETCAR(ptr, this->object);
      ptr = CDR(ptr);
      SETCAR(ptr, ScalarString(mkChar((funcs->utf8fromidentifier(name)))));
      ptr = CDR(ptr);
      
      PROTECT(tmp = R_NilValue);
      ConvertNPToR((NPVariant *)value, this->instance, this->funcs, CONV_DEFAULT, &tmp);
      SETCAR(ptr, tmp);
      //this->object = R_tryEval(call, R_GlobalEnv, &err);
      tmp2 = R_tryEval(call, R_GlobalEnv, &err);
      if(err)
	return false;
      
      SETCAR(call, Rf_install("@<-"));
      this->object = R_tryEval(call, R_GlobalEnv, &err);
      ret = !(bool)err;
      /*
      //XXX We should probably use tryEval so we can catch errors without it potentially crashing?
      SET_SLOT(this->object, ScalarString(mkChar(funcs->utf8fromidentifier(name))), tmp);
      */
      UNPROTECT(1);
    }
  return ret;
}

bool RS4Object::RemoveProperty(NPIdentifier name)
{
	return false;
}

bool RS4Object::Enumerate(NPIdentifier **identifier, uint32_t *count)
{
    fprintf(stderr, "\nIn RS4Object::Enumerate");fflush(stderr);
	return false;
}

bool RS4Object::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return true;
}

NPObject *RS4Object::Allocate(NPP npp, NPClass *aClass)
{
	NPObject *pObj = (NPObject *)new RS4Object(npp);
	return pObj;
}

void RS4Object::Detatch (void)
{
	m_Instance = NULL;
}



void RS4Object::_Deallocate(NPObject *npobj)
{
	RS4Object *pObj = ((RS4Object *) npobj);

  // Call the virtual destructor.
	pObj->Deallocate ();
	delete pObj;
}

void RS4Object::_Invalidate(NPObject *npobj)
{
	((RS4Object*)npobj)->Invalidate();
}

bool RS4Object::_HasMethod(NPObject *npobj, NPIdentifier name)
{
	return ((RS4Object*)npobj)->HasMethod (name);
}

bool RS4Object::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RS4Object*)npobj)->Invoke (name, args, argCount, result);
}

bool RS4Object::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RS4Object*)npobj)->InvokeDefault (args, argCount, result);
}

bool RS4Object::_HasProperty(NPObject * npobj, NPIdentifier name)
{
	return ((RS4Object*)npobj)->HasProperty (name);
}

bool RS4Object::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
	return ((RS4Object*)npobj)->GetProperty (name, result);
}

bool RS4Object::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
	return ((RS4Object*)npobj)->SetProperty (name, value);
}

bool RS4Object::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
	return ((RS4Object*)npobj)->RemoveProperty (name);
}

bool RS4Object::_Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
	return ((RS4Object*)npobj)->Enumerate (identifier, count);
}

bool RS4Object::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RS4Object*)npobj)->Construct (args, argCount, result);
}


NPClass RS4Object::_npclass = {                              
  NP_CLASS_STRUCT_VERSION,								                          
  RS4Object::Allocate,                                       
  RS4Object::_Deallocate,                                    
  RS4Object::_Invalidate,                                    
  RS4Object::_HasMethod,                                     
  RS4Object::_Invoke,                                        
  RS4Object::_InvokeDefault,                                 
  RS4Object::_HasProperty,                                   
  RS4Object::_GetProperty,                                   
  RS4Object::_SetProperty,                                   
  RS4Object::_RemoveProperty,                                
  RS4Object::_Enumerate,                                     
  RS4Object::_Construct                                      
};

RRefClassObject::RRefClassObject (NPP instance) 
{
  this->instance = instance;
  this->object = NULL;
  this->converter = NULL;
}

void RRefClassObject::Deallocate()
{
  if(this->object)
    {
      R_ReleaseObject(this->object);
      this->object=NULL;
    }
}

void RRefClassObject::Invalidate()
{
}

bool RRefClassObject::HasMethod(NPIdentifier name)
{
  int ret = 0;
  if(name == this->funcs->getstringidentifier("convert"))
    ret = 1;
  else if(name == this->funcs->getstringidentifier("toString"))
    ret = 1;
  /*
  else
    {
      SEXP call, ans, ptr;
      int error=0;
      PROTECT(ptr = call = allocVector(LANGSXP, 3));
      SETCAR(ptr, Rf_install("checkForMethod"));
      ptr = CDR(ptr);
      SETCAR(ptr, this-> object);
      ptr = CDR(ptr);
      if(this->funcs->identifierisstring(name))
	SETCAR(ptr, ScalarString(mkChar(this->funcs->utf8fromidentifier(name))));
      else
	SETCAR(ptr, ScalarInteger(this->funcs->intfromidentifier(name)));
      PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));
      UNPROTECT(2);
      //Need to check if the R property exists or not.... currently we just return true.
      if(!error)
	ret = LOGICAL(ans)[0];
      else
	ret = false;
      

    }
  */
  return (bool) ret;
}


bool RRefClassObject::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  if(name == this->funcs->getstringidentifier("convert"))
    {
      if(this->converter != NULL)
	{
	  fprintf(stderr, "\nUser assigned JavaScript converters are not supported at this time. If you need this functionality please contact the maintainer.\n");fflush(stderr);
	  return false;
	}
      //true indicates we want conversion from RRefClassObject to normalJavaScript object where possible.
      ConvertRToNP(this->object, this->instance, this->funcs, result, CONV_DEFAULT); 
      return true;
    } else if (name == this->funcs->getstringidentifier("toString")) 
    {
      fprintf(stderr, "\nIn tostring method of an RRefClassObject\n");fflush(stderr);
      //From NPN_ReleaseVariantValue docs: NPN_ReleaseVariantValue() will call NPN_ReleaseObject() on NPVariants of type NPVARIANTTYPE_OBJECT, and NPN_FreeMem() on NPVariants of type NPVARIANTTYPE_STRING. 
      NPUTF8 *strdat = (NPUTF8*) this->funcs->memalloc(34+1);
      //strdat = (NPUTF8*)"[Internal R Object]";
      memcpy(strdat, "[Internal R ReferenceClass Object]", 34+1);
      NPString str ={ strdat, 34};
      result->type = NPVariantType_String;
      result->value.stringValue = str;     
      return true;
    }
    
  return false;
}

bool RRefClassObject::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  return false;
}

bool RRefClassObject::HasProperty(NPIdentifier name)
{
  bool ret= true;
  fprintf(stderr, "\nIn RRefClassObject::HasProperty");fflush(stderr);
  //we need to return false for things that need to be methods.
  if(name == this->funcs->getstringidentifier("convert"))
    ret = 0;
  else if(name == this->funcs->getstringidentifier("toString"))
    ret = 0;
  else if(name == this->funcs->getstringidentifier("valueOf"))
    ret = 0;
  else if(name == this->funcs->getstringidentifier("isRObject"))
    ret = 1;
  else
    {
      SEXP call, ans, ptr;
      int error=0;
      PROTECT(call = ptr = allocVector(LANGSXP, 3));
      SETCAR(ptr, Rf_install("checkForFieldorMethod"));
      ptr = CDR(ptr);
      SETCAR(ptr, this-> object);
      ptr = CDR(ptr);
      if(this->funcs->identifierisstring(name))
	SETCAR(ptr, ScalarString(mkChar(this->funcs->utf8fromidentifier(name))));
      else
	SETCAR(ptr, ScalarInteger(this->funcs->intfromidentifier(name)));
      PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));

      //Need to check if the R property exists or not.... currently we just return true.
      if(!error)
	ret = LOGICAL(ans)[0];
      else
	ret = false;
      UNPROTECT(2);
    }
  return ret;
}

bool RRefClassObject::GetProperty(NPIdentifier name, NPVariant *result)
{
  if(name == this->funcs->getstringidentifier("isRObject"))
    {
      BOOLEAN_TO_NPVARIANT(true, *result); 
      return true;
    }
  //Emulate object[[name]], object$name, object@name in that order
  fprintf(stderr, "\nIn RRefClassObject::GetProperty");fflush(stderr);
  SEXP call, ptr, ans;
  int error;
  bool isString = this->funcs->identifierisstring(name);
  PROTECT(ptr = call = allocVector(LANGSXP, 3));
  if(isString)
    SETCAR(ptr, Rf_install("$"));
  else
    SETCAR(ptr, Rf_install("[["));
  ptr = CDR(ptr);
  SETCAR(ptr, this->object);
  ptr = CDR(ptr);
  if(isString)
    SETCAR(ptr, ScalarString(mkChar(this->funcs->utf8fromidentifier(name))));
  else
    SETCAR(ptr, ScalarInteger(this->funcs->intfromidentifier(name)));
  
  PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));
  bool ret = ConvertRToNP(ans, this->instance, this->funcs, result, CONV_DEFAULT);
  UNPROTECT(2);
  //ConvertRToNP(ans, Robj->instance, funcs, result, false);
  return ret;
}

bool RRefClassObject::SetProperty(NPIdentifier name, const NPVariant *value)
{
  bool ret = false;
  if(this->HasProperty(name))
    {
      SEXP  call, ptr, val;
      int err = 0;
      PROTECT(call = ptr = allocVector(LANGSXP, 4));
      PROTECT(val = R_NilValue);
      bool isString = this->funcs->identifierisstring(name);
      if(isString)
	SETCAR(ptr, Rf_install("$<-"));
      else
	SETCAR(ptr, Rf_install("[[<-"));
  
 
      ptr = CDR(ptr);
      SETCAR(ptr, this->object);
      ptr = CDR(ptr);
      if(isString)
	SETCAR(ptr, ScalarString(mkChar(this->funcs->utf8fromidentifier(name))));
      else
	SETCAR(ptr, ScalarInteger(this->funcs->intfromidentifier(name)));
      
      bool success = ConvertNPToR((NPVariant *)value, this->instance, this->funcs, CONV_DEFAULT, &val);
      ptr = CDR(ptr);
      SETCAR(ptr, val);
      this->object = R_tryEval(call, R_GlobalEnv, &err);
      ret = !(bool)err;
      UNPROTECT(2);

    }
  //Emulate object[[name]]<-value
  return ret;
}

bool RRefClassObject::RemoveProperty(NPIdentifier name)
{
	return false;
}

bool RRefClassObject::Enumerate(NPIdentifier **identifier, uint32_t *count)
{
    fprintf(stderr, "\nIn RRefClassObject::Enumerate");fflush(stderr);
	return false;
}

bool RRefClassObject::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return true;
}

NPObject *RRefClassObject::Allocate(NPP npp, NPClass *aClass)
{
	NPObject *pObj = (NPObject *)new RRefClassObject(npp);
	return pObj;
}

void RRefClassObject::Detatch (void)
{
	m_Instance = NULL;
}



void RRefClassObject::_Deallocate(NPObject *npobj)
{
	RRefClassObject *pObj = ((RRefClassObject *) npobj);

  // Call the virtual destructor.
	pObj->Deallocate ();
	delete pObj;
}

void RRefClassObject::_Invalidate(NPObject *npobj)
{
	((RRefClassObject*)npobj)->Invalidate();
}

bool RRefClassObject::_HasMethod(NPObject *npobj, NPIdentifier name)
{
	return ((RRefClassObject*)npobj)->HasMethod (name);
}

bool RRefClassObject::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RRefClassObject*)npobj)->Invoke (name, args, argCount, result);
}

bool RRefClassObject::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RRefClassObject*)npobj)->InvokeDefault (args, argCount, result);
}

bool RRefClassObject::_HasProperty(NPObject * npobj, NPIdentifier name)
{
	return ((RRefClassObject*)npobj)->HasProperty (name);
}

bool RRefClassObject::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
	return ((RRefClassObject*)npobj)->GetProperty (name, result);
}

bool RRefClassObject::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
	return ((RRefClassObject*)npobj)->SetProperty (name, value);
}

bool RRefClassObject::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
	return ((RRefClassObject*)npobj)->RemoveProperty (name);
}

bool RRefClassObject::_Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
	return ((RRefClassObject*)npobj)->Enumerate (identifier, count);
}

bool RRefClassObject::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RRefClassObject*)npobj)->Construct (args, argCount, result);
}


NPClass RRefClassObject::_npclass = {                              
  NP_CLASS_STRUCT_VERSION,				                          
  RRefClassObject::Allocate,                                       
  RRefClassObject::_Deallocate,                                    
  RRefClassObject::_Invalidate,                                    
  RRefClassObject::_HasMethod,                                     
  RRefClassObject::_Invoke,                                        
  RRefClassObject::_InvokeDefault,                                 
  RRefClassObject::_HasProperty,                                   
  RRefClassObject::_GetProperty,                                   
  RRefClassObject::_SetProperty,                                   
  RRefClassObject::_RemoveProperty,                                
  RRefClassObject::_Enumerate,                                     
  RRefClassObject::_Construct                                      
};





RVector::RVector (NPP instance) 
{
  this->instance = instance;
  this->object = NULL;
  this->converter = NULL;
}

NPObject *RVector::Allocate(NPP npp, NPClass *aClass)
{
	NPObject *pObj = (NPObject *)new RVector(npp);
	return pObj;
}


bool RVector::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  if(name == this->funcs->getstringidentifier("convert"))
    {
      if(this->converter != NULL)
	{
	  funcs->setexception(this, "User assigned JavaScript converters are not supported at this time. If you need this functionality please contact the maintainer");
	  fprintf(stderr, "\nUser assigned JavaScript converters are not supported at this time. If you need this functionality please contact the maintainer.\n");fflush(stderr);
	  return false;
	}
      //try to force conversion (copying)
      ConvertRToNP(this->object, this->instance, this->funcs, result, CONV_COPY); 
      return true;
    } else if (name == this->funcs->getstringidentifier("toString")) 
    {
      fprintf(stderr, "\nIn tostring method of an RVector\n");fflush(stderr);
      //From NPN_ReleaseVariantValue docs: NPN_ReleaseVariantValue() will call NPN_ReleaseObject() on NPVariants of type NPVARIANTTYPE_OBJECT, and NPN_FreeMem() on NPVariants of type NPVARIANTTYPE_STRING. 
      NPUTF8 *strdat = (NPUTF8*) this->funcs->memalloc(19+1);
      //strdat = (NPUTF8*)"[Internal R Object]";
      memcpy(strdat, "[Internal R Vector]", 19+1);
      NPString str ={ strdat, 19};
      result->type = NPVariantType_String;
      result->value.stringValue = str;     
      return true;
    }
  else if (name == this->funcs->getstringidentifier("length")) 
    {
      result->type = NPVariantType_Int32;
      result->value.intValue = LENGTH(this->object);
      return true;
    }
  return false;
}

NPClass RVector::_npclass = {                              
  NP_CLASS_STRUCT_VERSION,								                          
  RVector::Allocate,                                       
  RVector::_Deallocate,                                    
  RVector::_Invalidate,                                    
  RVector::_HasMethod,                                     
  RVector::_Invoke,                                        
  RVector::_InvokeDefault,                                 
  RVector::_HasProperty,                                   
  RVector::_GetProperty,                                   
  RVector::_SetProperty,                                   
  RVector::_RemoveProperty,                                
  RVector::_Enumerate,                                     
  RVector::_Construct                                      
};



NPObject *RList::Allocate(NPP npp, NPClass *aClass)
{
	NPObject *pObj = (NPObject *)new RList(npp);
	return pObj;
}


RList::RList (NPP instance) 
{
  this->instance = instance;
  this->object = NULL;
  this->converter = NULL;
}


bool RList::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  if(name == this->funcs->getstringidentifier("convert"))
    {
      if(this->converter != NULL)
	{
	  funcs->setexception(this, "User assigned JavaScript converters are not supported at this time. If you need this functionality please contact the maintainer");
	  fprintf(stderr, "\nUser assigned JavaScript converters are not supported at this time. If you need this functionality please contact the maintainer.\n");fflush(stderr);
	  return false;
	}
      //try to force conversion (copying)
      ConvertRToNP(this->object, this->instance, this->funcs, result, CONV_COPY); 
      return true;
    } else if (name == this->funcs->getstringidentifier("toString")) 
    {
      fprintf(stderr, "\nIn tostring method of an RVector\n");fflush(stderr);
      //From NPN_ReleaseVariantValue docs: NPN_ReleaseVariantValue() will call NPN_ReleaseObject() on NPVariants of type NPVARIANTTYPE_OBJECT, and NPN_FreeMem() on NPVariants of type NPVARIANTTYPE_STRING. 
      NPUTF8 *strdat = (NPUTF8*) this->funcs->memalloc(17+1);
      //strdat = (NPUTF8*)"[Internal R Object]";
      memcpy(strdat, "[Internal R List]", 17+1);
      NPString str ={ strdat, 17};
      result->type = NPVariantType_String;
      result->value.stringValue = str;     
      return true;
    }
  else if (name == this->funcs->getstringidentifier("length")) 
    {
      result->type = NPVariantType_Int32;
      result->value.intValue = LENGTH(this->object);
      return true;
    }
  return false;
}



NPClass RList::_npclass = {
 NP_CLASS_STRUCT_VERSION,								                          
  RList::Allocate,                                       
  RSubsettableObject::_Deallocate,                                    
  RSubsettableObject::_Invalidate,                                    
  RSubsettableObject::_HasMethod,                                     
  RSubsettableObject::_Invoke,                                        
  RSubsettableObject::_InvokeDefault,                                 
  RSubsettableObject::_HasProperty,                                   
  RSubsettableObject::_GetProperty,                                   
  RSubsettableObject::_SetProperty,                                   
  RSubsettableObject::_RemoveProperty,                                
  RSubsettableObject::_Enumerate,                                     
  RSubsettableObject::_Construct                                      
};


void RSubsettableObject::Deallocate()
{
  if(this->object)
    {
      R_ReleaseObject(this->object);
      this->object=NULL;
    }
}

void RSubsettableObject::Invalidate()
{
}

bool RSubsettableObject::HasMethod(NPIdentifier name)
{
  int ret = 0;
  if(name == this->funcs->getstringidentifier("convert"))
    ret = 1;
  else if(name == this->funcs->getstringidentifier("toString"))
    ret = 1;
  else if(name == this->funcs->getstringidentifier("length"))
    ret = 1;
  
  return (bool) ret;
}


bool RSubsettableObject::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  if(name == this->funcs->getstringidentifier("convert"))
    {
      if(this->converter != NULL)
	{
	  funcs->setexception(this, "User assigned JavaScript converters are not supported at this time. If you need this functionality please contact the maintainer");
	  fprintf(stderr, "\nUser assigned JavaScript converters are not supported at this time. If you need this functionality please contact the maintainer.\n");fflush(stderr);
	  return false;
	}
      //try to force conversion (copying)
      ConvertRToNP(this->object, this->instance, this->funcs, result, CONV_COPY); 
      return true;
    } else if (name == this->funcs->getstringidentifier("toString")) 
    {
      fprintf(stderr, "\nIn tostring method of an RSubsettableObject\n");fflush(stderr);
      //From NPN_ReleaseVariantValue docs: NPN_ReleaseVariantValue() will call NPN_ReleaseObject() on NPVariants of type NPVARIANTTYPE_OBJECT, and NPN_FreeMem() on NPVariants of type NPVARIANTTYPE_STRING. 
      NPUTF8 *strdat = (NPUTF8*) this->funcs->memalloc(19+1);
      //strdat = (NPUTF8*)"[Internal R Object]";
      memcpy(strdat, "[Internal R Vector]", 19+1);
      NPString str ={ strdat, 19};
      result->type = NPVariantType_String;
      result->value.stringValue = str;     
      return true;
    }
  else if (name == this->funcs->getstringidentifier("length")) 
    {
      result->type = NPVariantType_Int32;
      result->value.intValue = LENGTH(this->object);
      return true;
    }
  return false;
}

bool RSubsettableObject::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  return false;
}

bool RSubsettableObject::HasProperty(NPIdentifier name)
{
  bool ret= true;
  fprintf(stderr, "\nIn RSubsettableObject::HasProperty");fflush(stderr);
  //we need to return false for things that need to be methods.
  if(name == this->funcs->getstringidentifier("convert"))
    ret = 0;
  else if(name == this->funcs->getstringidentifier("toString"))
    ret = 0;
  else if(name == this->funcs->getstringidentifier("valueOf"))
    ret = 0;
  else if(name == this->funcs->getstringidentifier("isRObject"))
    ret = 1;
  else if(name == this->funcs->getstringidentifier("length"))
    ret = 0;
  else
    {

      SEXP call, ans, ptr;
      int error=0;
      PROTECT(ptr = call = allocVector(LANGSXP, 3));
      SETCAR(ptr, Rf_install("CheckForProperty"));
      ptr = CDR(ptr);
      SETCAR(ptr, this-> object);
      ptr = CDR(ptr);
      if(this->funcs->identifierisstring(name))
	SETCAR(ptr, ScalarString(mkChar(this->funcs->utf8fromidentifier(name))));
      else
	SETCAR(ptr, ScalarInteger(this->funcs->intfromidentifier(name)));
      PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));

      if(!error)
	ret = LOGICAL(ans)[0];
      else
	ret = false;
      UNPROTECT(2);
    }
  return ret;
}

bool RSubsettableObject::GetProperty(NPIdentifier name, NPVariant *result)
{
  if(name == this->funcs->getstringidentifier("isRObject"))
    {
      BOOLEAN_TO_NPVARIANT(true, *result); 
      return true;
    }
  //Emulate object[[name]], object$name, object@name in that order
  fprintf(stderr, "\nIn RSubsettableObject::GetProperty");fflush(stderr);
  SEXP call, ptr, ans;
  int error;
  PROTECT(ptr = call = allocVector(LANGSXP, 3));
  SETCAR(ptr, Rf_install("[["));
  ptr = CDR(ptr);
  SETCAR(ptr, this->object);
  ptr = CDR(ptr);
  if(this->funcs->identifierisstring(name))
    SETCAR(ptr, ScalarString(mkChar(this->funcs->utf8fromidentifier(name))));
  else
    {
      //+1 so that on the javascript side vectors are 0-indexed
      SETCAR(ptr, ScalarInteger(this->funcs->intfromidentifier(name) + 1));
    }
  PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));
  bool ret = ConvertRToNP(ans, this->instance, this->funcs, result, CONV_DEFAULT);
  UNPROTECT(2);
  //ConvertRToNP(ans, Robj->instance, funcs, result, false);
  return ret;
}

bool RSubsettableObject::SetProperty(NPIdentifier name, const NPVariant *value)
{
  //Emulate object[[name]]<-value
  bool ret = false;
  fprintf(stderr, "\nIn RSubsettableObject::SetProperty");fflush(stderr);
  SEXP call, ptr, ans, tmp;
  int error = 0;
  //[[<- doesn't work because a lot of times these objects are not assigned to any symbols.
  PROTECT(ptr = call = allocVector(LANGSXP, 4));
  SETCAR(ptr, Rf_install("[[<-"));
  ptr = CDR(ptr);
  SETCAR(ptr, this->object);
  ptr = CDR(ptr);
  if(this->funcs->identifierisstring(name))
    SETCAR(ptr, ScalarString(mkChar(this->funcs->utf8fromidentifier(name))));
  else
    SETCAR(ptr, ScalarInteger(this->funcs->intfromidentifier(name)));

  ConvertNPToR((NPVariant *)value, this->instance, this->funcs, CONV_DEFAULT, &tmp);
  ptr = CDR(ptr);
  SETCAR(ptr, tmp);
  //XXX This is getting garbage collected... and causing crashes
   PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance));
  //ans = rQueue.requestRCall(call, R_GlobalEnv, &error, this->instance);

  if(!error)
    {
      this->object = ans;

    }
  ret = (bool) !error;

  UNPROTECT(2);
  return ret;
}

bool RSubsettableObject::RemoveProperty(NPIdentifier name)
{
	return false;
}

bool RSubsettableObject::Enumerate(NPIdentifier **identifier, uint32_t *count)
{
    fprintf(stderr, "\nIn RSubsettableObject::Enumerate");fflush(stderr);
	return false;
}

bool RSubsettableObject::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return true;
}
/*
NPObject *RSubsettableObject::Allocate(NPP npp, NPClass *aClass)
{
	NPObject *pObj = (NPObject *)new RVector(npp);
	return pObj;
}
*/
void RSubsettableObject::Detatch (void)
{
	m_Instance = NULL;
}



void RSubsettableObject::_Deallocate(NPObject *npobj)
{
	RSubsettableObject *pObj = ((RSubsettableObject *) npobj);

  // Call the virtual destructor.
	pObj->Deallocate ();
	delete pObj;
}

void RSubsettableObject::_Invalidate(NPObject *npobj)
{
	((RSubsettableObject*)npobj)->Invalidate();
}

bool RSubsettableObject::_HasMethod(NPObject *npobj, NPIdentifier name)
{
	return ((RSubsettableObject*)npobj)->HasMethod (name);
}

bool RSubsettableObject::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RSubsettableObject*)npobj)->Invoke (name, args, argCount, result);
}

bool RSubsettableObject::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RSubsettableObject*)npobj)->InvokeDefault (args, argCount, result);
}

bool RSubsettableObject::_HasProperty(NPObject * npobj, NPIdentifier name)
{
	return ((RSubsettableObject*)npobj)->HasProperty (name);
}

bool RSubsettableObject::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
	return ((RSubsettableObject*)npobj)->GetProperty (name, result);
}

bool RSubsettableObject::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
	return ((RSubsettableObject*)npobj)->SetProperty (name, value);
}

bool RSubsettableObject::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
	return ((RSubsettableObject*)npobj)->RemoveProperty (name);
}

bool RSubsettableObject::_Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
	return ((RSubsettableObject*)npobj)->Enumerate (identifier, count);
}

bool RSubsettableObject::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RSubsettableObject*)npobj)->Construct (args, argCount, result);
}




RNAValue::RNAValue (NPP instance) 
{
  this->instance = instance;
  this->type = 0;
  this->funcs = NULL;
}

void RNAValue::Deallocate()
{

}

void RNAValue::Invalidate()
{
}

bool RNAValue::HasMethod(NPIdentifier name)
{
  int ret = 0;
  if(name == this->funcs->getstringidentifier("toString"))
    ret = 1;
  
  return (bool) ret;
}


bool RNAValue::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  if (name == this->funcs->getstringidentifier("toString")) 
    {
      fprintf(stderr, "\nIn tostring method of an RNAValue\n");fflush(stderr);
      //From NPN_ReleaseVariantValue docs: NPN_ReleaseVariantValue() will call NPN_ReleaseObject() on NPVariants of type NPVARIANTTYPE_OBJECT, and NPN_FreeMem() on NPVariants of type NPVARIANTTYPE_STRING. 
      const char *msg = "[Internal R NA Value]";
      NPUTF8 *strdat = (NPUTF8*) this->funcs->memalloc(strlen(msg)+1);
      //strdat = (NPUTF8*)"[Internal R Object]";
      memcpy(strdat, msg, strlen(msg)+1);
      NPString str ={ strdat, strlen(msg)};
      result->type = NPVariantType_String;
      result->value.stringValue = str;     
      return true;
    }
    
  return false;
}

bool RNAValue::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  return false;
}

bool RNAValue::HasProperty(NPIdentifier name)
{
  bool ret= false;
  fprintf(stderr, "\nIn RNAValue::HasProperty");fflush(stderr);
  //we need to return false for things that need to be methods.
  if(name == this->funcs->getstringidentifier("_R_NA_Type"))
    ret = true;
  return ret;
}

bool RNAValue::GetProperty(NPIdentifier name, NPVariant *result)
{
  if(name == this->funcs->getstringidentifier("_R_NA_Type"))
    {
      INT32_TO_NPVARIANT(this->type, *result); 
      return true;
    }
  return false;
}

bool RNAValue::SetProperty(NPIdentifier name, const NPVariant *value)
{
  //Emulate object[[name]]<-value
	return false;
}

bool RNAValue::RemoveProperty(NPIdentifier name)
{
	return false;
}

bool RNAValue::Enumerate(NPIdentifier **identifier, uint32_t *count)
{
    fprintf(stderr, "\nIn RNAValue::Enumerate");fflush(stderr);
	return false;
}

bool RNAValue::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return true;
}

NPObject *RNAValue::Allocate(NPP npp, NPClass *aClass)
{
	NPObject *pObj = (NPObject *)new RNAValue(npp);
	return pObj;
}

void RNAValue::Detatch (void)
{
	m_Instance = NULL;
}



void RNAValue::_Deallocate(NPObject *npobj)
{
	RNAValue *pObj = ((RNAValue *) npobj);

  // Call the virtual destructor.
	pObj->Deallocate ();
	delete pObj;
}

void RNAValue::_Invalidate(NPObject *npobj)
{
	((RNAValue*)npobj)->Invalidate();
}

bool RNAValue::_HasMethod(NPObject *npobj, NPIdentifier name)
{
	return ((RNAValue*)npobj)->HasMethod (name);
}

bool RNAValue::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RNAValue*)npobj)->Invoke (name, args, argCount, result);
}

bool RNAValue::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RNAValue*)npobj)->InvokeDefault (args, argCount, result);
}

bool RNAValue::_HasProperty(NPObject * npobj, NPIdentifier name)
{
	return ((RNAValue*)npobj)->HasProperty (name);
}

bool RNAValue::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
	return ((RNAValue*)npobj)->GetProperty (name, result);
}

bool RNAValue::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
	return ((RNAValue*)npobj)->SetProperty (name, value);
}

bool RNAValue::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
	return ((RNAValue*)npobj)->RemoveProperty (name);
}

bool RNAValue::_Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
	return ((RNAValue*)npobj)->Enumerate (identifier, count);
}

bool RNAValue::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((RNAValue*)npobj)->Construct (args, argCount, result);
}


NPClass RNAValue::_npclass = {                              
  NP_CLASS_STRUCT_VERSION,								                          
  RNAValue::Allocate,                                       
  RNAValue::_Deallocate,                                    
  RNAValue::_Invalidate,                                    
  RNAValue::_HasMethod,                                     
  RNAValue::_Invoke,                                        
  RNAValue::_InvokeDefault,                                 
  RNAValue::_HasProperty,                                   
  RNAValue::_GetProperty,                                   
  RNAValue::_SetProperty,                                   
  RNAValue::_RemoveProperty,                                
  RNAValue::_Enumerate,                                     
  RNAValue::_Construct                                      
};

