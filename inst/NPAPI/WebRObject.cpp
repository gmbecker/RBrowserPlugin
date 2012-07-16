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
      ConvertRToNP(this->object, this->instance, this->funcs, result, true); 
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
  bool ret = ConvertRToNP(ans, this->instance, this->funcs, result, true);
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
      ConvertRToNP(this->object, this->instance, this->funcs, result, true); 
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
  bool ret = ConvertRToNP(ans, this->instance, this->funcs, result, true);
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
      ConvertNPToR((NPVariant *)value, this->instance, this->funcs, true, &tmp);
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

/* 
bool RObject_GetProp(RObject *Robj, NPIdentifier name, NPNetscapeFuncs *funcs, NPVariant *result, bool check, NPP inst)
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
  
  PROTECT(ans = rQueue.requestRCall( call, R_GlobalEnv, &waserr, inst));
  if(!waserr && !IsMissing(ans, true))
    {
      //non-missing, non-null result. stop looking
      toret = 1;
    } else {
  //try $
    ptr = call;
    SETCAR(ptr, Rf_install("$"));
    ans = rQueue.requestRCall( call, R_GlobalEnv, &waserr, inst);
    if(!waserr && !IsMissing(ans, true))
      toret = 1;
    else
      {
	//try @
	ptr = call;
	SETCAR(ptr, Rf_install("@"));
	ans = rQueue.requestRCall( call, R_GlobalEnv, &waserr, inst);
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


bool IsMissing(SEXP obj, bool nullAlso)
{
  
  if(LENGTH(obj) > 1)
    return true;
  
  if(nullAlso && obj == R_NilValue)
    return true;
  bool ret;
  //Modified from do_isna in coerce.c
  switch (TYPEOF(obj)) {
  case LGLSXP:
    ret = (LOGICAL(obj)[0] == NA_LOGICAL);
    break;
  case INTSXP:
    ret = (INTEGER(obj)[0] == NA_INTEGER);
    break;
  case REALSXP:
    ret = ISNAN(REAL(obj)[0]);
    break;
  case CPLXSXP:
    ret = (ISNAN(COMPLEX(obj)[0].r) ||
	   ISNAN(COMPLEX(obj)[0].i));
    break;
  case STRSXP:
    ret = (STRING_ELT(obj, 0) == NA_STRING);
    break;

  
 
  case VECSXP:   
  case LISTSXP:
    ret = IsMissing(VECTOR_ELT(obj, 0), nullAlso);
    break;
  case RAWSXP:
  
    ret = 0;
    break;
  default:
    {
      fprintf(stderr, "%s() applied to non-(list or vector) of type '%s'", "IsMissing", type2char(TYPEOF(obj)));
      fflush(stderr);
      ret = 0;
    }
  }  
	return ret;
}

*/


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
      ConvertRToNP(this->object, this->instance, this->funcs, result, true); 
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
  bool ret = ConvertRToNP(ans, this->instance, this->funcs, result, true);
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
      
      bool success = ConvertNPToR((NPVariant *)value, this->instance, this->funcs, true, &val);
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
