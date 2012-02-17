#include <Rdefines.h>
#include <stdio.h>
#include <jsapi.h>


SEXP
R_SetPropertyCh(SEXP Rcon, SEXP Robjptr, SEXP Rname, SEXP Rcharval)
{
  JSBool success;
  JSContext *jscon =  (JSContext *) R_ExternalPtrAddr(GET_SLOT( Rcon , Rf_install( "ref" ) ) );
  jsval *valobj = (jsval *)  R_ExternalPtrAddr( GET_SLOT( Robjptr , Rf_install( "ref" ) ) );
  
  const char *name = CHAR(STRING_ELT(Rname, 0));
  JSObject *obj;// = JS_NewObject(jscon, NULL, NULL, NULL);
  success = JS_ValueToObject(jscon, *valobj, &obj);
  //success = JS_AddRoot(jscon, &obj);
  success = JS_AddObjectRoot(jscon, &obj);
  
  const char *charval = CHAR(STRING_ELT(Rcharval, 0));
  JSString *jstrval = JS_NewStringCopyZ(jscon, charval);
  //success = JS_AddRoot(jscon, &jstrval);
  success = JS_AddStringRoot(jscon, &jstrval);
  jsval jvalval = STRING_TO_JSVAL(jstrval);
  //JS_AddRoot(jscon, &jvalval);
  JS_AddValueRoot(jscon, &jvalval);
  
  success = JS_SetProperty(jscon, obj, name, &jvalval);
  if(!success)
    {
      fprintf(stderr, "ERROR: Setting %s property failed", name); fflush(stderr);
    }
  /*
  JS_RemoveRoot(jscon, &jvalval);
  JS_RemoveRoot(jscon, &jstrval);
  JS_RemoveRoot(jscon, &obj);
  */  
  JS_RemoveValueRoot(jscon, &jvalval);
  JS_RemoveStringRoot(jscon, &jstrval);
  JS_RemoveObjectRoot(jscon, &obj);

  return ScalarLogical(success);
}

SEXP
R_GetPropertyCh(SEXP Rcon, SEXP Robjptr, SEXP Rname, SEXP Rret)
{
  JSBool success;
  JSContext *jscon =  (JSContext *) R_ExternalPtrAddr(GET_SLOT( Rcon , Rf_install( "ref" ) ) );
  jsval *valobj = (jsval *)  R_ExternalPtrAddr( GET_SLOT( Robjptr , Rf_install( "ref" ) ) );
  //JS_AddRoot(jscon, valobj);
  JS_AddValueRoot(jscon, valobj);
  
  const char *name = CHAR(STRING_ELT(Rname, 0));
  JSObject *obj = JS_NewObject(jscon, NULL, NULL, NULL);
  success = JS_ValueToObject(jscon, *valobj, &obj);
  //success = JS_AddRoot(jscon, &obj);
  success = JS_AddObjectRoot(jscon, &obj);
  
  jsval *jstoret = (jsval *)  R_ExternalPtrAddr( GET_SLOT( Rret , Rf_install( "ref" ) ) );
  success = JS_GetProperty( jscon , obj , name , jstoret );
    
  JS_RemoveObjectRoot(jscon, &obj);
  JS_RemoveValueRoot(jscon, valobj);
  
  return ScalarLogical(success);
}
  
SEXP
R_Call_JS_Method(SEXP Rcon, SEXP Robjptr, SEXP Rname, SEXP Rargs, SEXP RobjIsVal, SEXP Rout)
{  
  unsigned int n = (unsigned int) LENGTH(Rargs);
  JSBool success;
  JSContext *jscon =  (JSContext *) R_ExternalPtrAddr(GET_SLOT( Rcon , Rf_install( "ref" ) ) );
  int objAdded = 0;
  const char *name = CHAR(STRING_ELT(Rname, 0));
  JSObject *obj = JS_NewObject(jscon, NULL, NULL, NULL); 
  jsval *valobj;
  if (LOGICAL(RobjIsVal)[0])
    {
      valobj = (jsval *) R_ExternalPtrAddr( GET_SLOT( Robjptr , Rf_install( "ref" ) ) );  
      success = JS_ValueToObject(jscon, *valobj, &obj);
      success = JS_AddObjectRoot(jscon, &obj);    
      objAdded = 1;
    } else 
    {
      obj = (JSObject *) R_ExternalPtrAddr( GET_SLOT( Robjptr , Rf_install( "ref" ) ) );  
      //JS_AddRoot(jscon, &obj);
    }
  
  jsval args[n];

  JSString *tmpstr;
  jsval tmpval;
  JS_AddValueRoot(jscon, &tmpval);
  
  //JS_AddRoot(jscon, tmpval2);
  int strrooted = 0;
  SEXP tmp;
  const char *kl;
  PROTECT(tmp = R_NilValue);
  for (int i = 0; i < n; i++)
    {
      tmp = VECTOR_ELT(Rargs, i);

      switch(TYPEOF(tmp)) {

      case LGLSXP:  
	fprintf(stderr, "R->JS: logical %d\n", LOGICAL(tmp)[0]);fflush(stderr);
	args[i] = BOOLEAN_TO_JSVAL(LOGICAL(tmp)[0]);
	break;
      case INTSXP:  
	  fprintf(stderr, "R->JS: integer %d\n", INTEGER(tmp) [0]);fflush(stderr);
	  args[i] = INT_TO_JSVAL( INTEGER( tmp ) [ 0 ] );
	  break;
      case REALSXP:  
          fprintf(stderr, "R->JS: numeric %lf\n", REAL( tmp )[0]);fflush(stderr);
	  JS_NewNumberValue(jscon, (jsdouble) REAL( tmp )[ 0 ] , &tmpval  );
	  args[i] = tmpval;
	  
          break;
      case STRSXP:  
	{
	fprintf(stderr, "R->JS: character %s\n", CHAR(STRING_ELT(tmp, 0)));fflush(stderr);
	tmpstr = JS_NewStringCopyZ( jscon , CHAR( STRING_ELT( tmp , 0) ) );
	  
	  if (!strrooted)
	    {
	      JS_AddStringRoot(jscon, &tmpstr);
	      strrooted = 1;
	    }
	  
	  //JS_AddRoot(jscon, &tmpstr);
	  args[i]  = STRING_TO_JSVAL(tmpstr);
	}
	  break;
      case CLOSXP:
	{
	  char buf[255];
	  sprintf(buf, "_SEXP:Function_:%ld", (long int) tmp);
	  fprintf(stderr, "R->JS: No conversion found. Object encoded in string as %s.", buf);fflush(stderr);
	  tmpstr = JS_NewStringCopyZ( jscon , buf ) ;
	  
	  if (!strrooted)
	    {
	      JS_AddStringRoot(jscon, &tmpstr);
	      strrooted = 1;
	    }
	  
	  //JS_AddRoot(jscon, &tmpstr);
	  args[i]  = STRING_TO_JSVAL(tmpstr);
	  
	}
	break;
      default:
	{
	  kl = CHAR(asChar(getAttrib(tmp, R_ClassSymbol)));
	  if ( strstr(kl, "jsvalRef"))
	    args[i] = *((jsval *) R_ExternalPtrAddr( GET_SLOT( tmp , Rf_install( "ref" ) ) )); 
	  else
	    {
	      
	  char buf[255];
	  sprintf(buf, "_SEXP:Object:%ld", (long int) tmp);
	  fprintf(stderr, "R->JS: No conversion found. Object encoded in string as %s.", buf);fflush(stderr);
	  tmpstr = JS_NewStringCopyZ( jscon , buf ) ;
	  
	if (!strrooted)
	    {
	      JS_AddStringRoot(jscon, &tmpstr);
	      strrooted = 1;
	    }
	  
	  //JS_AddRoot(jscon, &tmpstr);
	args[i]  = STRING_TO_JSVAL(tmpstr);
	    }
	}
	break;
      }
      /*
      curtype = INTEGER(Rtypes)[i];
      switch ( curtype )
	{
	case 1:
	  //argument is R character value
	  tmpstr = JS_NewStringCopyZ( jscon , CHAR( STRING_ELT( VECTOR_ELT( Rargs , i ) , 0) ) );
	  
	  if (!strrooted)
	    {
	      JS_AddStringRoot(jscon, &tmpstr);
	      strrooted = 1;
	    }
	  
	  //JS_AddRoot(jscon, &tmpstr);
	  args[i]  = STRING_TO_JSVAL(tmpstr);
	  //JS_RemoveRoot(jscon, &tmpstr);
	  break;
	case 2: 
	  //argument is R integer
	  //in some versions of spidermonkey there are issues with very large integers not fitting into jsvals.
	  args[i] = INT_TO_JSVAL( INTEGER( VECTOR_ELT( Rargs , i ) ) [ 0 ] );
	  break;
	case 3:
	  //argument is R numeric
	  JS_NewNumberValue(jscon, (jsdouble) REAL(VECTOR_ELT(Rargs , i ) )[ 0 ] , &tmpval  );
	  args[i] = tmpval;
	  break;
	case 4:
	  //argument is R jsvalRef
	  tmpval2 = (jsval *)  R_ExternalPtrAddr( GET_SLOT( VECTOR_ELT( Rargs , i ) , Rf_install( "ref" ) ) );  
	  args[i] = *tmpval2;
	  break;
	default:
	  
	}
      */
      
	  
     JS_AddValueRoot(jscon, &(args[i]));
    }
  jsval *retval = (jsval *) R_ExternalPtrAddr( GET_SLOT( Rout , Rf_install( "ref" ) ) );  
  success = JS_CallFunctionName(jscon, obj, name, n, args, retval);
  if(!success)
    {
      fprintf(stderr, "ERROR: Call to JavaScript function %s failed.\n", name); fflush(stderr);
    }
  //addrooted in R code
  //JS_AddRoot(jscon, retval);
  if(objAdded)
    JS_RemoveObjectRoot(jscon, &obj);
  if(strrooted)
    JS_RemoveStringRoot(jscon, &tmpstr);
  JS_RemoveValueRoot(jscon, &tmpval);
  //JS_RemoveRoot(jscon, tmpval2);
  for(int j = 0; j < n; j++)
    JS_RemoveValueRoot(jscon, &(args[j]));
 
 UNPROTECT(1);
 return ScalarLogical(success);
}
  

  
  
