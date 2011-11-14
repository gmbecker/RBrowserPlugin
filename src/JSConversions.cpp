#ifndef USE_RINTERNALS
#define NO_RINTERNALS
#endif

#define USE_RINTERNALS 1
#define JS_THREADSAFE 1

#include "jsapi.h"
#include "Rdefines.h"
#include "R.h"
#include "Rinternals.h"

#include "JSConversions.h"


SEXP 
R_JSRefToR(SEXP con, SEXP jsobj)
{

  JSContext *jscon = (JSContext *) R_ExternalPtrAddr( GET_SLOT( con , Rf_install( "ref" ) ) );

  jsval *jval = (jsval *) R_ExternalPtrAddr( GET_SLOT( jsobj , Rf_install( "ref" ) ) );

  return(JSRefToR(jscon, jval));

}

SEXP JSRefToR(JSContext *jscon, jsval *jsobj)
{
  SEXP ans;
  //intiialize to NULL
  
  PROTECT(ans = R_NilValue);
  JSType typ = JS_TypeOfValue(jscon, *jsobj);
  switch(typ)
    {
    case JSTYPE_VOID:
    case JSTYPE_NULL:
      //stay as NULL
      break;
    case JSTYPE_NUMBER:
      {
	ans = NEW_NUMERIC(1);
	jsdouble dblval;
	JSBool isnum = JSVAL_IS_NUMBER(*jsobj);
	fprintf(stderr, "is Number: %d", isnum);
	JS_ValueToNumber(jscon, *jsobj, &dblval);
	REAL(ans)[0] = dblval;
	break;
      }
    case JSTYPE_STRING:
      {
	ans = NEW_CHARACTER( 1 );
	JSString *jsstrval = JS_ValueToString( jscon , *jsobj);
	
	unsigned int siz = JS_GetStringEncodingLength(jscon, jsstrval);
	char *tmpchr;
	JS_EncodeStringToBuffer(jsstrval, tmpchr, siz);
	const char *strval = (const char *) tmpchr;
	SET_STRING_ELT( ans, 0, mkChar(strval));
	break;
      }
    case JSTYPE_OBJECT:
      {
	JSObject *myobj = JS_NewObject(jscon, NULL, NULL, NULL);
	JSBool res = JS_ValueToObject( jscon , *jsobj , &myobj );
	if (myobj)
	  {
	    JS_AddObjectRoot( jscon , &myobj);
	    if(JS_IsArrayObject( jscon , myobj ) )
	      {
		ans = JSArrayToList(jscon, myobj);
		
	      } else {
	      fprintf(stderr, "Non-Array JSObject detected. Conversion to R object not supported at this time");
	    }
	    JS_RemoveObjectRoot( jscon , &myobj );
	  }
	break;
      }
    default:
      {
	fprintf(stderr, "Objects of type %d are not supported", typ);
	break;
      }
      
    }
  
  UNPROTECT(1);
  return(ans);
}

SEXP JSArrayToList(JSContext *jscon, JSObject *array)
{
  unsigned int len;
  int wasError = 0;

  JS_GetArrayLength( jscon , array , &len );
  
  jsval tmp;
  SEXP ans, simplifyCall, p;
  PROTECT( ans = NEW_LIST( len ) );
  JS_AddValueRoot(jscon, &tmp);
  for(int i = 0; i < len ; i++)
    {
      JS_GetElement( jscon , array , i , &tmp );
      SET_ELEMENT( ans , i , JSRefToR( jscon , &tmp ) );
    }
  JS_RemoveValueRoot(jscon, &tmp);
  //We only want to unlist if it really should be a vector (ie a list of length 5 with 5 numerics of length 1 in it), otherwise the elements represent different arguments for the do.call so the list should be preserved.
  //XXX This check is still not perfect, is there a better way to tell whether to unlist or not?
  int simplify = 1;
  for (int j=0; j < LENGTH( ans ) ; j++)
    {
      if (LENGTH(VECTOR_ELT(ans, j ) ) > 1)
	{
	  simplify = 0;
	  break;
	}
    }
  if(simplify)
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

/*

jsval RToJSRef(JSContext *jscon, SEXP Robj)
{
  
  switch(TYPEOF(Robj))
    {
    case REALSXP:
      {
	
	break;
      }

}

SEXP R_RToJSRef(SEXP RContext, SEXP Robj)
{

  JSContext *jscon = (JSContext *) R_ExternalPtrAddr( GET_SLOT( con , Rf_install( "ref" ) ) );
  jsval jsret = RToJSRef(jscon, Robj);

  return 1;
}

*/
#ifdef NO_RINTERNALS
#undef USE_RINTERNALS
#endif

