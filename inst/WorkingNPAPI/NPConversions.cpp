#include "BasicPlugin.h"

bool ConvertRToNP(SEXP val, NPP inst, NPVariant *ret)
{
  fprintf(stderr, "\nIn ConvertRToNP. R value:");fflush(stderr);
  Rf_PrintValue(val);
  int len = LENGTH(val);
  if(len > 1)
    RVectorToNP(val, inst, ret);
  else
    {
      switch(TYPEOF(val))
	{
	case NILSXP:
	  break;
	case REALSXP:
	  DOUBLE_TO_NPVARIANT(REAL(val)[0], *ret);
	  break;
	case INTSXP:
	  INT32_TO_NPVARIANT(INTEGER(val)[0], *ret);
	  break;
	case LGLSXP:
	  BOOLEAN_TO_NPVARIANT( (bool) LOGICAL(val)[0], *ret);
	  break;
	case STRSXP:
	  STRINGZ_TO_NPVARIANT(CHAR(STRING_ELT(val, 0)), *ret);
	  break;
	case CLOSXP:
	  {
	    char buf[255];
	    sprintf(buf, "_SEXP:Function_:%ld", val);
	    STRINGZ_TO_NPVARIANT((const char *) buf, *ret);
	  }
	  break;
	default:
	  {
	    char buf[255];
	    sprintf(buf, "_SEXP:Object_:%ld", val);
	    STRINGZ_TO_NPVARIANT((const char *) buf, *ret);
	  }
	  break;
	}
    }
  return true;
}

bool RVectorToNP(SEXP vec, NPP inst, NPVariant *ret)
{
  int len = LENGTH(vec);
  fprintf(stderr, "\n R vector of length: %d detected", len); fflush(stderr);
  
  NPObject *domwin = NULL;
 
  NPVariant *vartmp2 = (NPVariant *) myNPNFuncs->memalloc(sizeof(NPVariant));
  NPVariant *vartmp3 = (NPVariant *) myNPNFuncs->memalloc(sizeof(NPVariant));
  NPError res;
    
  res = myNPNFuncs->getvalue(inst, NPNVWindowNPObject , &domwin);
  //NPObject* window = NULL; NPN_GetValue(aInstance, NPNVWindowNPObject, &window);
  //fprintf(stderr, "\nres: %d domwin._class.Invoke:%lx domwin._class.GetProperty:%lx", res, domwin->_class->invoke, domwin->_class->getProperty);fflush(stderr);
  //documentation isn't entirely clear. Am I getting an NPObject or an NPVariant?

  NPIdentifier arrid = myNPNFuncs->getstringidentifier("Array");
  myNPNFuncs->retainobject(domwin);
  //NPN_Invoke(inst, domwin, NPN_GetStringIdentifier("Array"), NULL, 0, ret);
  //fprintf(stderr, "\nAttempting to create JS array object. domwin address:%lx. instance address:%lx. ret address:%lx", domwin, inst, ret);fflush(stderr);
  
  myNPNFuncs->invoke(inst, domwin, arrid, NULL, 0, ret);
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
	  ConvertRToNP(VECTOR_ELT(vec, i), inst, vartmp2);
	  break;
	case STRSXP:
	  ConvertRToNP(STRING_ELT(vec, i), inst, vartmp2);
	  break;
	}
      //  fprintf(stderr, "\nAttempting push call");fflush(stderr);
      //NPN_Invoke(inst, ret->value.objectValue, NPN_GetStringIdentifier("push"), vartmp, 1, vartmp2);
      myNPNFuncs->invoke(inst, ret->value.objectValue, myNPNFuncs->getstringidentifier("push"), vartmp2, 1, vartmp3);
    }
							  //OBJECT_TO_NPVARIANT(rettmp, *ret);
  /*							  
  NPN_MemFree(domwin);
  NPN_MemFree(vartmp);
  NPN_MemFree(vartmp2);
  */
  myNPNFuncs->releaseobject(domwin);
   myNPNFuncs->memfree(vartmp3);
  myNPNFuncs->memfree(vartmp2);
  return true;
  
}

SEXP ConvertNPToR(NPVariant var, NPP inst) 
{
  SEXP ans;
  PROTECT(ans = R_NilValue);
  switch(var.type)
    {
    case NPVariantType_Void:
    case NPVariantType_Null:
      break;
    case NPVariantType_Bool:
      ans = NEW_LOGICAL(1);
      LOGICAL(ans)[0] = var.value.boolValue;
      break;
    case NPVariantType_Int32:
      ans = NEW_INTEGER(1);
      INTEGER(ans)[0] = var.value.intValue;
      break;
    case NPVariantType_Double:
      ans = NEW_NUMERIC(1);
      REAL(ans)[0] = var.value.doubleValue;
      break;
    case NPVariantType_String:
      {
	ans = NEW_CHARACTER( 1 ) ;
	NPString str = NPVARIANT_TO_STRING(var);
	NPUTF8 *tmpchr = (NPUTF8 *) malloc((str.UTF8Length +1)*sizeof(char));
	memcpy(tmpchr,str.UTF8Characters,  str.UTF8Length);
	tmpchr[str.UTF8Length] = '\0';
	const char *strval = (const char *) tmpchr;
	SET_STRING_ELT(ans, 0, mkChar(strval));
      }
      break;
    case NPVariantType_Object:
      {
	NPObject *inObject = var.value.objectValue;
	NPVariant npvLength;
	
	//NPN_GetProperty(inst, inObject, NPN_GetStringIdentifier("length"), &npvLength);
	myNPNFuncs->getproperty(inst, inObject, myNPNFuncs->getstringidentifier("length"), &npvLength);
	int len = npvLength.value.intValue;
	if (len > 1)
	  ans = NPArrayToR(var, len, 0, inst);
	else
	  {


	  }
      }
      break;
    }
  fprintf(stderr, "\n leaving ConvertNPToR. R object:");fflush(stderr);
  Rf_PrintValue(ans);
  return ans;
}

static int depth = 0;
SEXP NPArrayToR(NPVariant arr, int len, int simplify, NPP inst)
{
  SEXP ans, simplifyCall, p;
  NPVariant curValue;
  PROTECT( ans = NEW_LIST( len ) );
  int wasError=0;
  depth ++;
  for (int i = 0; i < len; i++)
    {
      
      //NPN_GetProperty(inst, arr.value.objectValue, NPN_GetIntIdentifier(i), &curValue);
      myNPNFuncs->getproperty(inst, arr.value.objectValue, myNPNFuncs->getintidentifier(i), &curValue);
      SET_VECTOR_ELT(ans, i, ConvertNPToR(curValue, inst));

    }
  depth--;

  if( depth > 0 || simplify )
    {
      
      PROTECT( simplifyCall = allocVector( LANGSXP , 3 ) );
      PROTECT( p = simplifyCall );
      SETCAR( p , Rf_install("unlist" ) );
      p = CDR( p );
      SETCAR( p , ans );
      p = CDR( p );
      //recursive = FALSE for the unlist call
      SETCAR( p , ScalarLogical( 0 ) );
  
      ans = R_tryEval(simplifyCall , R_GlobalEnv , &wasError);
      UNPROTECT(2);
    }
  
  UNPROTECT(1);
  return( ans );
}

SEXP makeNPVarRef(NPVariant *ref)
{
  SEXP klass, ans, Rptr;
  PROTECT( klass = MAKE_CLASS( "NPVarRef" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( Rptr = R_MakeExternalPtr( ref ,
				    Rf_install( "NPVariantRef" ),
				    R_NilValue));

  SET_SLOT( ans , Rf_install( "ref" ), Rptr );
  UNPROTECT(3);

  return ans;


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
