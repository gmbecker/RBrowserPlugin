#include "WebR.h"


RFunction::RFunction (NPP instance)
{


  //this->m_getVersion_id = NPN_GetStringIdentifier("getVersion");
  //  this->m_getVersion_id = myNPNFuncs->getstringidentifier("getVersion");
  //fprintf(stderr, "\nCreating RFunction object. Instance:%lx", (unsigned long long int) instance);fflush(stderr);
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
  if(name == this->funcs->getstringidentifier("valueOf"))
    ret = 1;
  if(name == this->funcs->getstringidentifier("handleEvent"))
    ret = 1;
  if(name == this->funcs->getstringidentifier("call"))
    ret = 1;
  return (bool) ret;
}


bool RFunction::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  fprintf(stderr, "\nLooking for method: %s on RFunction object.", this->funcs->utf8fromidentifier(name));fflush(stderr);
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
  if (name == this->funcs->getstringidentifier("valueOf"))
    {
      fprintf(stderr, "\nIn valueOf method of an RFunction\n");fflush(stderr);
      NPUTF8 *strdat = (NPUTF8*) this->funcs->memalloc(21+1);
      //strdat = (NPUTF8*)"[Internal R Object]";
      memcpy(strdat, "[Internal R Function]", 21+1);
      NPString str ={ strdat, 21};
      result->type = NPVariantType_String;
      result->value.stringValue = str;

      return true;

    }
  if (name == this->funcs->getstringidentifier("handleEvent"))
    {
      return this->InvokeDefault(args, argCount, result);
    }
  if (name == this->funcs->getstringidentifier("call"))
    {
      return this->InvokeDefault(args, argCount, result);
    }
  return false;
}

bool RFunction::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{

  if(argCount && args[0].type == NPVariantType_Object && this->funcs->hasproperty(this->instance, args[0].value.objectValue, this->funcs->getstringidentifier("namedArgsForR")))
    {
      return doNamedCall(this->instance, this->object, args, argCount, result, this->funcs);
    }

  SEXP Rargs[argCount];
  convert_t convRet = CONV_DEFAULT;
  NPVariant convRetVariant;
  int numprot = 0;  
  bool canfree;
  uint32_t  j=0;
  bool wasConvRet;
  bool retained[argCount];
  //argCountR is the number of arguments to be actually passed to the R function. Does not include, e.g. convertRet specification args
  int argCountR = argCount;
  //i is position in JS args, j is position in converted R args  
  for(uint32_t i=0; i< argCount; i++)
  {
    wasConvRet = false;
    
    //If the argument is not an "emptyArg" object, indicating, eg, foo(a, , c), convert as normal
    if(args[i].type == NPVariantType_Object && this->funcs->hasproperty(this->instance, args[i].value.objectValue, this->funcs->getstringidentifier("_convertRet")))
      {
	wasConvRet = true;
	funcs->getproperty(this->instance, args[i].value.objectValue, this->funcs->getstringidentifier("behavior"), &convRetVariant);
	if(convRetVariant.type==NPVariantType_Int32) 
	  convRet = (convert_t) convRetVariant.value.intValue;
	else if ( convRetVariant.type==NPVariantType_Double)
	  convRet = (convert_t) convRetVariant.value.doubleValue;
	else
	  convRet = CONV_CUSTOM;
      } else if(args[i].type == NPVariantType_Object && this->funcs->hasproperty(this->instance, args[i].value.objectValue, this->funcs->getstringidentifier("emptyRArg")))
      {
      //If it is the special emptyRArg object, it represents a missing argument, eg rnorm(2, , 5)
	PROTECT(Rargs[j] = R_NilValue);
	Rargs[j] = R_MissingArg;
	numprot++;
      }
    else
      {
	PROTECT(Rargs[j] = R_NilValue);
	//We need to retain this because we are calling R which can initiate another conversion before we return and I think some things (eg events in raphZoom) are getting improperly freed during the inner conversion cycle
	if(NPVARIANT_IS_OBJECT(args[i]))
	  {
	    this->funcs->retainobject(args[i].value.objectValue);
	    retained[ i ] = true;
	  } else {
	  retained[ i ] =  false;
	}

	ConvertNPToR((NPVariant *) &(args[i]), this->instance, this->funcs, CONV_DEFAULT, &Rargs[j]);
	numprot++;
      }
    //If the argument was a convertRet specification it doesn't count as a "real argument", we decrease the argument count and do not increment i
    if(wasConvRet)
      argCountR--;
    else
      j++;
  }
  SEXP ans;
  SEXP call;
  int error = 0;
  int addProt = 0;

  SEXP ptr;
  //argCountR is the number of arguments after we remove any convertRet specifiers
  PROTECT(ptr = call = allocVector(LANGSXP, argCountR  + 1));
  SETCAR(ptr, (SEXP) this->object );
  for(uint32_t i=0; i < argCountR; i++)
    {
      ptr = CDR( ptr );
 	SETCAR(ptr, Rargs[i]);

    }

  Rf_PrintValue(call);
  //PROTECT(ans = R_tryEval( call, R_GlobalEnv, &error));
  PROTECT(ans = rQueue.requestRCall( call, R_GlobalEnv, &error, this->instance));

  addProt = 2;
  if(!error)
    {
    //ConvertRToNP(ans, this->instance, this->funcs, result, CONV_DEFAULT);
      ConvertRToNP(ans, this->instance, this->funcs, result, convRet);
      if(convRet == CONV_CUSTOM)
	this->funcs->invokeDefault(this->instance, convRetVariant.value.objectValue, result, 1, result);
    }
  //If it's an error, just throw an error for the browser.
  else
    {
      ThrowRError(this, this->funcs);
      ConvertRToNP(R_NilValue, this->instance, this->funcs, result, CONV_DEFAULT);
    }
  UNPROTECT(numprot + addProt);
//There is a bug in chrome where if an NPObject method call returns false NPN_SetException doesn't work. I'm going to experiment with always returning true...
  //return !error;

  //Unretain objects now that we are done calling R
  for(int k = 0; k < argCount; k++)
    {
      if(retained[k])
	this->funcs->releasevariantvalue((NPVariant *) &args[k]);
    }
  return true;
  
}

bool RFunction::HasProperty(NPIdentifier name)
{
  fprintf(stderr, "\nLooking for property: %s on RFunction object.", this->funcs->utf8fromidentifier(name));fflush(stderr);
  bool ret = false;
  if (name == this->funcs->getstringidentifier("handleEvent"))
    ret = true;
  else if(name == this->funcs->getstringidentifier("isRObject"))
    ret = true;
  return ret;
}

bool RFunction::GetProperty(NPIdentifier name, NPVariant *result)
{
  if (name == this->funcs->getstringidentifier("handleEvent"))
    return this->InvokeDefault(NULL, 0, result);
  else if(name == this->funcs->getstringidentifier("isRObject"))
    {
      BOOLEAN_TO_NPVARIANT(true, *result);
      return true;
    }

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


/*
bool RFunction_GetProp(RFunction *Robj, NPIdentifier name, NPNetscapeFuncs *funcs, NPVariant *result, bool check, NPP inst)
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

  PROTECT(ans = rQueue.requestRCall( call, R_GlobalEnv, &waserr, Robj->instance));
  if(!waserr && !IsMissing(ans, true))
    {
      //non-missing, non-null result. stop looking
      toret = 1;
    } else {
  //try $
    ptr = call;
    SETCAR(ptr, Rf_install("$"));
    ans = rQueue.requestRCall( call, R_GlobalEnv, &waserr, Robj->instance);
    if(!waserr && !IsMissing(ans, true))
      toret = 1;
    else
      {
	//try @
	ptr = call;
	SETCAR(ptr, Rf_install("@"));
	ans = rQueue.requestRCall( call, R_GlobalEnv, &waserr, Robj->instance);
	if(!waserr && !IsMissing(ans, false))
	  toret = 1;
	else
	  {
	    ans = NEW_NUMERIC(1);
	    REAL(ans)[0] = NA_REAL;
	  }
      }
  }

  ConvertRToNP(ans, Robj->instance, funcs, result, CONV_REF);
  if(!check)
    return true;
  else
    return toret;
}
*/
bool doNamedCall(NPP inst, SEXP fun, const NPVariant *argsIn, uint32_t count, NPVariant *_res, NPNetscapeFuncs *funcs)
{
  fprintf(stderr, "\nAttempting to create R call with named arguments\n");fflush(stderr);
  uint32_t idcount = 0;
   NPIdentifier *ids;
  NPObject *obj = argsIn[0].value.objectValue;
  bool success = funcs->enumerate(inst, obj, &ids, &idcount);
  SEXP call, ans, ptr, tmp;
  NPVariant curprop;
  NPVariant *convFun = (NPVariant *) funcs->memalloc(sizeof(NPVariant)) ;
  convert_t conv= CONV_DEFAULT;
  int len = 1 + idcount;

  if(funcs->hasproperty(inst, obj, funcs->getstringidentifier("namedArgsForR")))
    len = len -1; //the namedArgsForR property isn't going to be used as an argument in the function call
  if(funcs->hasproperty(inst, obj, funcs->getstringidentifier("convertRet")))
    len = len -1; //the convertRet property isn't going to be used as an argument in the function call
  if(funcs->hasproperty(inst, obj, funcs->getstringidentifier("convertFun")))
    len = len -1; //the convertFun property isn't going to be used as an argument in the function call
  PROTECT(ptr = call = allocVector(LANGSXP, len));
  SETCAR(ptr, fun);
  PROTECT(tmp = R_NilValue);

  for(int i =0; i < idcount; i++)
    {
      funcs->getproperty(inst, obj, ids[i], &curprop);
      if(ids[i] == funcs->getstringidentifier("convertRet"))
	{
	  //switched between 3 options on Javascript side and converted to number
	  
	  if(NPVARIANT_IS_INT32(curprop))
	    conv = (convert_t) curprop.value.intValue;
	  else
	    conv = (convert_t) curprop.value.doubleValue;
	}
     else if (ids[i] == funcs->getstringidentifier("convertFun"))
	{
	  *convFun = curprop;
	  //	  conv = CONV_REF;

	}
      else if(curprop.type == NPVariantType_Object && !funcs->hasproperty(inst, curprop.value.objectValue, funcs->getstringidentifier("emptyRArg")))
	{
	  ptr = CDR(ptr);
	  //empty argument, ie 2nd "argument" in foo(a, , c)
	  SETCAR(ptr,  R_MissingArg);
	  
	}
      else if(ids[i] != funcs->getstringidentifier("namedArgsForR"))
	{
	  fprintf(stderr, "\nAccessing property %s\n", funcs->utf8fromidentifier(ids[i]));fflush(stderr);
	  ptr = CDR(ptr);
	  //funcs->getproperty(inst, obj, ids[i], &curprop);
	  ConvertNPToR(&curprop, inst, funcs, CONV_DEFAULT, &tmp);
	  SETCAR(ptr, tmp);
	  SET_TAG(ptr, Rf_install((const char *) funcs->utf8fromidentifier(ids[i])));
	}
    }

  fprintf(stderr, "\nFull call:\n");fflush(stderr);
  Rf_PrintValue(call);
  int err = 0;
  PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &err, inst));
  //NPVariant *tmpres = 
  
  ConvertRToNP(ans, inst, funcs, _res, conv);
  //ConvertRToNP(ans, inst, funcs, tmpres, conv);
  //call custom conversion method if available
  if(conv == 3)
    {
      funcs->invokeDefault(inst, convFun->value.objectValue, _res, 1, _res);
    }
  funcs->memfree(ids);
  funcs->releasevariantvalue(convFun);
  
  return !err;
}
