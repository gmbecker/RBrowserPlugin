#include "WebR.h"

bool ConvertRToNP(SEXP val, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret, bool convertRes)
{

  //XXXNot sure if this Class check will work...
  SEXP klass;
  PROTECT(klass = MAKE_CLASS("NPVarRef"));
  if (GET_CLASS(val) == klass)
    { 
      //XXX We shouldn't have to copy here, but do we really want to pass in double pointers?
      *ret = *(NPVariant *) R_ExternalPtrAddr(GET_SLOT( val , Rf_install( "ref" ) ) );
	}
  if(!convertRes)
    {
      MakeRRefForNP(val, inst, funcs, ret);
      return true;
    }
  int err = 0;
//XXX If it is a promise we need the actual value. Will this come back to bite us by violating lazy loading?  
  if(TYPEOF(val) == PROMSXP)
    val = R_tryEval(val, R_GlobalEnv, &err);
  fprintf(stderr, "\nIn ConvertRToNP type: %d", TYPEOF(val));fflush(stderr);
  int len = LENGTH(val);

  
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
    case STRSXP:
      if(len > 1)
	RVectorToNP(val, inst, funcs, ret);
      else
	STRINGZ_TO_NPVARIANT(CHAR(STRING_ELT(val, 0)), *ret);
      break;
    case CLOSXP:
      {
	char *buf = (char *) funcs->memalloc((16+sizeof(long int) + 1)*sizeof(char));
	sprintf(buf, "_SEXP:Function_:%ld", val);
	STRINGZ_TO_NPVARIANT((const char *) buf, *ret);
      }
      break;
    default:
      {
	char *buf = (char *) funcs->memalloc((14+sizeof(long int) + 1)*sizeof(char));;
	sprintf(buf, "_SEXP:Object_:%ld", val);
	STRINGZ_TO_NPVARIANT((const char *) buf, *ret);
      }
      break;
    }
  return true;
}

bool RVectorToNP(SEXP vec, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret)
{
  int len = LENGTH(vec);
  fprintf(stderr, "\n R vector of length: %d detected", len); fflush(stderr);
  
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
	  ConvertRToNP(VECTOR_ELT(vec, i), inst, funcs, vartmp2, false);
	  break;
	case STRSXP:
	  ConvertRToNP(STRING_ELT(vec, i), inst, funcs, vartmp2, false);
	  break;
	}
      //  fprintf(stderr, "\nAttempting push call");fflush(stderr);
      //NPN_Invoke(inst, ret->value.objectValue, NPN_GetStringIdentifier("push"), vartmp, 1, vartmp2);
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

bool ConvertNPToR(NPVariant *var, NPP inst, NPNetscapeFuncs *funcs, bool convRet,  SEXP *_ret) 
//Returns a bool indicating whether the variant passed in is safe to free (ie if we did NOT create new references to it within R)
{
  int canfree = 1;
  
  if(!convRet)
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
	    
	    //NPN_GetProperty(inst, inObject, NPN_GetStringIdentifier("length"), &npvLength);
	    funcs->getproperty(inst, inObject, funcs->getstringidentifier("length"), &npvLength);
	    int len = npvLength.value.intValue;
	    if (len > 1)
	      canfree = NPArrayToR(var, len, 0, inst, funcs, _ret);
	    else
	      {
		fprintf(stderr, "\nGeneric NPObject detected. No Conversion found.");
		*_ret = MakeNPRefForR(var);
		canfree = 0;
	      }
	  }
	  break;
	}
    }
  fprintf(stderr, "\n leaving ConvertNPToR. R object:");fflush(stderr);
  Rf_PrintValue(*_ret);
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
      tmpcanfree = ConvertNPToR(&curValue, inst, funcs, false, &tmp);
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
  PROTECT( klass = MAKE_CLASS( "NPVarRef" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( Rptr = R_MakeExternalPtr( ref ,
				    Rf_install( "NPVariantRef" ),
				    R_NilValue));

  //XXX need to add finalizer!!
  SET_SLOT( ans , Rf_install( "ref" ), Rptr );
  UNPROTECT(3);
  return ans;
}

void MakeRRefForNP(SEXP obj, NPP inst, NPNetscapeFuncs *funcs, NPVariant *ret)
{
  if (TYPEOF(obj) == CLOSXP)
    {
      /*
      buf = (char *) funcs->memalloc((16+sizeof(long int) + 1)*sizeof(char));
      sprintf(buf, "_SEXP:Function_:%ld", obj);
      */
      RFunction *retobj;
      retobj = (RFunction *) funcs->createobject(inst, &RFunction::_npclass);
      funcs->retainobject(retobj);
      retobj->object = obj;
      retobj->funcs = funcs;
      R_PreserveObject(retobj->object);
      OBJECT_TO_NPVARIANT(retobj, *ret);
    }
  else
    {
      /*
      buf = (char *) funcs->memalloc((16+sizeof(long int) + 1)*sizeof(char));
      sprintf(buf, "_SEXP:Object_:%ld", obj);
      */
      RObject *retobj;
      retobj = (RObject *) funcs->createobject(inst, &RObject::_npclass);
      funcs->retainobject(retobj);
      retobj->object = obj;
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
