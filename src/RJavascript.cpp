#include "xpcom-config.h"
#include "mozilla-config.h"
#include "nsEmbedAPI.h"
/*
#include "nspr/prtypes.h"
#include "xpcom/nscore.h"
#include "nsXPCOM.h"
#include "xulapp/nsXULAppAPI.h" 
#if 1
#include "nsEmbedCID.h" 
//#include "sdk/prtypes.h" 

#include "embed_base/nsEmbedAPI.h"
#include "nsXPCOMGlue.h" 
#include "nsDebug.h" 
#include "Gabe.h"
#include "widget/nsIBaseWindow.h" 

#include "nsIWebBrowser.h" 
#include "docshell/nsIWebNavigation.h" 
#include "nsEmbedString.h" 
#include "xulapp/nsXULAppAPI.h" 
#include "nsComponentManagerUtils.h"
#endif
*/
//#include "necko/nsICookieManager2.h"
#include "Moz191Compat.h"
#include "nsXPCOM.h"
#include "nsEmbedCID.h" 
#include "nsXPCOMGlue.h"
#include "nsDebug.h"
#include "Moz191Compat.h" //Note this is in here twice, not sure why
#include "nsIWebBrowser.h" 
#include "nsComponentManagerUtils.h"

#include "RMozilla.h"
#include "gtk/gtk.h"
//#include "RChrome.h"

#define XP_UNIX
#include "jsapi.h"
#include "jsobj.h"
#include "jsstr.h"


static int xpcomInit = 0;

#define R_GET_NSISUPPORTS(obj) \
    (nsISupports *) R_ExternalPtrAddr(GET_SLOT((obj), Rf_install("ref")))


extern "C"
SEXP jscriptTest(SEXP wbrowser)
{

  nsresult rv;
  //nsIWebBrowser *browser = (nsIWebBrowser *) R_ExternalPtrAddr(wbrowser);
  nsIWebBrowser *browser = (nsIWebBrowser *) R_GET_NSISUPPORTS(wbrowser);
  //http://chadaustin.me/2009/02/evaluating-javascript-in-an-embedded-xulrunnergecko-window/
  nsCOMPtr<nsIPrincipal> principal;
  nsCOMPtr<nsIScriptSecurityManager> secMan = do_GetService(
							    NS_SCRIPTSECURITYMANAGER_CONTRACTID);
  rv = secMan->GetSystemPrincipal(getter_AddRefs(principal));
  if (NS_FAILED(rv)) {
    PROBLEM "Failed to get system principal"
      ERROR;
  }

  nsCOMPtr<nsIScriptGlobalObject> sgo = do_GetInterface(browser, &rv);
  
  if(NS_FAILED(rv))
    {
      PROBLEM "Failed to get nsIScriptGlobalObject"
	ERROR;
    }
  nsCOMPtr<nsIScriptContext> ctx = sgo->GetContext();
  
  const nsEmbedString myScript = nsEmbedString(NS_ConvertUTF8toUTF16("alert('hi from deep within C');").get(), 31);
  jsval out;
  rv = ctx->EvaluateStringWithValue(myScript,
				    sgo->GetGlobalJSObject(),
				    principal,
				    "mozembed",
				    0,
				    nsnull,
				    &out,
				    nsnull);

  if(NS_FAILED(rv))
    {
      PROBLEM "ctx->EvaluateStringWithValue failed"
	ERROR;
    }

  return ScalarLogical(TRUE);
}

extern "C"
SEXP R_EvaluateScriptInContext(//SEXP wbrowser,
			       SEXP context,
			       SEXP jscript, 
			       SEXP len)
{
  const char *script = CHAR(STRING_ELT(jscript, 0));
  int lngth = INTEGER(len)[0];


  nsresult rv;
#if 0
  //nsIWebBrowser *browser = (nsIWebBrowser *) R_ExternalPtrAddr(wbrowser);
  nsIWebBrowser *browser = (nsIWebBrowser *) R_GET_NSISUPPORTS(wbrowser);
#endif  
//http://chadaustin.me/2009/02/evaluating-javascript-in-an-embedded-xulrunnergecko-window/
  nsCOMPtr<nsIPrincipal> principal;
  nsCOMPtr<nsIScriptSecurityManager> secMan = do_GetService(
							    NS_SCRIPTSECURITYMANAGER_CONTRACTID);
  rv = secMan->GetSystemPrincipal(getter_AddRefs(principal));
  if (NS_FAILED(rv)) {
    PROBLEM "Failed to get system principal"
      ERROR;
  }
#if 0
  nsCOMPtr<nsIScriptGlobalObject> sgo = do_GetInterface(browser, &rv);
  
  if(NS_FAILED(rv))
    {
      PROBLEM "Failed to get nsIScriptGlobalObject"
	ERROR;
    }
  nsCOMPtr<nsIScriptContext> ctx = sgo->GetContext();
#endif

  nsIScriptContext *ctx = (nsIScriptContext *) R_GET_NSISUPPORTS(context);
  const nsEmbedString myScript = nsEmbedString(NS_ConvertUTF8toUTF16(script).get(), lngth);
  jsval out;
  PRBool isUndefined;
  rv = ctx->EvaluateStringWithValue(myScript,
				    //sgo->GetGlobalJSObject(),
				    (ctx->GetGlobalObject())->GetGlobalJSObject(),
				    principal,
				    "mozembed",
				    0,
				    nsnull,
				    &out,
				    &isUndefined);
				    //nsnull);

  if(NS_FAILED(rv))
    {
      PROBLEM "ctx->EvaluateStringWithValue failed"
	ERROR;
    }

  return ScalarLogical(TRUE);
}



extern "C"
SEXP R_get_nsIScriptContext(SEXP nsIWB)
{
  nsresult rv;
  nsIWebBrowser *browser = (nsIWebBrowser *) R_GET_NSISUPPORTS(nsIWB);

  nsCOMPtr<nsIScriptGlobalObject> sgo = do_GetInterface(browser, &rv);
  
  if(NS_FAILED(rv))
    {
      PROBLEM "Failed to get nsIScriptGlobalObject"
	ERROR;
    }
  nsIScriptContext *ctx = sgo->GetContext();
  NS_IF_ADDREF(ctx); 
//XXX I'm having trouble finding out if GetContext does the AddRef'ing for me, if so this is double counting
 

  SEXP ans, klass, ptr;
  
  PROTECT(klass = MAKE_CLASS("nsIScriptContext"));
  PROTECT(ans = NEW(klass));
  PROTECT(ptr = R_MakeExternalPtr(ctx, 
			     Rf_install("nsIScriptContext"), 
				  R_NilValue));
  //Rprintf("Registering Finalizer \n");
  R_RegisterCFinalizer(ptr,
			 R_nsISupportsFinalizer);
  //Rprintf("Finalizer Registered");
  SET_SLOT(ans, Rf_install("ref"),
	   ptr);

  
  UNPROTECT(3);
  return(ans);
}

extern "C"
SEXP R_get_JSContext(SEXP nsISC)
{
  nsIScriptContext* scontext = R_GET_NS_ITYPE(nsISC, nsIScriptContext);
  JSContext *jscon = (JSContext *) scontext->GetNativeContext();

  SEXP klass, ans, ptr;
  PROTECT( klass = MAKE_CLASS( "JSContext" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( ptr = R_MakeExternalPtr( jscon,
				    Rf_install( "JSContext" ),
				    R_NilValue));
  //finalizer here if needed
  SET_SLOT( ans, Rf_install( "ref" ), ptr );
  UNPROTECT(3);
  return(ans);

}
  
#include "string.h"
#include "stdlib.h"
extern "C"
SEXP jscriptTest2()
{
  //From http://egachine.berlios.de/embedding-sm-best-practice/ar01s02.html
/* EXIT_FAILURE and EXIT_SUCCE
  /* pointer to our runtime object */
  JSRuntime *runtime=NULL;
  /* pointer to our context */
  JSContext *context=NULL;
  /* pointer to our global JavaScript object */
  JSObject  *global=NULL;

  /* script to run (should return 100) */
  char *script="var x=10;x*x;";
  /* JavaScript value to store the result of the script */
  jsval rval;
  
  /* create new runtime, new context, global object */
  if (    (!(runtime = JS_NewRuntime (1024L*1024L)))
       || (!(context = JS_NewContext (runtime, 8192)))
       || (!(global  = JS_NewObject  (context, NULL, NULL, NULL)))
	  ) return ScalarLogical(FALSE);
  /* set global object of context and initialize standard ECMAScript
     objects (Math, Date, ...) within this global object scope */
  if (!JS_InitStandardClasses(context, global)) return ScalarLogical(FALSE);

  /* now we are ready to run our script */
  if (!JS_EvaluateScript(context, global,script, strlen(script),
			 "script", 1, &rval))
    return ScalarLogical(FALSE);
  /* check if the result is really 100 */
  if (!(JSVAL_IS_INT(rval)&&(JSVAL_TO_INT(rval)==100)))
    return ScalarLogical(FALSE);

  /* clean up */
  JS_DestroyContext(context);
  JS_DestroyRuntime(runtime);
  JS_ShutDown();
  return ScalarLogical(TRUE);
}

extern "C"
SEXP R_getJSVALInt(SEXP jsvalref)
{
  jsval val = * ((jsval*)R_ExternalPtrAddr(GET_SLOT(jsvalref, 
						    Rf_install("ref"))));

  SEXP ans;
  PROTECT(ans = NEW_INTEGER(1));
  INTEGER(ans)[0] = (uint) val;
  UNPROTECT(1);
  return ans;
}

extern "C"
SEXP R_CallMethodTest(SEXP cx, SEXP parentasjsval, SEXP funname,
		      SEXP nargs, SEXP args, SEXP retval)
{
  JSContext *context = (JSContext *) R_ExternalPtrAddr(GET_SLOT(cx, Rf_install("ref")));
  
  JSObject *parent;
  JS_ValueToObject(context, *(jsval *) R_ExternalPtrAddr(GET_SLOT(parentasjsval, Rf_install("ref"))), &parent);
  const char *fname = CHAR(STRING_ELT(funname, 0));
  unsigned int numarg = INTEGER(nargs)[0];
  jsval * argsv = (jsval *) R_ExternalPtrAddr(GET_SLOT(args, Rf_install("ref")));
  jsval * rval = (jsval *) R_ExternalPtrAddr(GET_SLOT(retval, Rf_install("ref")));
  
  JSBool rv = JS_CallFunctionName(context, parent, fname, numarg, argsv, rval);

  return rv ? ScalarLogical(TRUE):ScalarLogical(FALSE);
}

extern "C"
SEXP R_CallFunctionNameTestC(SEXP cx, SEXP parentjsvRef)
{
  JSContext *context = (JSContext *) R_ExternalPtrAddr(cx);
  jsval * parentref= (jsval *) R_ExternalPtrAddr(GET_SLOT(parentjsvRef, Rf_install("ref")));

  const char * fname = "getElementById";
  
  JSObject *parobj;
  JSBool rv = JS_ValueToObject(context, *parentref, &parobj);
  
  JSString *arg = JS_NewStringCopyZ(context, "Success!");
  
  jsval mydivjsv;
  jsval string = STRING_TO_JSVAL(arg);
  JS_AddRoot(context, &mydivjsv);
  fprintf(stderr, "mydivjsv: %u\n", mydivjsv);fflush(stderr);
  rv = JS_CallFunctionName(context, parobj, fname, 1, &string, 
			   &mydivjsv);
  if (!rv)
    fprintf(stderr, "JS_CallFunctionName failed"); fflush(stderr);

  fprintf(stderr, "mydivjsv: %u\n", mydivjsv);fflush(stderr);
  //hacky hacky
  JSObject* mydiv = JS_ConstructObject(context, NULL, NULL, NULL);
  fprintf(stderr, "mydiv object address: %u\n", mydiv); fflush(stderr);
  fprintf(stderr, "mydivjsv: %u\n", mydivjsv);fflush(stderr);
  rv = JS_ValueToObject(context, mydivjsv, &mydiv);
  fprintf(stderr, "mydiv object address: %u\n", mydiv); fflush(stderr); 
  rv = JS_SetProperty(context,  mydiv, "innerHTML", &string);
  
 
  return rv ? ScalarLogical(TRUE) : ScalarLogical(FALSE);
}
  
  
  

  


  
