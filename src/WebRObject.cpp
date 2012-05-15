#include "WebR.h"


RObject::RObject (NPP instance) 
{
  

  //this->m_getVersion_id = NPN_GetStringIdentifier("getVersion");
  //  this->m_getVersion_id = myNPNFuncs->getstringidentifier("getVersion");
  fprintf(stderr, "\nCreating RObject object. Instance:%lx", instance);fflush(stderr);
  this->instance = instance;
  this->object = NULL;
  this->converter = NULL;
  //XXX Need to retain this object so it doesn't get gc'ed. Someone posted on rdevel mentioning this somewhat recently. Simon maybe?
  fprintf(stderr, "\nLeaving RObject()\n");fflush(stderr);
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
  NPVariant res;
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
      PROTECT(ans = R_tryEval(call, R_GlobalEnv, &error));
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
  
  PROTECT(ans = R_tryEval(call, R_GlobalEnv, &error));
  bool ret = ConvertRToNP(ans, this->instance, this->funcs, result, false);
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
  fprintf(stderr, "\nIn RObject::Allocate\n");fflush(stderr);
  
	NPObject *pObj = (NPObject *)new RObject(npp);
	return pObj;
	fprintf(stderr, "\nLeaving RObject::Allocate\n");fflush(stderr);
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

 
bool RObject_GetProp(RObject *Robj, NPIdentifier name, NPNetscapeFuncs *funcs, NPVariant *result, bool check)
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

    /* Same code for LISTSXP and VECSXP : */
 
  case VECSXP:   
  case LISTSXP:
    ret = IsMissing(VECTOR_ELT(obj, 0), nullAlso);
    break;
  case RAWSXP:
    /* no such thing as a raw NA */
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
