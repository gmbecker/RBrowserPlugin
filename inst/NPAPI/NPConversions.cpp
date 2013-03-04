#include "WebR.h"

bool ConvertRToNP(SEXP val, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret, convert_t convRes)
{

//If it is a promise we need the actual value.  
  int err = 0;
  int numprot = 0;
  if(TYPEOF(val) == PROMSXP)
    {
      PROTECT(val = rQueue.requestRCall(val, R_GlobalEnv, &err, inst));
      numprot = 1;
    }
  //Is it already a reference to an existing NP/JS object? If so just return that object!
  if (CheckSEXPForJSRef(val, inst))
    {
      //XXX We shouldn't have to copy here, but do we really want to pass in double pointers?
      *ret = *(NPVariant *) R_ExternalPtrAddr(GET_SLOT( val , Rf_install( "ref" ) ) );
      return true;
    }

  if(convRes == CONV_COPY)
    {
      MakeCopyRToNP(val, inst, funcs, ret);
      return true;
    }
  if(convRes == CONV_REF || convRes == CONV_CUSTOM)
    {
      MakeRRefForNP(val, inst, funcs, ret);
      return true;
    }
 
  // I don't think we need this check here, it happens in MakeCopyRToNP.
  // if(checkRForNA(val))
  // {
  //    makeNAForNP(TYPEOF(val), ret);
  //   return true;
  // }
  //Default marshalling behavior
  switch(TYPEOF(val))
    {
    case NILSXP:
      break;
    case CHARSXP:
      MakeCopyRToNP(val, inst, funcs, ret);
      break;
    case REALSXP:
    case INTSXP:
    case LGLSXP:
    case STRSXP:
      if(LENGTH(val) <= 1)
	MakeCopyRToNP(val, inst, funcs, ret);
      else
	MakeRRefForNP(val, inst, funcs, ret);
      break;
    case CLOSXP:
    case S4SXP:
    case VECSXP:
    default:
      MakeRRefForNP(val, inst, funcs, ret);
      break;
    }
  if(numprot)
    UNPROTECT(1);
  return true;
}

void MakeCopyRToNP(SEXP val, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret)
{
  
  if(IS_S4_OBJECT(val))
    CopyS4ToNP(val, inst, funcs, ret);
  else if(checkRForNA(val))
    {
      makeNAForNP(TYPEOF(val),  inst, funcs, ret);
    } 
  else 
    {
      int len = LENGTH(val);
      if(len == 0)
	return;
      

      switch(TYPEOF(val))
	{
	case NILSXP:
	  break;
	case REALSXP:
	  if(len > 1)
	    RVectorToNP(val, inst, funcs, ret);
	  else	  
	    DOUBLE_TO_NPVARIANT(REAL(val)[0], *ret);
	  break;
	case INTSXP:
	  if(len > 1)
	    RVectorToNP(val, inst, funcs, ret);
	  else
	    INT32_TO_NPVARIANT(INTEGER(val)[0], *ret);
	  break;
	case LGLSXP:
	  if(len > 1)
	    RVectorToNP(val, inst, funcs, ret);
	  else
	    BOOLEAN_TO_NPVARIANT( (bool) LOGICAL(val)[0], *ret);
	  break;
	case VECSXP:
	  RVectorToNP(val, inst, funcs, ret);
	  break;
	case STRSXP:
	  if(len > 1)
	    RVectorToNP(val, inst, funcs, ret);
	  else
	    {
	      //We need to use funcs->memalloc so that we can 
	      //safely call funcs->releasevariantvalue later...
	      const char *fromR = CHAR(STRING_ELT(val, 0));
	      //+1 for the null termination char
	      int len = strlen(fromR) + 1;
	      char *dat = (char *) funcs->memalloc(len*sizeof(char));
	      memcpy(dat, fromR, len);
	      STRINGZ_TO_NPVARIANT(dat, *ret);
	    }
	  break;
	case CHARSXP:
	  {

	      const char *fromR = CHAR(val);
	      fprintf(stderr, "\nProcessing CHARSXP: %s\n", fromR);fflush(stderr);
	      //+1 for the null termination char
	      int len = strlen(fromR) + 1;
	      char *dat = (char *) funcs->memalloc(len*sizeof(char));
	      memcpy(dat, fromR, len);
	      STRINGZ_TO_NPVARIANT(dat, *ret);
	  }
	case CLOSXP:
	  {
	    fprintf(stderr, "\nConverting R function to JavaScript function.");fflush(stderr);
	    NPObject *domwin = NULL;
	    NPError res;
	    res = funcs->getvalue(inst, NPNVWindowNPObject , &domwin);
	    NPVariant *vartmp = (NPVariant *) funcs->memalloc(sizeof(NPVariant));;
	    
	    MakeRRefForNP(val, inst, funcs,vartmp);
	    funcs->invoke(inst, domwin, funcs->getstringidentifier("makeFun"), vartmp, 1, ret);
	  }
	  break;
	default:
	  {
	    fprintf(stderr, "\nUnable to copy R value of type: %d. Creating reference.\n", TYPEOF(val));fflush(stderr); 
	    MakeRRefForNP(val, inst, funcs,ret);
	  }
	  break;
	}
    }
  return;
}


void CopyS4ToNP(SEXP val, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret)
{
  if(checkForRefClass(val))
    return CopyRefClassToNP(val, inst, funcs, ret);

  SEXP call, ptr, slots, tmp;
  int err = 0;
  NPObject *domwin;
  NPError res;
  NPIdentifier constructorId = funcs->getstringidentifier("Object");
  NPVariant *vartmp2 = (NPVariant *) funcs->memalloc(sizeof(NPVariant));

  res = funcs->getvalue(inst, NPNVWindowNPObject , &domwin);
  funcs->retainobject(domwin);
  funcs->invoke(inst, domwin, constructorId, NULL, 0, ret);
  PROTECT(ptr = call = allocVector(LANGSXP, 2));
  SETCAR(call, Rf_install("getSlotNames"));
  ptr = CDR(ptr);
  SETCAR(ptr, val);
  PROTECT(slots = R_tryEval(call, R_GlobalEnv, &err));
  int len = LENGTH(slots);
  for (int i = 0; i < len; i++)
    {
      ConvertRToNP(GET_SLOT(val, Rf_install(CHAR(STRING_ELT(slots, i)))), inst, funcs, vartmp2, CONV_DEFAULT); 
      funcs->setproperty(inst, ret->value.objectValue, funcs->getstringidentifier(CHAR(STRING_ELT(slots, i))), vartmp2);

    }

  vartmp2->type=NPVariantType_Bool;
  vartmp2->value.boolValue = true;
  funcs->setproperty(inst, ret->value.objectValue, funcs->getstringidentifier("__CopiedFromR__"), vartmp2);
  
  ConvertRToNP(GET_CLASS(val), inst, funcs, vartmp2, CONV_DEFAULT);
  funcs->setproperty(inst, ret->value.objectValue, funcs->getstringidentifier("__S4Class__"), vartmp2);
  UNPROTECT(2);
  funcs->memfree(vartmp2);
}

void CopyRefClassToNP(SEXP val, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret)
{
fprintf(stderr, "\nCopying of ReferenceClass objects is not supported. Creating Reference\n");fflush(stderr);
  MakeRRefForNP(val, inst, funcs, ret);
}
/*
bool ConvertRToNP(SEXP val, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret, convert_t convertRes)
{


//XXX If it is a promise we need the actual value. Will this come back to bite us by violating lazy loading?  
  int err = 0;
  if(TYPEOF(val) == PROMSXP)
    val = rQueue.requestRCall(val, R_GlobalEnv, &err, inst);
  if (CheckSEXPForJSRef(val, inst))
    {
      //XXX We shouldn't have to copy here, but do we really want to pass in double pointers?
      *ret = *(NPVariant *) R_ExternalPtrAddr(GET_SLOT( val , Rf_install( "ref" ) ) );
      return true;
	}

  if(convertRes == CONV_REF || (convertRes == CONV_DEFAULT && (IS_S4_OBJECT(val) || TYPEOF(val) == CLOSXP) ) ) 
    {
      MakeRRefForNP(val, inst, funcs, ret);
      return true;
    }



  int len = LENGTH(val);

  if(len > 0)
    {
      switch(TYPEOF(val))
	{
	case NILSXP:
	  break;
	case REALSXP:
	  if(len > 1)
	    RVectorToNP(val, inst, funcs, ret);
	  else	  
	    DOUBLE_TO_NPVARIANT(REAL(val)[0], *ret);
	  break;
	case INTSXP:
	  if(len > 1)
	    RVectorToNP(val, inst, funcs, ret);
	  else
	    INT32_TO_NPVARIANT(INTEGER(val)[0], *ret);
	  break;
	case LGLSXP:
	  if(len > 1)
	    RVectorToNP(val, inst, funcs, ret);
	  else
	    BOOLEAN_TO_NPVARIANT( (bool) LOGICAL(val)[0], *ret);
	  break;
	case VECSXP:
	  RVectorToNP(val, inst, funcs, ret);
	  break;
	case STRSXP:
	  if(len > 1)
	    RVectorToNP(val, inst, funcs, ret);
	  else
	    {
	      //We need to use funcs->memalloc so that we can 
	      //safely call funcs->releasevariantvalue later...
	      const char *fromR = CHAR(STRING_ELT(val, 0));
	      //+1 for the null termination char
	      int len = strlen(fromR) + 1;
	      char *dat = (char *) funcs->memalloc(len*sizeof(char));
	      memcpy(dat, fromR, len);
	      STRINGZ_TO_NPVARIANT(dat, *ret);
	    }
	  break;
	case CHARSXP:
	  {

	      const char *fromR = CHAR(val);
	      fprintf(stderr, "\nProcessing CHARSXP: %s\n", fromR);fflush(stderr);
	      //+1 for the null termination char
	      int len = strlen(fromR) + 1;
	      char *dat = (char *) funcs->memalloc(len*sizeof(char));
	      memcpy(dat, fromR, len);
	      STRINGZ_TO_NPVARIANT(dat, *ret);
	  }
	case CLOSXP:
	  {
	    fprintf(stderr, "\nConverting R function to JavaScript function.");fflush(stderr);
	    NPObject *domwin = NULL;
	    NPError res;
	    res = funcs->getvalue(inst, NPNVWindowNPObject , &domwin);
	    NPVariant *vartmp = (NPVariant *) funcs->memalloc(sizeof(NPVariant));;
	    
	    MakeRRefForNP(val, inst, funcs,vartmp);
	    funcs->invoke(inst, domwin, funcs->getstringidentifier("makeFun"), vartmp, 1, ret);
	  }
	  break;
	default:
	  {
	    fprintf(stderr, "\nUnrecognized type: %d. Creating reference.\n", TYPEOF(val));fflush(stderr); 
	    MakeRRefForNP(val, inst, funcs,ret);
	  }
	  break;
	}
    }
  return true;
}

*/
bool RVectorToNP(SEXP vec, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret)
{
  int len = LENGTH(vec);
  fprintf(stderr, "\n R vector or list of length: %d detected", len); fflush(stderr);
 
  NPObject *domwin = NULL;
  NPVariant *vartmp2 = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  NPVariant *vartmp3 = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  //NPVariant vartmp2;

  NPError res;
  bool named = !isNull(GET_NAMES(vec));
  res = funcs->getvalue(inst, NPNVWindowNPObject , &domwin);
  funcs->retainobject(domwin);
  NPIdentifier arrid;
  SEXP names;
  int protstack = 0;
  if(named)
    {
      fprintf(stderr, "\nVector has non-NULL names. Creating associative array\n");fflush(stderr);
      arrid = funcs->getstringidentifier("Object");
      PROTECT(names = GET_NAMES(vec));
      protstack++;
    }
  else
    arrid = funcs->getstringidentifier("Array");
  
   
  funcs->invoke(inst, domwin, arrid, NULL, 0, ret);
  //fprintf(stderr, "\nJS array object created.");fflush(stderr);
  SEXP el;
  PROTECT(el = R_NilValue);
  protstack++;
  for (int i = 0; i< len; i++)
    {
      switch(TYPEOF(vec))
	{
	case REALSXP:
	  vartmp2->type=NPVariantType_Double;
	  vartmp2->value.doubleValue = REAL(vec)[i];
	  break;
	case INTSXP:
	  vartmp2->type=NPVariantType_Int32;
	  vartmp2->value.intValue = INTEGER(vec)[i];
	  break;
	case LGLSXP:
	  vartmp2->type=NPVariantType_Bool;
	  vartmp2->value.boolValue = (bool) LOGICAL(vec)[i];
	  break;
	case VECSXP:
	  ConvertRToNP(VECTOR_ELT(vec, i), inst, funcs, vartmp2, CONV_DEFAULT);
	  break;
	case STRSXP:
	  ConvertRToNP(STRING_ELT(vec, i), inst, funcs, vartmp2, CONV_DEFAULT);
	  break;
	}

      if(named)
	funcs->setproperty(inst, ret->value.objectValue, funcs->getstringidentifier(CHAR(STRING_ELT(names, i))), vartmp2);
      else
	funcs->invoke(inst, ret->value.objectValue, funcs->getstringidentifier("push"), vartmp2, 1, vartmp3);
    }
  fprintf(stderr, "\nConversion loop complete.");fflush(stderr);

  if(named)
    {
      vartmp2->type=NPVariantType_Bool;
      vartmp2->value.boolValue = true;
      funcs->setproperty(inst, ret->value.objectValue, funcs->getstringidentifier("__CopiedFromR__"), vartmp2);
      //UNPROTECT(1);
    }

  funcs->releaseobject(domwin);
  /*
  funcs->memfree(vartmp3);
  funcs->memfree(vartmp2);
  */  
  funcs->releasevariantvalue(vartmp3);
  funcs->releasevariantvalue(vartmp2);
  UNPROTECT(protstack);
  return true;
  
}
extern "C"{
bool ConvertNPToR(NPVariant *var, NPP inst, NPNetscapeFuncs *funcs, convert_t convRet,  SEXP *_ret) 
//Returns a bool indicating whether the variant passed in is safe to free (ie if we did NOT create new references to it within R)
{
  int canfree = 1;
  if (NPVARIANT_IS_OBJECT(*var) && funcs->hasproperty(inst, var->value.objectValue, funcs->getstringidentifier("_convert")))
    {
      fprintf(stderr, "Javascript argument conversion specified via _convert property.\n");fflush(stderr);
      convRet = GetConvertBehavior(var, inst, funcs);
    }
  switch(convRet)
    {
    case CONV_CUSTOM:
      {
	*_ret = MakeNPRefForR(var, inst, funcs);
      NPVariant convFun;
      funcs->getproperty(inst, var->value.objectValue, funcs->getstringidentifier("_convert"), &convFun);
      canfree = 0;
      NPVariant isRFun;
      bool tmp = funcs->getproperty(inst, convFun.value.objectValue, funcs->getstringidentifier("isRObject"), &isRFun);
      if(tmp && NPVARIANT_IS_BOOLEAN(isRFun) && isRFun.value.boolValue)
	{
	  SEXP call, p, fun ;
	  int wasError;
	  PROTECT(call = allocVector(LANGSXP, 2));
	  PROTECT(p = call);
	  PROTECT(fun = (SEXP) ((RObject *) convFun.value.objectValue)->object);
	  SETCAR(p, fun);
	  p = CDR(p);
	  SETCAR(p, *_ret);
	  *_ret = rQueue.requestRCall(call, R_GlobalEnv, &wasError, inst);
	  UNPROTECT(3);
	 }
      else
	{
	  fprintf(stderr, "Invalid custom JavaScript argument converter specified. Custom converters must be RFunction objects");fflush(stderr);
	  
	}
      }
      break;
    case CONV_REF:
      *_ret = MakeNPRefForR(var, inst, funcs);
      canfree = !checkNPForNA(var, inst, funcs);
      break;
    case CONV_COPY:
      CopyNPVarForR(var, inst, funcs, _ret);
      canfree = true;
      break;

    case CONV_DEFAULT:
      {
	if(!NPVARIANT_IS_OBJECT(*var))
	  {
	    CopyNPVarForR(var, inst, funcs, _ret);
	    canfree = true;
	  } 
	else 
	  {
	    *_ret = MakeNPRefForR(var, inst, funcs);
	    canfree = checkNPForNA(var, inst, funcs);
	  }
      }
      break;
      /*
	    
	  NPObject *inObject = var->value.objectValue;
	  NPVariant npvLength;
	  
	  //check if it is an R object.
	  NPVariant isRObject;
	  bool tmp = funcs->getproperty(inst, inObject, funcs->getstringidentifier("isRObject"), &isRObject);
	  if(tmp && NPVARIANT_IS_BOOLEAN(isRObject) && isRObject.value.boolValue)
	    {
	      fprintf(stderr, "\nRObject (or subclass) detected. Extracting original SEXP\n");fflush(stderr);
	      *_ret = ((RObject *) inObject)->object;
	      canfree = 1;
	    } else if (funcs->hasmethod(inst, inObject, funcs->getstringidentifier("pop")) && convRet == CONV_COPY)
	    {
	      //XXX Taking a shortcut here, assuming only arrays have a pop method. A better while performant way to check this would be good...	
	      funcs->getproperty(inst, inObject, funcs->getstringidentifier("length"), &npvLength);
	      //int len = npvLength.value.intValue;
	      int len = (int) npvLength.value.doubleValue;
	      fprintf(stderr, "\nNPArray of length %d detected. Copying to R list/vector", len);fflush(stderr);
	      canfree = NPArrayToR(var, len, 0, inst, funcs, _ret);
	    }
	  else
	    {
	      tmp = funcs->getproperty(inst, inObject, funcs->getstringidentifier("__CopiedFromR__"), &isRObject);
	      if(convRet == CONV_COPY || (tmp && NPVARIANT_IS_BOOLEAN(isRObject) && isRObject.value.boolValue))
		{
		  //CONV_COPY - force (shallow) copy. Deep copy would drag over the entire JS scope because of parent/child properties
		  *_ret = CopyNPObjForR(var, inst, myNPNFuncs);
		  canfree = 1;
		}
	      else
		//CONV_DEFAULT and object is not a copy of an R object (eg associative array created when copying named vector)
		{
		  //No specific conversion found so we return a reference
		  *_ret = MakeNPRefForR(var);
		  
		  canfree = 0;
		}
		}
	}
	
	break;
      }
      }
      break;
    }
      */
    }
  return canfree;
}
} //extern "C"

static int depth = 0;
bool NPArrayToR(NPVariant *arr, int len, int simplify, NPP inst, NPNetscapeFuncs *funcs, SEXP *_ret)
//Returns bool indicating whether it is safe to free variant.
//SEXP living at *_ret must already be PROTECTed
{
  SEXP tmp, simplifyCall, p;
  NPVariant curValue;
  *_ret = NEW_LIST( len ) ;
  int wasError=0;
  depth ++;
  PROTECT(tmp = R_NilValue);
  int tmpcanfree, canfree;
  canfree = 1;
  for (int i = 0; i < len; i++)
    {
      funcs->getproperty(inst, arr->value.objectValue, funcs->getintidentifier(i), &curValue);
      tmpcanfree = ConvertNPToR(&curValue, inst, funcs, CONV_DEFAULT, &tmp);
      SET_VECTOR_ELT(*_ret, i, tmp);
      if (!tmpcanfree)
	canfree =  0;
    }
  depth--;

  //XXX this if is a relic of the listCall from RFirefox, it's not clear we need it here??
  if( depth > 0 || simplify )
    {
      
      PROTECT( simplifyCall = allocVector( LANGSXP , 3 ) );
      PROTECT( p = simplifyCall );
      SETCAR( p , Rf_install("unlist" ) );
      p = CDR( p );
      SETCAR( p , *_ret );
      p = CDR( p );
      //recursive = FALSE for the unlist call
      SETCAR( p , ScalarLogical( 0 ) );
  
      *_ret = rQueue.requestRCall(simplifyCall , R_GlobalEnv , &wasError, inst);
      UNPROTECT(2);
    }
  
  UNPROTECT(1);
  return( canfree );
}

SEXP MakeNPRefForR(NPVariant *ref, NPP inst, NPNetscapeFuncs *funcs)
{
  SEXP ans;
  PROTECT(ans = R_NilValue);
  if(checkNPForNA(ref, inst, funcs))
    {
      makeNAForR(ref->value.objectValue, inst, funcs, &ans);
      UNPROTECT(1);
    }  
  else if (checkNPForRObj(ref, inst, funcs)) 
    {
      fprintf(stderr, "\nRObject (or subclass) detected. Extracting original SEXP\n");fflush(stderr);
      UNPROTECT(1);
      return ((RObject *) ref->value.objectValue)->object;
    }
  
  else 
    {
      
      SEXP klass, Rptr;
      PROTECT( klass = MAKE_CLASS( "JSValueRef" ) );
      ans = NEW( klass ) ;
      PROTECT( Rptr = R_MakeExternalPtr( ref ,
					 Rf_install( "JSValueRef" ),
					 R_NilValue));
      
  //XXX need to add finalizer!!
      SET_SLOT( ans , Rf_install( "ref" ), Rptr );
  UNPROTECT(3);
    }
  return ans;
}

bool CopyNPVarForR(NPVariant *ref, NPP inst, NPNetscapeFuncs *funcs, SEXP *_ret)
{
  bool canfree = true;
  if(checkNPForNA(ref, inst, funcs))
    {
      makeNAForR(ref->value.objectValue, inst, funcs, _ret);
      return true;
    }
   else if (checkNPForRObj(ref, inst, funcs)) 
    {
      fprintf(stderr, "\nRObject (or subclass) detected. Extracting original SEXP\n");fflush(stderr);
      
      return ((RObject *) ref->value.objectValue)->object;
    }
  switch(ref->type)
    {
    case NPVariantType_Void:
    case NPVariantType_Null:
      break;
    case NPVariantType_Bool:
      *_ret = NEW_LOGICAL(1);
      LOGICAL(*_ret)[0] = ref->value.boolValue;
      break;
    case NPVariantType_Int32:
      *_ret = NEW_INTEGER(1);
      INTEGER(*_ret)[0] = ref->value.intValue;
      break;
    case NPVariantType_Double:
      *_ret = NEW_NUMERIC(1);
      REAL(*_ret)[0] = ref->value.doubleValue;
      break;
    case NPVariantType_String:
      {
	
	NPString str = NPVARIANT_TO_STRING(*ref);
	NPUTF8 *tmpchr = (NPUTF8 *) malloc((str.UTF8Length +1)*sizeof(NPUTF8));
	memcpy(tmpchr,str.UTF8Characters,  str.UTF8Length);
	tmpchr[str.UTF8Length] = '\0';
	const char *strval = (const char *) tmpchr;
	
	*_ret = ScalarString(mkChar(strval));
	
      }	
      break;
    case NPVariantType_Object:
      {
	NPObject *inObject = ref->value.objectValue;
	NPVariant npvLength;
	
	//check if it is an R object.
	NPVariant isRObject;
	bool tmp = funcs->getproperty(inst, inObject, funcs->getstringidentifier("isRObject"), &isRObject);
	if(tmp && NPVARIANT_IS_BOOLEAN(isRObject) && isRObject.value.boolValue)
	  {
	    fprintf(stderr, "\nRObject (or subclass) detected. Extracting original SEXP\n");fflush(stderr);
	    *_ret = ((RObject *) inObject)->object;
	    canfree = 1;
	  } else if (funcs->hasmethod(inst, inObject, funcs->getstringidentifier("pop")))
	  {
	    //XXX Taking a shortcut here, assuming only arrays have a pop method. A better while performant way to check this would be good...	
	    funcs->getproperty(inst, inObject, funcs->getstringidentifier("length"), &npvLength);
	    //int len = npvLength.value.intValue;
	    int len = (int) npvLength.value.doubleValue;
	    fprintf(stderr, "\nNPArray of length %d detected. Copying to R list/vector", len);fflush(stderr);
	    canfree = NPArrayToR(ref, len, 0, inst, funcs, _ret);
	  } else {
	  *_ret = CopyNPObjForR(ref, inst, funcs);
	}
      }
      break;
    default:
      break;
    }
      

}

SEXP CopyNPObjForR(NPVariant *ref, NPP inst, NPNetscapeFuncs *funcs)
//This creates a shallow copy of the JS object as an R list (ie properties are copied, but with the default conversion mechanics). Deep copying is not allowed because it would drag over the entire JS scope, probably multiple times, due to parent/child based properties.
{

  uint32_t idcount = 0;
  NPIdentifier *ids;
  NPObject *obj = ref->value.objectValue;
  NPVariant curprop;
  bool success = myNPNFuncs->enumerate(inst, obj, &ids, &idcount);
  SEXP ans, tmp, names;
  PROTECT(ans = allocVector(VECSXP, idcount));
  PROTECT(names = allocVector(STRSXP, idcount));
  PROTECT(tmp = R_NilValue);
  for(int i =0; i < idcount; i++)
    {
      
      myNPNFuncs->getproperty(inst, obj, ids[i], &curprop);
      ConvertNPToR(&curprop, inst, myNPNFuncs, CONV_DEFAULT, &tmp);
      SET_ELEMENT(ans, i, tmp);
      SET_STRING_ELT(names, i, mkChar(myNPNFuncs->utf8fromidentifier(ids[i])));
    }
  SET_NAMES(ans, names);
  UNPROTECT(3);
  return ans;
}



void MakeRRefForNP(SEXP obj, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret)
{
  SEXP ans = obj;
  int err;
  if(TYPEOF(ans) == PROMSXP)
    {
      fprintf(stderr, "\nPromise detected when creating R Reference"); fflush(stderr);//	  ans = PRVALUE(ans);
      ans = rQueue.requestRCall(ans, R_GlobalEnv, &err, inst);
    }
  //some comments in Rinternals.h vaguely hint at checking for TYPEOF==S4SXP may be insufficient, probably when setOldClass is being used!
  if (IS_S4_OBJECT(ans))
    {
      if(checkForRefClass(ans))
	{

	  RRefClassObject *retobj;
	  retobj = (RRefClassObject *) funcs->createobject(inst, &RRefClassObject::_npclass);
	  funcs->retainobject(retobj);
	  retobj->object = ans;
	  retobj->funcs = funcs;
	  R_PreserveObject(retobj->object);
	  OBJECT_TO_NPVARIANT(retobj, *ret);

	} else {

	RS4Object *retobj;
	retobj = (RS4Object *) funcs->createobject(inst, &RS4Object::_npclass);
	funcs->retainobject(retobj);
	retobj->object = ans;
	retobj->funcs = funcs;
	R_PreserveObject(retobj->object);
	OBJECT_TO_NPVARIANT(retobj, *ret);
      }
    } 
  switch(TYPEOF(ans))
    {
    case CLOSXP:
      // if (TYPEOF(ans) == CLOSXP)
    {
      RFunction *retobj;
      retobj = (RFunction *) funcs->createobject(inst, &RFunction::_npclass);
      funcs->retainobject(retobj);
      retobj->object = ans;
      retobj->funcs = funcs;
      R_PreserveObject(retobj->object);
      OBJECT_TO_NPVARIANT(retobj, *ret);
      break;
    }
    case LGLSXP:
    case INTSXP:
    case REALSXP:
    case STRSXP:
      {
	RVector *retobj;
	retobj = (RVector *) funcs->createobject(inst, &RVector::_npclass);
	funcs->retainobject(retobj);
	retobj->object = ans;
	retobj->funcs = funcs;
	R_PreserveObject(retobj->object);
	//retobj->vecType = TYPEOF(ans);
	OBJECT_TO_NPVARIANT(retobj, *ret);
	
	break;
      }
      
    case VECSXP:
      {
	RList *retobj;
	retobj = (RList *) funcs->createobject(inst, &RList::_npclass);
	funcs->retainobject(retobj);
	retobj->object = ans;
	retobj->funcs = funcs;
	R_PreserveObject(retobj->object);
	OBJECT_TO_NPVARIANT(retobj, *ret);
	
	break;
      }
      
    default:
      {
	RObject *retobj;
	retobj = (RObject *) funcs->createobject(inst, &RObject::_npclass);
	funcs->retainobject(retobj);
	retobj->object = ans;
	retobj->funcs = funcs;
	R_PreserveObject(retobj->object);
	OBJECT_TO_NPVARIANT(retobj, *ret);
      }
    }
  return;
}

//Note: This function allocs mem. need to remember to free it.
const char * NPStringToConstChar(NPString str)
{
     NPUTF8 *tmpchr = (NPUTF8 *) malloc((str.UTF8Length +1)*sizeof(char));
      memcpy(tmpchr,str.UTF8Characters,  str.UTF8Length);
      tmpchr[str.UTF8Length] = '\0';
      const char *conchar = (const char *) tmpchr;
      return conchar;
}

bool CheckSEXPForJSRef(SEXP obj, NPP inst)
{

 if(!IS_S4_OBJECT(obj))
    return false;

  SEXP ans, call, ptr;
  int err = 0;
  PROTECT(ptr = call= allocVector(LANGSXP, 3));
  SETCAR(ptr, Rf_install("is"));
  ptr = CDR(ptr);
  SETCAR(ptr, obj);
  ptr = CDR(ptr);
  SETCAR(ptr, ScalarString(mkChar("JSValueRef")));
  
  PROTECT(ans = rQueue.requestRCall(call, R_GlobalEnv, &err, inst));
  bool ret;
  if (ans == R_UnboundValue || ans == R_NilValue || err)
    ret = 0;
  else
    {
    
      ret = LOGICAL(ans)[0];
      if(ret)
	{fprintf(stderr, "\nR object contains JS reference.\n");fflush(stderr);}
    }
  UNPROTECT(2);
  return ret;

}

bool checkForRefClass(SEXP obj)
{

  SEXP ans, call, ptr;
  PROTECT(call = ptr = allocVector(LANGSXP, 3));
  SETCAR(ptr, Rf_install("is"));
  ptr = CDR(ptr);
  SETCAR(ptr, obj);
  ptr = CDR(ptr);
  SETCAR(ptr, ScalarString(mkChar("envRefClass")));
  int err = 0;
  bool ret = true;
  PROTECT(ans = R_tryEval(call, R_GlobalEnv, &err));
  if(err || !LOGICAL(ans)[0])
    ret = false;
  UNPROTECT(2);
  return ret;
}

convert_t GetConvertBehavior(NPVariant *var, NPP inst, NPNetscapeFuncs *funcs)
{
  NPVariant curprop;
  funcs->getproperty(inst, var->value.objectValue, funcs->getstringidentifier("_convert"), &curprop);
  convert_t ret; 
  switch(curprop.type)
    {
    case NPVariantType_Int32:
      ret = (convert_t) NPVARIANT_TO_INT32(curprop);
      break;
    case NPVariantType_Double:
      ret = (convert_t) NPVARIANT_TO_DOUBLE(curprop);
      break;
    case NPVariantType_String:
      {
      NPString str = var->value.stringValue;
      if(strncmp(str.UTF8Characters, "default", str.UTF8Length))
	ret = CONV_DEFAULT;
      else if (strncmp(str.UTF8Characters, "reference", str.UTF8Length))
	ret = CONV_REF;
      else if (strncmp(str.UTF8Characters, "copy", str.UTF8Length))
	ret = CONV_COPY;
      break;
      }     
    case NPVariantType_Object:
      ret = CONV_CUSTOM;
      break;
		   
    default:
      fprintf(stderr, "Unable to understand _convert property. Default marshalling will be performed.");fflush(stderr);
      ret  = CONV_DEFAULT;
      break;
    }
  return ret;
}

bool checkRForNA(SEXP obj)
{
  fprintf(stderr, "\nIn checkRForNA! R object is:");fflush(stderr);
  Rf_PrintValue(obj);
  bool ret = false;
  if(LENGTH(obj) != 1)
    return ret;
  
  switch(TYPEOF(obj))
    {
    case INTSXP:
    case LGLSXP:
      ret = (INTEGER(obj)[0] == NA_INTEGER) ? true : false;
      break;
    case REALSXP:
      ret = (REAL(obj)[0] == NA_REAL) ? true : false;
      break;
    case STRSXP:
      ret = (STRING_ELT(obj, 0) == NA_STRING) ? true : false;
      break;
    case CHARSXP:
      ret = (obj == NA_STRING) ? true : false;
      break;
    default:
      break;
    }
  return ret;
}

void makeNAForNP(int type, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret)
{
  	RNAValue *retobj;
	retobj = (RNAValue *) funcs->createobject(inst, &RNAValue::_npclass);
	funcs->retainobject(retobj);
	retobj->type = type;
	retobj->funcs = funcs;
	OBJECT_TO_NPVARIANT(retobj, *ret);
}

bool checkNPForNA(NPVariant *var, NPP inst, NPNetscapeFuncs *funcs)
{
  if(!NPVARIANT_IS_OBJECT(*var))
    return false;
  return funcs->hasproperty(inst, var->value.objectValue, funcs->getstringidentifier("_R_NA_Type"));
}

void makeNAForR(NPObject *obj, NPP inst, NPNetscapeFuncs *funcs, SEXP *_ret)
{
  NPVariant typevar;
  funcs->getproperty(inst, obj, funcs->getstringidentifier("_R_NA_Type"), &typevar);
  switch(typevar.value.intValue)
    {
    case INTSXP:
    case LGLSXP:
      *_ret = ScalarInteger(NA_INTEGER);
      break;
    case REALSXP:
      *_ret = ScalarReal(NA_REAL);
      break;
    case STRSXP:
      *_ret = ScalarString(NA_STRING);
      break;
    case CHARSXP:
      *_ret = NA_STRING;
      break;
    default:
      fprintf(stderr, "Unrecognized NA type detected. Returning ScalarInteger(NA_INTEGER)");fflush(stderr);
      *_ret = ScalarInteger(NA_INTEGER);
      break;
    }
}

bool checkNPForRObj(NPVariant *var, NPP inst, NPNetscapeFuncs *funcs)
{
  if(!NPVARIANT_IS_OBJECT(*var))
    return false;
  else
    {

    	  NPObject *inObject = var->value.objectValue;
	  
	  //check if it is an R object.
	  NPVariant isRObject;
	  bool tmp = funcs->getproperty(inst, inObject, funcs->getstringidentifier("isRObject"), &isRObject);
	  return (tmp && NPVARIANT_IS_BOOLEAN(isRObject) && isRObject.value.boolValue);
	 
    }
  /*   {
       fprintf(stderr, "\nRObject (or subclass) detected. Extracting original SEXP\n");fflush(stderr);
       *_ret = ((RObject *) inObject)->object;
	      canfree = 1;
	      
  */
}
