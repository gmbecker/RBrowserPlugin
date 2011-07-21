#include <Rdefines.h>
#include <stdio.h>
#include <jsapi.h>


SEXP
R_SetPropertyCh(SEXP Rcon, SEXP Robjptr, SEXP Rname, SEXP Rcharval)
{
  JSBool success;
  JSContext *jscon =  (JSContext *) R_ExternalPtrAddr(GET_SLOT( Rcon , Rf_install( "ref" ) ) );
  jsval *valobj = (jsval *)  R_ExternalPtrAddr( GET_SLOT( Robjptr , Rf_install( "ref" ) ) );
  //JS_AddRoot(jscon, valobj);  
  
  const char *name = CHAR(STRING_ELT(Rname, 0));
  JSObject *obj;// = JS_NewObject(jscon, NULL, NULL, NULL);
  success = JS_ValueToObject(jscon, *valobj, &obj);
  success = JS_AddRoot(jscon, &obj);
  
  const char *charval = CHAR(STRING_ELT(Rcharval, 0));
  JSString *jstrval = JS_NewStringCopyZ(jscon, charval);
  success = JS_AddRoot(jscon, &jstrval);
  jsval jvalval = STRING_TO_JSVAL(jstrval);
  JS_AddRoot(jscon, &jvalval);
  
  success = JS_SetProperty(jscon, obj, name, &jvalval);
 
  JS_RemoveRoot(jscon, &jvalval);
  JS_RemoveRoot(jscon, &jstrval);
  JS_RemoveRoot(jscon, &obj);
  //JS_RemoveRoot(jscon, valobj);

  return ScalarLogical(success);
}

SEXP
R_GetPropertyCh(SEXP Rcon, SEXP Robjptr, SEXP Rname, SEXP Rret)
{
  JSBool success;
  JSContext *jscon =  (JSContext *) R_ExternalPtrAddr(GET_SLOT( Rcon , Rf_install( "ref" ) ) );
  jsval *valobj = (jsval *)  R_ExternalPtrAddr( GET_SLOT( Robjptr , Rf_install( "ref" ) ) );
  JS_AddRoot(jscon, valobj);
  
  const char *name = CHAR(STRING_ELT(Rname, 0));
  JSObject *obj = JS_NewObject(jscon, NULL, NULL, NULL);
  success = JS_ValueToObject(jscon, *valobj, &obj);
  success = JS_AddRoot(jscon, &obj);
  
  jsval *jstoret = (jsval *)  R_ExternalPtrAddr( GET_SLOT( Rret , Rf_install( "ref" ) ) );
  success = JS_GetProperty( jscon , obj , name , jstoret );
  //already addrooted with finalizer on R side.
  //JS_AddRoot( jscon , jstoret );
  
  JS_RemoveRoot(jscon, &obj);
  JS_RemoveRoot(jscon, valobj);
  
  return ScalarLogical(success);
}
  
SEXP
R_Call_JS_Method(SEXP Rcon, SEXP Robjptr, SEXP Rname, SEXP Rargs, SEXP Rtypes, SEXP RobjIsVal, SEXP Rout)
{  
  unsigned int n = (unsigned int) LENGTH(Rargs);
  JSBool success;
  JSContext *jscon =  (JSContext *) R_ExternalPtrAddr(GET_SLOT( Rcon , Rf_install( "ref" ) ) );
  int objAdded = 0;
  const char *name = CHAR(STRING_ELT(Rname, 0));
  JSObject *obj; // = JS_NewObject(jscon, NULL, NULL, NULL);
  //JS_AddRoot(jscon, &obj);
  jsval *valobj;
  if (LOGICAL(RobjIsVal)[0])
    {
      valobj = (jsval *) R_ExternalPtrAddr( GET_SLOT( Robjptr , Rf_install( "ref" ) ) );  
      success = JS_ValueToObject(jscon, *valobj, &obj);
      success = JS_AddRoot(jscon, &obj);    
      objAdded = 1;
    } else 
    {
      obj = (JSObject *) R_ExternalPtrAddr( GET_SLOT( Robjptr , Rf_install( "ref" ) ) );  
      //JS_AddRoot(jscon, &obj);
    }
  
  jsval args[n];

  JSString *tmpstr;
  jsval tmpval;
  JS_AddRoot(jscon, &tmpval);
  
  jsval *tmpval2;
  //JS_AddRoot(jscon, tmpval2);
  int curtype;
  int strrooted = 0;
  
  for (int i = 0; i < n; i++)
    {
      curtype = INTEGER(Rtypes)[i];
      switch ( curtype )
	{
	case 1:
	  //argument is R character value
	  tmpstr = JS_NewStringCopyZ( jscon , CHAR( STRING_ELT( VECTOR_ELT( Rargs , i ) , 0) ) );
	  
	  if (!strrooted)
	    {
	      JS_AddRoot(jscon, &tmpstr);
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
	}
	  
     JS_AddRoot(jscon, &(args[i]));
    }
  jsval *retval = (jsval *) R_ExternalPtrAddr( GET_SLOT( Rout , Rf_install( "ref" ) ) );  
  success = JS_CallFunctionName(jscon, obj, name, n, args, retval);
  //addrooted in R code
  //JS_AddRoot(jscon, retval);
  if(objAdded)
    JS_RemoveRoot(jscon, &obj);
  if(strrooted)
    JS_RemoveRoot(jscon, &tmpstr);
  JS_RemoveRoot(jscon, &tmpval);
  //JS_RemoveRoot(jscon, tmpval2);
for(int j = 0; j < n; j++)
    JS_RemoveRoot(jscon, &(args[j]));
  
  return ScalarLogical(success);
}
  

  
  
