#include "WebR.h"


bool ConvertRToNP(SEXP val, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret, convert_t convRes)
{

//If it is a promise we need the actual value.  
  int err = 0;
  if(TYPEOF(val) == PROMSXP)
    val = R_tryEval(val, R_GlobalEnv, &err);
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
  if(convRes == CONV_REF)
    {
      MakeRRefForNP(val, inst, funcs, ret);
      return true;
    }
  
  //Default marshalling behavior
  switch(TYPEOF(val))
    {
    case NILSXP:
      break;
    case REALSXP:
    case INTSXP:
    case LGLSXP:
    case STRSXP:
    case CHARSXP:
    case VECSXP:
      MakeCopyRToNP(val, inst, funcs, ret);
      break;
    case CLOSXP:
    case S4SXP:
    default:
      MakeRRefForNP(val, inst, funcs, ret);
      break;
    }
  return true;
}

void MakeCopyRToNP(SEXP val, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret)
{

  if(IS_S4_OBJECT(val))
    CopyS4ToNP(val, inst, funcs, ret);
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


  //PROTECT(klass = MAKE_CLASS("NPVariantRef"));
//XXX If it is a promise we need the actual value. Will this come back to bite us by violating lazy loading?  
  int err = 0;
  if(TYPEOF(val) == PROMSXP)
    val = R_tryEval(val, R_GlobalEnv, &err);
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


  //fprintf(stderr, "\nIn ConvertRoNP type: %d", TYPEOF(val));fflush(stderr);
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
    
  res = funcs->getvalue(inst, NPNVWindowNPObject , &domwin);
 
  NPIdentifier arrid = funcs->getstringidentifier("Array");
  funcs->retainobject(domwin);
   
  funcs->invoke(inst, domwin, arrid, NULL, 0, ret);
  //fprintf(stderr, "\nJS array object created.");fflush(stderr);
  SEXP el;
  PROTECT(el = R_NilValue);
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
	  vartmp2->type=NPVariantType_Int32;
	  vartmp2->value.intValue = LOGICAL(vec)[i];
	  break;
	case VECSXP:
	  ConvertRToNP(VECTOR_ELT(vec, i), inst, funcs, vartmp2, CONV_DEFAULT);
	  break;
	case STRSXP:
	  ConvertRToNP(STRING_ELT(vec, i), inst, funcs, vartmp2, CONV_DEFAULT);
	  break;
	}

      funcs->invoke(inst, ret->value.objectValue, funcs->getstringidentifier("push"), vartmp2, 1, vartmp3);
    }
  fprintf(stderr, "\nConversion loop complete.");fflush(stderr);

  funcs->releaseobject(domwin);
  /*
  funcs->memfree(vartmp3);
  funcs->memfree(vartmp2);
  */  
  funcs->releasevariantvalue(vartmp3);
  funcs->releasevariantvalue(vartmp2);
  return true;
  
}

bool ConvertNPToR(NPVariant *var, NPP inst, NPNetscapeFuncs *funcs, convert_t convRet,  SEXP *_ret) 
//Returns a bool indicating whether the variant passed in is safe to free (ie if we did NOT create new references to it within R)
{
  int canfree = 1;
  
  if(convRet == CONV_REF)
    {
      *_ret = MakeNPRefForR(var);
      canfree = 0;
    }
  else
    {
      switch(var->type)
	{
	case NPVariantType_Void:
	case NPVariantType_Null:
	  break;
	case NPVariantType_Bool:
	  *_ret = NEW_LOGICAL(1);
	  LOGICAL(*_ret)[0] = var->value.boolValue;
	  break;
	case NPVariantType_Int32:
	  *_ret = NEW_INTEGER(1);
	  INTEGER(*_ret)[0] = var->value.intValue;
	  break;
	case NPVariantType_Double:
	  *_ret = NEW_NUMERIC(1);
	  REAL(*_ret)[0] = var->value.doubleValue;
	  break;
	case NPVariantType_String:
	  {
	    
	    NPString str = NPVARIANT_TO_STRING(*var);
	    NPUTF8 *tmpchr = (NPUTF8 *) malloc((str.UTF8Length +1)*sizeof(NPUTF8));
	    memcpy(tmpchr,str.UTF8Characters,  str.UTF8Length);
	    tmpchr[str.UTF8Length] = '\0';
	    const char *strval = (const char *) tmpchr;
	    
	    *_ret = ScalarString(mkChar(strval));

	  }	
	  break;
	case NPVariantType_Object:
	  {
	    NPObject *inObject = var->value.objectValue;
	    NPVariant npvLength;
	    
	    //XXX Taking a shortcut here, assuming only arrays have a pop method. A better while performant way to check this would be good...
	    if (funcs->hasmethod(inst, inObject, funcs->getstringidentifier("pop")))
	      {
		
		funcs->getproperty(inst, inObject, funcs->getstringidentifier("length"), &npvLength);
		//int len = npvLength.value.intValue;
		int len = (int) npvLength.value.doubleValue;
		fprintf(stderr, "\nNPArray of length %d detected. Converting to R list/vector", len);fflush(stderr);
		canfree = NPArrayToR(var, len, 0, inst, funcs, _ret);
	      }
	    else
	      {
		//check if it is an R object.
		NPVariant isRObject;
		bool tmp = funcs->getproperty(inst, inObject, funcs->getstringidentifier("isRObject"), &isRObject);
		if(tmp && NPVARIANT_IS_BOOLEAN(isRObject) && isRObject.value.boolValue)
		  {
		    fprintf(stderr, "\nRObject detected. Extracting original SEXP\n");fflush(stderr);
		    *_ret = ((RObject *) inObject)->object;
		    canfree = 1;
		  } else
		  {							   
		    if(convRet == CONV_DEFAULT)
		      {
			//No specific conversion found so we return a reference
			*_ret = MakeNPRefForR(var);
		    
			canfree = 0;
		      } else {
		      //CONV_COPY - force (shallow) copy. Deep copy would drag over the entire JS scope because of parent/child properties
		      *_ret = CopyNPObjForR(var, inst, funcs);
		      canfree = 1;
		    }
		  }
	      }
	  }
	  break;
	}
    }
  return canfree;
}

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
  
      *_ret = R_tryEval(simplifyCall , R_GlobalEnv , &wasError);
      UNPROTECT(2);
    }
  
  UNPROTECT(1);
  return( canfree );
}

SEXP MakeNPRefForR(NPVariant *ref)
{
  SEXP klass, ans, Rptr;
  PROTECT( klass = MAKE_CLASS( "NPVariantRef" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( Rptr = R_MakeExternalPtr( ref ,
				    Rf_install( "NPVariantRef" ),
				    R_NilValue));

  //XXX need to add finalizer!!
  SET_SLOT( ans , Rf_install( "ref" ), Rptr );
  UNPROTECT(3);
  return ans;
}


SEXP CopyNPObjForR(NPVariant *ref, NPP inst, NPNetscapeFuncs *funcs)
//This creates a shallow copy of the JS object as an R list (ie properties are copied, but with the default conversion mechanics). Deep copying is not allowed because it would drag over the entire JS scope, probably multiple times, due to parent/child based properties.
{

  uint32_t idcount = 0;
  NPIdentifier *ids;
  NPObject *obj = ref->value.objectValue;
  NPVariant curprop;
  bool success = funcs->enumerate(inst, obj, &ids, &idcount);
  SEXP ans, tmp, names;
  PROTECT(ans = allocVector(VECSXP, idcount));
  PROTECT(names = allocVector(STRSXP, idcount));
  PROTECT(tmp = R_NilValue);
  for(int i =0; i < idcount; i++)
    {
      funcs->getproperty(inst, obj, ids[i], &curprop);
      ConvertNPToR(&curprop, inst, funcs, CONV_DEFAULT, &tmp);
      SET_ELEMENT(ans, i, tmp);
      SET_STRING_ELT(names, i, mkChar(funcs->utf8fromidentifier(ids[i])));
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
      ans = R_tryEval(ans, R_GlobalEnv, &err);
    }
  if (TYPEOF(ans) == CLOSXP)
    {
      RFunction *retobj;
      retobj = (RFunction *) funcs->createobject(inst, &RFunction::_npclass);
      funcs->retainobject(retobj);
      retobj->object = ans;
      retobj->funcs = funcs;
      R_PreserveObject(retobj->object);
      OBJECT_TO_NPVARIANT(retobj, *ret);
    }
  else
    {

      RObject *retobj;
      retobj = (RObject *) funcs->createobject(inst, &RObject::_npclass);
      funcs->retainobject(retobj);
      retobj->object = ans;
      retobj->funcs = funcs;
      R_PreserveObject(retobj->object);
      OBJECT_TO_NPVARIANT(retobj, *ret);
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

  SEXP ans, call, ptr;
  int err;
  PROTECT(ptr = call= allocVector(LANGSXP, 3));
  SETCAR(ptr, Rf_install("is"));
  ptr = CDR(ptr);
  SETCAR(ptr, obj);
  ptr = CDR(ptr);
  SETCAR(ptr, ScalarString(mkChar("NPVariantRef")));
  
  PROTECT(ans = R_tryEval(call, R_GlobalEnv, &err));
  bool ret = LOGICAL(ans)[0];
  if(ret)
    {fprintf(stderr, "\nR object contains JS reference.\n");fflush(stderr);}
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
  
  return ret;
}
