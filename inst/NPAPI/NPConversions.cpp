
bool ConvertRToNP(SEXP val, NPP inst, NPVariant *ret)
{
  int len = LENGTH(val);
  if(len > 1)
    RVectorToNP(val, inst, ret);
  else
    {
      switch(TYPEOF(val))
	{
	case NILSXP:
	  
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
	  STRINGZ_TO_NPVARIANT(CHAR(STRING_ELT(ans, 0)), *ret;);
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
  NPObject *domwin = malloc(sizeof(NPObject));
  NPObject *rettmp = malloc(sizeof(NPObject));
  NPVariant *vartmp = malloc(sizeof(NPVariant));
  NPVariant *vartmp2 = malloc(sizeof(NPVariant));
  NPN_GetValue(inst, NPNVWindowNPObject , domwin);
  NPN_Invoke(inst, domwin, NPN_GetStringIdentifier("Array"), NULL, 0, rettmp);
  
  SEXP el;
  PROTECT(el = R_NilValue);
  for (int i = 0; i< len; i++)
    {
      switch(TYPEOF(vec))
	{
	case REALSXP:
	case INTSXP:
	case LGLSXP:
	case VECSXP:
	  ConvertRToNP(VECTOR_ELT(vec, i), inst, vartmp);
	  break;
	case STRSXP:
	  ConvertRToNP(STRING_ELT(ans, i), inst, vartmp);
	  break;
	}
      NPN_Invoke(inst, rettmp, NPN_GetSTringIdentifier("push"), vartmp, 1, vartmp2);
    }
  OBJECT_TO_NPVARIANT(rettmp, *ret);
  free(domwin);
  free(rettmp);
  free(vartmp);
  free(vartmp2);
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
      LOGICAL(ans)[0] = var -> value.boolValue;
      break;
    case NPVariantType_Int32:
      ans = NEW_INTEGER(1);
      INTEGER(ans)[0] = var->value.intValue;
      break;
    case NPVariantType_Double:
      ans = NEW_NUMERIC(1);
      REAL(ans)[0] = var->value.doubleValue;
      break;
    case NPVariantType_String:
      {


      }
      break;
    case NPVariantType_Object:
      {
	NPObject *inObject = val->value.objectValue;
	NPVariant npvLength;
	NPN_GetProperty(npp, inObject, NPN_GetStringIdentifier("length"), &npvLength);
	int len = npvLenth.value.intValue;
	if (len > 1)
	  ans = NPArrayToR(var, len, 0, inst);
	else
	  {


	  }
      }
      break;
    }
  return ans;
}

static int depth = 0;
SEXP NPArrayToR(NPVariant arr, int len, int simplify, NPP inst)
{
  SEXP ans, simplifyCall, p;
  NPVariant curValue;
  PROTECT( ans = NEW_LIST( len ) );
  depth++;
  for (int i = 0; i < len; i++)
    {
      
      NPN_GetProperty(inst, inObject, NPN_GetIntIdentifier(i), &curValue);
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
