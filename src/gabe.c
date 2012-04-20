#ifdef BUILD_RFIREFOX

#ifndef USE_RINTERNALS
#define NO_RINTERNALS
#endif

#define USE_RINTERNALS 1
#define JS_THREADSAFE 1

#include "jsapi.h"
#include "Rdefines.h"
#include "R.h"
#include "Rinternals.h"

SEXP
R_STRING_TO_JSVAL(SEXP con, SEXP string, SEXP addRoot)
{
  jsval val;
  JSContext *jscon = (JSContext *) R_ExternalPtrAddr( GET_SLOT( con , Rf_install( "ref" ) ) );

  val = STRING_TO_JSVAL((JSString *) R_ExternalPtrAddr(string));
  
  
  if (LOGICAL(addRoot)[0])
    JS_AddValueRoot(jscon, &val);
  
  SEXP klass, ans, Rptr;
  PROTECT( klass = MAKE_CLASS( "jsvalRef" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( Rptr = R_MakeExternalPtr( &val,
				    Rf_install( "jsval" ),
				    R_NilValue));

  SET_SLOT( ans , Rf_install( "ref" ), Rptr );
  UNPROTECT(3);
  return ans;
}


SEXP
R_JS_ConvertValue(SEXP con, SEXP Rvalptr, SEXP type, SEXP _retv)
{
  fprintf(stderr, "sizeof(jsval): %zu\n", sizeof(jsval)); fflush(stderr); 
  JSContext *jscon =  (JSContext *) R_ExternalPtrAddr(GET_SLOT( con , Rf_install( "ref" ) ) );
  SEXP tmp; 
  PROTECT(tmp = GET_SLOT(Rvalptr, Rf_install("ref")));
  Rf_PrintValue(tmp);
  
  UNPROTECT(1);
  jsval *val = (jsval *)  R_ExternalPtrAddr( GET_SLOT( Rvalptr , Rf_install( "ref" ) ) );  
  jsval *ret = (jsval *) R_ExternalPtrAddr( GET_SLOT( _retv , Rf_install( "ref" ) ) );

  fprintf(stderr, "sizeof val: %zu\n", sizeof(*val)); fflush(stderr);

  JSType ty = (JSType) INTEGER(type)[0];
  
  JSBool success = JS_ConvertValue(jscon, *val, ty, ret);

 if (!success)
    PROBLEM "Failure in JS_ConvertValue." ERROR;

  return( _retv );
}

SEXP
R_JS_ValueToObject(SEXP con, SEXP valptr, SEXP _retobj)
{
  JSContext *jscon = (JSContext *) R_ExternalPtrAddr(GET_SLOT( con , Rf_install( "ref" ) ) );
  jsval *val;
  JSObject *ret;
  val = (jsval *) R_ExternalPtrAddr( GET_SLOT( valptr , Rf_install( "ref" ) ) );
  //ret = (JSObject *) R_ExternalPtrAddr( GET_SLOT( _retobj , Rf_install( "ref" ) ) );
ret = (JSObject *) R_ExternalPtrAddr( GET_SLOT( _retobj , Rf_install( "ref" ) ) );

  JSBool success = JS_ValueToObject(jscon, *val, &ret);
  JS_AddObjectRoot(jscon, &ret);
  JSClass *class = JS_GET_CLASS(jscon, ret);
  fprintf(stderr, "resulting object class: %s\n", class->name);fflush(stderr);
  if (!success)
    PROBLEM "Failure in JS_ValueToObject." ERROR;
  
  return(_retobj);
}

SEXP
R_JS_ValueToJSString(SEXP con, SEXP valptr)
{
  JSContext *jscon = (JSContext *) R_ExternalPtrAddr(GET_SLOT( con , Rf_install( "ref" ) ) );
  jsval *val;

  val = (jsval *) R_ExternalPtrAddr( GET_SLOT( valptr , Rf_install( "ref" ) ) );
  //ret = (JSObject *) R_ExternalPtrAddr( GET_SLOT( _retobj , Rf_install( "ref" () ) );


  JSString *ret = JS_ValueToString(jscon, *val);

  SEXP klass, ans, Rptr;
  PROTECT( klass = MAKE_CLASS( "JSStringRef" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( Rptr = R_MakeExternalPtr( ret,
				    Rf_install( "JSString" ),
				    R_NilValue));

  SET_SLOT( ans , Rf_install( "ref" ), Rptr );
  UNPROTECT(3);
  
  return(ans);
}
 
SEXP
/*
type 
0 = jsval *
1 = JSObject *
2 = JSString *
*/ 
R_JS_RemoveRoot(SEXP con, SEXP Rptr, SEXP type)
 {
   
   void *thing = R_ExternalPtrAddr( GET_SLOT( Rptr , Rf_install( "ref" ) ) );
   JSContext *jscon = (JSContext *) R_ExternalPtrAddr(GET_SLOT( con , Rf_install( "ref" ) ) );
   if(INTEGER(type)[0] == 0)
     {
       jsval *val = (jsval *) thing;
     JS_RemoveValueRoot(jscon, val);
     }
   else if (INTEGER(type)[0] == 1) 
     {
       JSObject *obj = (JSObject *) thing;
       JS_RemoveObjectRoot(jscon, &obj);
     }
   else
     {
       JSString *str = (JSString *) thing;
       JS_RemoveStringRoot(jscon, &str);
     }
   return ScalarLogical(TRUE);
   
 }

SEXP 
/*
type 
0 = jsval *
1 = JSObject *
2 = JSString *
*/ 

R_JS_AddRoot(SEXP con, SEXP Rptr, SEXP type)
{
  void *thing = R_ExternalPtrAddr( GET_SLOT( Rptr , Rf_install( "ref" ) ) );
  JSContext *jscon = (JSContext *) R_ExternalPtrAddr(GET_SLOT( con , Rf_install( "ref" ) ) );
  int typ = INTEGER(type)[0];
  if(typ == 0)
     {
       jsval *val = (jsval *) thing;
       JS_AddValueRoot(jscon, val);
     }
   else if (typ== 1) 
     {
       JSObject *obj = (JSObject *) thing;
       JS_AddObjectRoot(jscon, &obj);
     }
   else
     {
       JSString *str = (JSString *) thing;
       JS_AddStringRoot(jscon, &str);
     }

   return ScalarLogical(TRUE);
}

SEXP 
R_Do_Finalizer(SEXP con, SEXP ptr, SEXP type)
{
  void *thing = R_ExternalPtrAddr(ptr);
  JSContext *jscon = (JSContext *) R_ExternalPtrAddr(GET_SLOT( con , Rf_install( "ref" ) ) );
  int typ = INTEGER(type)[0];
  
  if(typ == 0)
    {
      jsval *val = (jsval *) thing;
      JS_RemoveValueRoot(jscon, val);
    }
  else if (typ == 1)
    {
      JSObject *obj = (JSObject *) thing;
      JS_RemoveObjectRoot(jscon, &obj);
    }
  else
    {
      JSString *str = (JSString *) thing;
      JS_RemoveStringRoot(jscon, &str);
    }
  return ScalarLogical(1);
}

SEXP
R_InspectJSClass(SEXP con, SEXP Rclass)
{
  JSClass *class = (JSClass *) R_ExternalPtrAddr( GET_SLOT( Rclass , Rf_install( "ref" ) ) );
 
  fprintf(stderr, "class name: %s\n", class->name); fflush(stderr);

  return ScalarLogical(TRUE);

}

SEXP
R_JS_GetClass(SEXP con, SEXP Robj)
{

  JSObject *obj = (JSObject *) R_ExternalPtrAddr( GET_SLOT( Robj , Rf_install( "ref" ) ) );
  JSContext *jscon = (JSContext *) R_ExternalPtrAddr(GET_SLOT( con , Rf_install( "ref" ) ) );

  JSClass *class = JS_GET_CLASS(jscon, obj);

  SEXP klass, ans, Rptr;
  PROTECT( klass = MAKE_CLASS( "JSClassRef" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( Rptr = R_MakeExternalPtr( class,
				    Rf_install( "JSClass" ),
				    R_NilValue));

  SET_SLOT( ans , Rf_install( "ref" ), Rptr );
  UNPROTECT(3);
  
  return ans;
}

SEXP
R_JS_CallFunctionName(SEXP con, SEXP Rparent, SEXP Rname,
		      SEXP Rargc, SEXP Rargv, SEXP Rret)
{
  JSObject *par = (JSObject *) R_ExternalPtrAddr( GET_SLOT( Rparent , Rf_install( "ref" ) ) );
  JSContext *jscon = (JSContext *) R_ExternalPtrAddr(GET_SLOT( con , Rf_install( "ref" ) ) );
  const char * name = CHAR(STRING_ELT(Rname, 0));
  int argc = INTEGER(Rargc)[0];

  jsval *ret = (jsval *) R_ExternalPtrAddr( GET_SLOT( Rret , Rf_install( "ref" ) ) );
  jsval *argv = (jsval *) R_ExternalPtrAddr( GET_SLOT( Rargv , Rf_install( "ref" ) ) );
   
  //  JSBool success =  JS_CallFunctionName(jscon, par, name,					argc, argv, &ret);
  JSBool success =  JS_CallFunctionName(jscon, par, name,					argc, argv, ret);

  if(!success)
    {
      fprintf(stderr, "JS_CallFunctionName failed.\n");fflush(stderr);
    }
  return ScalarLogical(success);
}


SEXP R_JS_GetGlobalObject(SEXP Rcon)
{

  JSContext *jscon = (JSContext *) R_ExternalPtrAddr(GET_SLOT( Rcon , Rf_install( "ref" ) ) );

  JSObject *toret = JS_GetGlobalObject(jscon);

  SEXP klass, ans, Rptr;
  PROTECT( klass = MAKE_CLASS( "JSObjectRef" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( Rptr = R_MakeExternalPtr( toret,
				    Rf_install( "JSObject" ),
				    R_NilValue));

  SET_SLOT( ans , Rf_install( "ref" ), Rptr );
  UNPROTECT(3);
  
  return ans;
}

SEXP R_jsVal(SEXP Rcon)
{
  JSContext *jscon = (JSContext *) R_ExternalPtrAddr(GET_SLOT( Rcon , Rf_install( "ref" ) ) );
  jsval *ret = JS_malloc(jscon, sizeof(jsval));

  SEXP klass, ans, Rptr;
  PROTECT( klass = MAKE_CLASS( "jsvalRef" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( Rptr = R_MakeExternalPtr( ret ,
				    Rf_install( "jsvalRef" ),
				    R_NilValue));

  SET_SLOT( ans , Rf_install( "ref" ), Rptr );
  UNPROTECT(3);

  return ans;

}

SEXP R_EvaluateScript(SEXP Rcon, SEXP Rparent, SEXP Rscript, SEXP Rnumchar, SEXP Rtitle, SEXP Rlinenum, SEXP Rret)
{

  JSContext *jscon = (JSContext *) R_ExternalPtrAddr(GET_SLOT( Rcon , Rf_install( "ref" ) ) );

  JSObject *jsparent = (JSObject *) R_ExternalPtrAddr(GET_SLOT( Rparent , Rf_install( "ref" ) ) );

  jsval *jsret = (jsval *) R_ExternalPtrAddr(GET_SLOT( Rret , Rf_install( "ref" ) ) );

  const char *script = CHAR(STRING_ELT(Rscript, 0));
  const char *title = CHAR(STRING_ELT(Rtitle, 0));
  int numchar = INTEGER(Rnumchar)[0];
  int linenum = INTEGER(Rlinenum)[0];

  JSBool success = JS_EvaluateScript(jscon, jsparent, script, numchar, title, linenum, jsret);

  if(!success)
    {
      fprintf(stderr, "Evaluation of Script failed.");fflush(stderr);
    }
  
  return ScalarLogical(1);

}

#ifdef NO_RINTERNALS
#undef USE_RINTERNALS
#endif

#endif //BUILD_RFIREFOX
