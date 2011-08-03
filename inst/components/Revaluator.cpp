

#include "nsIScriptContext.h"
#include "nsIServiceManager.h"
#include "nsServiceManagerUtils.h"
#include "nsIScriptGlobalObject.h"
#include "nsIWindowMediator.h"
#include "nsIDOMWindow.h"
#include "nsIDOMDocument.h"
#include "nsIDocument.h"
#include "nsXPCOM.h"
#include "nsIDOMWindowInternal.h"
#include "nsCOMPtr.h"
#include "nsIScriptSecurityManager.h"
#include "RXPCOM.h"
#include "RMozilla.h"
#include <Rdefines.h>
#include <Rinternals.h>

#include "jsapi.h"
#include "jsobj.h"
#include "nsIJSContextStack.h"
#include "nsIScriptObjectPrincipal.h"
#include "nsIXPConnect.h"
//#include "nsIServiceManager.h"
#include "nsServiceManagerUtils.h"


NS_IMPL_ISUPPORTS1(REvaluatorImpl, REvaluator)

void R_getServiceManager();
JSContext *GetContextForR();
JSBool FunCallTest(nsIPrincipal *principal, JSContext *con);
SEXP jscriptTest2();
nsresult TestnsScriptContext(nsIScriptContext *context, nsIPrincipal *principal);
JSBool TestJSContextEval(JSContext *jscon, nsIPrincipal *principal, const char *code);
JSBool TestJSContextFunCall(nsIPrincipal *principal, JSContext *jscon);
nsresult doContextPush(nsIPrincipal *principal, JSContext *jscon);
nsresult doContextPop(nsIPrincipal *principal, JSContext *jscon);
nsIScriptGlobalObject *grabnsIScriptGlobalObject();

//duplicate from RXPCOM.cpp
//Finalizers are making things unhappy. Turning off for now.
extern "C"
void R_nsISupportsFinalizer(SEXP ptr)
{
  Rprintf( "Finalizers are currently disabled.");
  //Rprintf( "Executing Finalizer" );
  //R_NS_IF_RELEASE( ptr );
  
  return;
}
/* Added by gabe for debugging */
extern "C"
int *(*GabeGetINTEGER())(SEXP)
{
	return &INTEGER;
}



REvaluatorImpl::REvaluatorImpl()
{
}

static int isInitialized = 0;

#include <dlfcn.h>
NS_IMETHODIMP
REvaluatorImpl::Init(const char **args, PRUint32 nargs, PRBool *_retval)
{

  char **rargs;
  unsigned int i;

  if(isInitialized)
    return(NS_OK);



  R_getServiceManager();

  rargs = (char **) malloc(nargs * sizeof(char *));
  for(i = 0 ; i < nargs; i++)
    rargs[i] = strdup(args[i]);
  fprintf(stderr, "Attempting to start embedded R.");fflush(stderr);
  Rf_initEmbeddedR(nargs, rargs);
  fprintf(stderr, "R initialization done."); fflush(stderr);
#if 0
  /* Testing */
  SEXP ans, e;
  PROTECT(e =  allocVector(LANGSXP, 2));
  SETCAR(e, Rf_install("rnorm"));
  SETCAR(CDR(e), ScalarInteger(10));
  Rf_PrintValue(Rf_eval(e, R_GlobalEnv));
#endif

  //We're going to want to ifdef this out because it is only required on
  // linux (ubuntu specifically), so far. If both Mac and Windows don't 
  // need it then it is a large waste of memory on those OSes.
  /* 
void *h = dlopen("/home/gmbecker/Downloads/R-2.12.0/lib/libR.so", RTLD_NOW);
  if(h == NULL)
    PROBLEM "dlopen failed" ERROR;

  Rf__INTEGER = (R_INT_PTR)dlsym(h, "INTEGER");
  if (Rf__INTEGER == NULL)
    PROBLEM "dlsym failed" ERROR;
  */
  /* Testing Gabe */

#if 0  
  SEXP test;
  PROTECT(test = allocVector(INTSXP, 5));
  int *tmp = Rf__INTEGER(test); //-Bstatic when linking libR does not seem to have helped, -static causes linking to fail altogether
  tmp[1]=5;
  Rf_PrintValue(test);
  //fprintf(stderr, "INTEGER test: %d\n", INTEGER(test)[0]);
  UNPROTECT(1);
#endif
 
  //Loading RFirefox (which loads SpiderMonkey. We need these for
  //parseEval and JSContextRef class.
  nsIVariant *retval;
  fprintf(stderr, "before library call in init");fflush(stderr);
  this -> Library("RFirefox", &retval);
  fprintf(stderr, "after library call in init"); fflush(stderr);
  PRBool loaded;
  nsresult rv;
  rv = retval -> GetAsBool(&loaded);
 
  isInitialized = 1;
  *_retval = true;

  return(NS_OK);
}

nsresult
REvaluatorImpl::Call1(const char *funName, nsIVariant *arg, nsIVariant **_retval)
{
  SEXP ans, e, p;
  int wasError = 0;
  
  if(!funName || !funName[0]) {
    // Throw an error to the caller.
    fprintf(stderr, "Empty function name in Call1\n");fflush(stderr);
    return(NS_ERROR_BASE);
  }

  //  fprintf(stderr, "Calling function %s\n", funName);fflush(stderr);
  PROTECT(e = allocVector(LANGSXP, 2));

  SETCAR(e, Rf_install(funName)); p = CDR(e);
  SETCAR(p, convertVariantToR(arg));

  PROTECT(ans = R_tryEval(e, R_GlobalEnv, &wasError));//gabe added PROTECT

  convertRToVariant(ans, _retval);
  UNPROTECT(2); //gabe added;
  return(NS_OK);
}


  //we seem to need the internal version of strings, etc here. Including nsContentUtils.h without them causes errors. nsIContentUtils does not contain the class we need for the variable argument call method
//#include <nsContentUtils.h>

nsresult
//We have to get the arguments from the javascript call through xpconnect. see nsGlobalWindow::OpenDialog in nsGlobalWindow.cpp
REvaluatorImpl::Call(const char *funName, nsIVariant **_retval)
{

#if TRUE
  SEXP ans, e, p;
  int wasError = 0;
  nsresult rv;
  
  if(!funName || !funName[0]) {
    // Throw an error to the caller.
    fprintf(stderr, "Empty function name in Call\n");fflush(stderr);
    return(NS_ERROR_BASE);
  }
  
  //From nsGClobalWindow.cpp
  //if (!nsContentUtils::IsCallerTrustedForWrite()) {
  //  return NS_ERROR_BASE;
  // }
  
  nsCOMPtr<nsIXPConnect> xpc_;
  xpc_ = do_GetService("@mozilla.org/js/xpc/XPConnect;1", &rv);

  nsAXPCNativeCallContext *ncc = nsnull;

  rv = xpc_ -> GetCurrentNativeCallContext(&ncc);
  NS_ENSURE_SUCCESS(rv, rv);

  if (!ncc)
    return NS_ERROR_NOT_AVAILABLE;

  JSContext *cx = nsnull;

  rv = ncc->GetJSContext(&cx);
  NS_ENSURE_SUCCESS(rv, rv);

  PRUint32 argc;
  jsval *argv = nsnull;

  // XXX - need to get this as nsISupports?
  ncc->GetArgc(&argc);
  ncc->GetArgvPtr(&argv);

  fprintf(stderr, "argc: %u", argc); fflush(stderr);
 
  //  fprintf(stderr, "Calling function %s\n", funName);fflush(stderr);
  PROTECT(e = allocVector(LANGSXP, argc));

  SETCAR(e, Rf_install( funName ) ); 
  nsIVariant *tmp;
  PROTECT( p = e );

  
  for (int i=1; i < argc; i++) 
    {
      p = CDR( p );
      rv = xpc_ -> JSToVariant(cx, argv[i], &tmp);
      SETCAR( p , convertVariantToR( tmp ) );
    }
  Rf_PrintValue(e);
  PROTECT(ans = R_tryEval( e , R_GlobalEnv , &wasError ) );//gabe added PROTECT

  convertRToVariant( ans , _retval );
  UNPROTECT(3); //gabe added;
#endif
  return(NS_OK);
}

#include "nsCOMPtr.h"
//#include <xpcom/nsComponentManagerUtils.h>
#include <nsComponentManagerUtils.h>
nsresult
REvaluatorImpl::Library(const char *funName, nsIVariant **_retval)
{
  SEXP ans, e, p;
  int wasError = 0, errorOccurred = 0;

  PROTECT(e = allocVector(LANGSXP, 2));
  SETCAR(e, Rf_install("library"));
  SETCAR(CDR(e), mkString(funName));

  R_tryEval(e, R_GlobalEnv, &errorOccurred);
  UNPROTECT(1);

  nsCOMPtr<nsIWritableVariant> var;
  nsresult nr;
  var = do_CreateInstance("@mozilla.org/variant;1", &nr);
  var->SetAsBool(errorOccurred ? 0 : 1);
  var->QueryInterface(NS_GET_IID(nsIVariant), (void **)_retval);

  return(NS_OK);
}

int R_EVAL_ERROR = 345 & 0x80000000;


static int evalDepth = 0;

nsresult
REvaluatorImpl::Eval(const char *funName, nsIVariant **_retval)
{
  fprintf(stderr, "REvaluatorImpl::Eval is executing."); fflush(stderr);
#if 0

#endif
  JSContext *jscon;
#if 1
  nsresult rv;
 jscon = GetContextForR();
 
  nsCOMPtr<nsIJSContextStack> stack(do_GetService("@mozilla.org/js/xpc/ContextStack;1"));
  //Grab context;
  
  if (evalDepth == 0)
    {
      //jscon = GetContextForR();
      
      rv = stack -> Push( jscon );
      if (NS_FAILED(rv))
	fprintf(stderr, "Failed to push context onto context stack: %d", rv);fflush(stderr);
    }
  evalDepth++;
 

  /**/
  SEXP ans, e, p;
  int wasError = 0, errorOccurred = 0;

  PROTECT(e = allocVector(LANGSXP, 2));
  SETCAR(e, Rf_install("parseEval"));
  SETCAR(CDR(e), mkString(funName));

  PROTECT(ans = R_tryEval(e, R_GlobalEnv, &errorOccurred)); //gabe added PROTECT
  
  convertRToVariant(ans, _retval);
  UNPROTECT(2);

  
  if (evalDepth == 1)
    {
      stack -> Pop( &jscon );
    }
  evalDepth--;
  
#endif  

  /**/
  //return(errorOccurred ? R_EVAL_ERROR : NS_OK);
  return(NS_OK);
}

nsresult
REvaluatorImpl::Exists(const char *varName, nsIVariant **_retval)
{
  SEXP ans;

  if(!varName || !varName[0]) {
      return(R_EVAL_ERROR);
  }

  ans = Rf_findVarInFrame3(R_GlobalEnv, Rf_install(varName), (Rboolean) FALSE);
  if(ans == R_UnboundValue)
      ans = ScalarLogical(FALSE);
  else
      ans = ScalarLogical(TRUE);

  convertRToVariant(ans, _retval);

  return(NS_OK);
}


nsresult
REvaluatorImpl::Get(const char *varName, nsIVariant **_retval)
{
  SEXP ans, tmp;

  if(!varName || !varName[0]) {
      return(R_EVAL_ERROR);
  }

  PROTECT(tmp = Rf_install(varName));
  PROTECT(ans = Rf_findVarInFrame3(R_GlobalEnv, tmp, (Rboolean) TRUE));
  //ans = Rf_findVar(tmp, R_GlobalEnv);
  convertRToVariant(ans, _retval);
  UNPROTECT(2);
  return(NS_OK);
}

nsresult
REvaluatorImpl::Assign(const char *varName, nsIVariant *val, nsIVariant **_retval)
{
    SEXP ans, rval;

  if(!varName || !varName[0]) {
      return(R_EVAL_ERROR);
  }

  rval = convertVariantToR(val);

  Rf_defineVar(Rf_install(varName), rval, R_GlobalEnv);
  convertRToVariant(ScalarLogical(TRUE), _retval);

  return(NS_OK);
}


nsresult
REvaluatorImpl::Remove(const char *varName, nsIVariant **_retval)
{
    SEXP e, tmp, ans, rval;
    int errorOccurred = 0;
  if(!varName || !varName[0]) {
      return(R_EVAL_ERROR);
  }

  PROTECT(e = allocVector(LANGSXP, 2));
  SETCAR(e, Rf_install("remove"));
  tmp = CDR(e);
  SETCAR(tmp, mkString(varName));
  SET_TAG(tmp, Rf_install("list"));
  ans = R_tryEval(e, R_GlobalEnv, &errorOccurred);

  convertRToVariant(ScalarLogical(!errorOccurred), _retval);  

  return(NS_OK);
}

#include "Rversion.h"

char RVersionString[20] = "\0";

nsresult
REvaluatorImpl::GetVersion(char * *aVersion)
{
    if(!RVersionString[0])
        sprintf(RVersionString, "%s.%s", R_MAJOR, R_MINOR);
    *aVersion = RVersionString;
    return(NS_OK);
}

/* Testing */

#include "nsXPCOM.h" 
#include "nsIServiceManager.h"
/*
#include "xpcom/nsIInterfaceInfoManager.h"
#include "xpcom/nsIInterfaceInfo.h"
#include "xpcom/nsIInterfaceRequestorUtils.h"
#include "xpcom/xptinfo.h"
*/
#include "nsIInterfaceInfoManager.h"
#include "nsIInterfaceInfo.h"
#include "nsIInterfaceRequestorUtils.h"
#include "xptinfo.h"

//GB
/*
#undef NS_GetServiceManager
XPCOM_API(nsresult) NS_GetServiceManager(nsIServiceManager* *result);
*/
void
R_getServiceManager()
{
#if 1
    nsCOMPtr<nsIServiceManager> servMan; 
    nsCOMPtr<nsIInterfaceInfoManager> imgr;
    nsresult nr;
    nr = NS_GetServiceManager(getter_AddRefs(servMan)); 
    fprintf(stderr, "ServiceManager %p", (void*) servMan); fflush(stderr);
    if(NS_FAILED(nr)) {
        PROBLEM  "Can't get ServiceManager"
            ERROR;
    }
    nr = servMan->GetServiceByContractID("@mozilla.org/xpti/interfaceinfomanager-service;1",
                                         NS_GET_IID(nsIInterfaceInfoManager), getter_AddRefs(imgr)); 

    fprintf(stderr, "interface info manager %p", (void*) imgr); fflush(stderr);
    if(NS_FAILED(nr)) {
        PROBLEM  "Can't get InterfaceInfoManager"
            ERROR;
    }
#endif
}

static int spidermonkey = 0;

JSContext *
GetContextForR()
{
  fprintf(stderr, "GetContextForR is running"); fflush(stderr);
  nsresult rv;
  //already addreffed
  //nsIDocument *doc = grabnsIDocument();
  nsIScriptGlobalObject *globj = grabnsIScriptGlobalObject();
  

  nsCOMPtr<nsIScriptContext> context = globj->GetScriptContext(nsIProgrammingLanguage::JAVASCRIPT);

  if(context == nsnull)
    PROBLEM "Script object was null" ERROR;
  //  context -> AddRef();
  
  JSContext *jscon = static_cast<JSContext *> (context -> GetNativeContext());
  //JSContext *jscon = (JSContext *) (context -> GetNativeContext());

  //JSClass *globclass = static_cast<JSClass *> (context -> GetNativeGlobal());
  JSObject *glob2 = static_cast<JSObject *> (context -> GetNativeGlobal()); 
  if (!jscon)
    fprintf(stderr, "Failed to get native javascript context");fflush(stderr);
  
  if (!glob2)
    fprintf(stderr, "Failed to get native global object"); fflush(stderr);


  nsCOMPtr<nsIScriptObjectPrincipal> objprincipal = do_QueryInterface(globj, &rv);
    
  nsIPrincipal *principal = objprincipal -> GetPrincipal();
  NS_IF_ADDREF(principal);
  //nsCOMPtr<nsIPrincipal> principal = objprincipal -> GetPrincipal();
  //testing nsIScriptContext->EvaluateStringWithValue
#ifdef TEST_NS_SCRIPTCONTEXT
  rv = TestnsScriptContext(context, principal);
  
  if(NS_FAILED(rv))
    fprintf(stderr, "TestnsScriptContext failed.");fflush(stderr);
#endif

  //testing JScontext eval;
  JSBool success;
  // success = doContextPush(principal, jscon);
#ifdef TEST_JSCONTEXT_EVAL
  const char *script = "var div = document.getElementById('test1'); div.innerHTML='JSContext eval test 1 successful.';";
  success = TestJSContextEval(jscon, principal, script);
  //success = doContextPop(principal, jscon);
#endif

#ifdef TEST_JSCONTEXT_EVAL2
  //success = doContextPush(principal, jscon);
  const char *script2 = "var div = document.getElementById('test2'); div.innerHTML='JSContext eval test 2 successful.';";
  success = TestJSContextEval(jscon, principal, script2);
  //success = doContextPop(principal, jscon);
#endif

  //testing direct function calling;
#ifdef TEST_JSCONTEXT_FUNCALL
  //  JSBool success2 = TestJSContextFunCall(principal, jscon);
  success = FunCallTest(principal, jscon);
#endif

  NS_IF_RELEASE(principal);
  NS_IF_RELEASE(globj);
  //Create R object and place it in global environment.
  SEXP klass, ans, ptr;
  PROTECT( klass = MAKE_CLASS( "JSContextRef" ) );
  PROTECT( ans = NEW( klass ) );
  PROTECT( ptr = R_MakeExternalPtr( jscon,
				    Rf_install( "JSContext" ),
				    R_NilValue));
  
  //finalizer here if needed
  R_RegisterCFinalizerEx(ptr , R_nsISupportsFinalizer, (Rboolean) TRUE);
  SET_SLOT(ans, Rf_install("ref"), ptr);
  Rf_defineVar(Rf_install("ScriptCon"), ans, R_GlobalEnv);
  UNPROTECT(3);
  return jscon;
}

nsresult TestnsScriptContext(nsIScriptContext *context, nsIPrincipal *principal)
{
  nsresult rv;
  nsCOMPtr<nsIScriptGlobalObject> globj = context->GetGlobalObject();
  //testing nsIScriptContext;
  
  
  fprintf(stderr, " nsIPrincipal: %u \n", principal); fflush(stderr);

  const char *nscript = "alert('nsIScriptContext working properly');";
  
  jsval out;
  
  PRBool isUndefined;
  
  rv = context -> EvaluateStringWithValue(NS_ConvertUTF8toUTF16(nscript),
					  //&(context -> GetGlobalObject()) -> GetGlobalJSObject,;
					  context -> GetNativeGlobal(), 
					  principal,
					  "hithere",
					  0,
					  nsnull,
					  &out,
					  &isUndefined);


  return(rv);
}


JSBool TestJSContextEval(JSContext *jscon, nsIPrincipal *principal, const char *code)
{

  jsval retval;
  int linnum;
  nsresult rv;

  //doContextPush(principal, jscon);
  //const char *script = "var x =5; x*x;";
  JSObject * glob = JS_GetGlobalObject(jscon);
  JS_AddRoot(jscon, &glob);
  //const char *script = "var div = document.getElementById('test1'); div.innerHTML = 'hi there, that is weird.';";
  JSBool success;
  success = JS_EvaluateScript(jscon, glob,
		    code, strlen(code), "thing",
		    0, &retval);
  JS_AddRoot(jscon, &retval);
  if(success != JS_TRUE)
    {
      fprintf(stderr,"jsapi failed to execute \n"); 
      fflush(stderr);
    }
  else
    { 
      fprintf(stderr, "jsapi executed and returned %d\n", JSVAL_TO_INT(retval));
      fflush(stderr);
    }
  JS_RemoveRoot(jscon, &glob);
  JS_RemoveRoot(jscon, &retval);
  //doContextPop(principal, jscon);
  return(success);
}

  
JSBool FunCallTest(nsIPrincipal *principal, JSContext *con)
{
  JSBool success = JS_TRUE;
   
  int i=1;
  //1;
  fprintf(stderr, "GC call %d commencing\n", i); fflush(stderr);
  //JS_GC(con);
  fprintf(stderr, "GC call %d successful\n", i); fflush(stderr);
  i++;
  jsval *rv, fun, tmp;
  JSObject *div = JS_NewObject(con, nsnull, nsnull, nsnull), 
    *doc =JS_NewObject(con, nsnull, nsnull, nsnull) ;
  const char *code =  "document;";
  JSString *id, *text;
  JSClass *cl;
  JS_AddRoot(con, rv);  
  //2;
  fprintf(stderr, "GC call %d commencing\n", i); fflush(stderr);
  //JS_GC(con);
  fprintf(stderr, "GC call %d successful\n", i); fflush(stderr);
  i++;

  fprintf(stderr, "code: %s\ncodelen:%u\n", code, strlen(code));fflush(stderr);
  unsigned int len = strlen(code);
  success = JS_EvaluateScript(con, JS_GetGlobalObject(con), code, len, "script", 1, rv);
  
  if (!success)
    {
      fprintf(stderr, "JS_EvaluateScript failed"); fflush(stderr);
    }
  //3;
  
  fprintf(stderr, "GC call %d commencing\n", i); fflush(stderr);
  //JS_GC(con);
  fprintf(stderr, "GC call %d successful\n", i); fflush(stderr);
  i++;  
  
  success = JS_ConvertValue(con, *rv,  JSTYPE_OBJECT, &tmp);
  JS_AddRoot(con, &tmp);
  if (!success)
    {
      fprintf(stderr, "Failed to convert jsval to JSObject"); fflush(stderr);
    }
  
  //4;
  fprintf(stderr, "GC call %d commencing\n", i); fflush(stderr);
  // JS_GC(con);
  fprintf(stderr, "GC call %d successful\n", i); fflush(stderr);
  i++;
  
  jsval *rv2;
  id = JS_NewStringCopyZ(con, "test3");
  JS_AddRoot(con, &id);
  jsval idval = STRING_TO_JSVAL(id);
  JS_AddRoot(con, &idval);
  fprintf(stderr, "tmp %u idvalptr %u rvptr %u\n", tmp, &idval, rv);fflush(stderr);
  				 
  success = JS_ValueToObject(con, tmp, &doc);

  //5;
  fprintf(stderr, "GC call %d commencing\n", i); fflush(stderr);
  //JS_GC(con);
  fprintf(stderr, "GC call %d successful\n", i); fflush(stderr);
  i++;

  success = JS_CallFunctionName(con, doc, "getElementById", 1,
  				 &idval , rv);
   
  //JS_AddRoot(con, rv);
  //6;
  fprintf(stderr, "jsval rv: %ld\n", *rv); fflush(stderr);

  fprintf(stderr, "GC call %d commencing\n", i); fflush(stderr);
  //JS_GC(con);
  fprintf(stderr, "GC call %d successful\n", i); fflush(stderr);
  i++;

  fprintf(stderr, "jsval rv: %ld\n", *rv); fflush(stderr);
 
  if (!success)
    {
      fprintf(stderr, "Failed to call function\n"); fflush(stderr);
    }   

  success = JS_ValueToObject(con, *rv, &div);
  
  cl = JS_GET_CLASS(con, div);
  fprintf(stderr, "div object class: %s\n", cl->name);fflush(stderr);
  //7;
  fprintf(stderr, "GC call %d commencing\n", i); fflush(stderr);
  //JS_GC(con);
  fprintf(stderr, "GC call %d successful\n", i); fflush(stderr);
  i++;  
  

 if (!success)
    {
      fprintf(stderr, "Failed to convert jsval to JSObject\n"); fflush(stderr);
    }   
  
 text = JS_NewStringCopyZ(con, "Direct Function Calls and Property Setting within C code Successful.");
 JS_AddRoot(con, &text);
 jsval textval;
 textval = STRING_TO_JSVAL(text);
 JS_AddRoot(con, &textval);
 const char *propname = "innerHTML";
 success = JS_SetProperty(con, div, propname, &textval);

 if (!success)
    {
      fprintf(stderr, "Failed to set object property\n"); fflush(stderr);
    }   
   
  JS_RemoveRoot(con, &textval);
  JS_RemoveRoot(con, &idval); 
  JS_RemoveRoot(con, &tmp);
  JS_RemoveRoot(con, rv);
  JS_RemoveRoot(con, &text);
  JS_RemoveRoot(con, &id);
   
  //8;
  fprintf(stderr, "GC call %d commencing\n", i); fflush(stderr);
  //JS_GC(con);
 fprintf(stderr, "GC call %d successful\n", i); fflush(stderr);
 
 return success;
}

nsresult doContextPush(nsIPrincipal *principal, JSContext *jscon)
{
  nsresult rv;
  nsCOMPtr<nsIScriptSecurityManager> secManOld = 
    do_GetService(NS_SCRIPTSECURITYMANAGER_CONTRACTID, &rv);  

  nsCOMPtr<nsIScriptSecurityManager_1_9_2> secMan = do_QueryInterface(secManOld, &rv);
  if (NS_FAILED(rv)) {
    fprintf(stderr, "Failed to get Script Security Manager 1.9.2: %u \n", rv); fflush(stderr);
    return(JS_FALSE);
  }
 
  JSObject * glob = JS_GetGlobalObject(jscon);
  
  JSPrincipals *jsprinc;
  principal -> GetJSPrincipals( jscon , &jsprinc);
  //We need to do JSPRINCIPALS_DROP before returning as per nsJSEnvironment.cpp;

  
  PRBool ok = PR_FALSE;
  rv = secMan -> CanExecuteScripts(jscon, principal, &ok);
  
  if (NS_FAILED( rv ) )
    {
      JSPRINCIPALS_DROP( jscon, jsprinc );
      fprintf(stderr, "CanExecuteScripts failed."); fflush( stderr );
    }

  nsCOMPtr<nsIJSContextStack> stack =
    do_GetService("@mozilla.org/js/xpc/ContextStack;1", &rv);

  
  if (NS_FAILED(rv) || NS_FAILED( stack -> Push( jscon ) ) ) {
    JSPRINCIPALS_DROP( jscon , jsprinc );
    fprintf(stderr, "Failed to push context onto context stack."); fflush( stderr );
    }		 
  
  rv = secMan -> PushContextPrincipal( jscon, nsnull, principal);
  if ( NS_FAILED(rv) )
    {
      JSPRINCIPALS_DROP( jscon , jsprinc );
      fprintf(stderr, "Failed To push context principal"); fflush( stderr );
    }
  return rv;
}

nsresult doContextPop(nsIPrincipal *principal, JSContext *jscon)
{
  nsresult rv;
  nsCOMPtr<nsIScriptSecurityManager> secManOld = 
    do_GetService(NS_SCRIPTSECURITYMANAGER_CONTRACTID, &rv);  
  
  nsCOMPtr<nsIScriptSecurityManager_1_9_2> secMan = do_QueryInterface(secManOld, &rv);
  if (NS_FAILED(rv)) {
    fprintf(stderr, "Failed to get Script Security Manager 1.9.2: %u \n", rv); fflush(stderr);
  }
  
  secMan -> PopContextPrincipal( jscon );  

  nsCOMPtr<nsIJSContextStack> stack =
    do_GetService("@mozilla.org/js/xpc/ContextStack;1", &rv);

  JSPrincipals *jsprinc;
  principal -> GetJSPrincipals( jscon , &jsprinc);
  
  if (NS_FAILED(rv))
    {
      fprintf(stderr, "Failed to grab JSContextStack: %u", rv); fflush(stderr);
    }
  
  if ( NS_FAILED( stack -> Pop( nsnull ) ) )
    {
      fprintf( stderr , "Failed to pop context off of stack."); fflush( stderr );
    }
  
  JSPRINCIPALS_DROP( jscon , jsprinc );

}

JSBool TestJSContextFunCall(nsIPrincipal *principal, JSContext *jscon)
{
  //doContextPush(principal, jscon);
  jsval *retval;
  JSObject * glob = JS_GetGlobalObject(jscon);
  const char *script = "var div = document.getElementById('test2'); div.innerHTML = 'hi there, that is weird.';";
  JSBool success;
  success = JS_EvaluateScript(jscon, glob,
		    script, strlen(script), "thing",
		    0, retval);
  if(success != JS_TRUE)
    {
      fprintf(stderr,"jsapi failed to execute \n"); 
      fflush(stderr);
    }
  else
    { 
      fprintf(stderr, "jsapi executed and returned %d\n", JSVAL_TO_INT(*retval));
      fflush(stderr);
    }

  //doContextPop(principal, jscon); 
  return success;
}

/*Returns addref'ed pointer to nsIScriptGlobalObject corresponding to open 
browser window.
*/
nsIScriptGlobalObject *grabnsIScriptGlobalObject()
{
nsresult rv;

  nsCOMPtr<nsIWindowMediator> winmed = do_GetService(NS_WINDOWMEDIATOR_CONTRACTID, &rv);
  
  if (NS_FAILED(rv))
    {
      fprintf(stderr, "Failed to get window mediator %u", rv);
      PROBLEM "Failed to get Window Mediator" ERROR;
    }
  nsCOMPtr<nsIDOMWindowInternal> domwinint;
  rv = winmed -> GetMostRecentWindow( NS_ConvertASCIItoUTF16("navigator:browser").get(), getter_AddRefs( domwinint ));
  
  if (NS_FAILED(rv))
    PROBLEM "Failed to get most recent window" ERROR;

  nsCOMPtr<nsIDOMWindow> domwin;
  rv = domwinint -> GetContent( getter_AddRefs( domwin ));
  if (NS_FAILED(rv))
    PROBLEM "Failed to get content from domwindowinternal" ERROR;

  nsCOMPtr<nsIDOMDocument> domdoc;
  rv = domwin -> GetDocument( getter_AddRefs(domdoc) );
  
  if (NS_FAILED(rv))
    PROBLEM "Failed to get DOM Document from dom window" ERROR;

  nsCOMPtr<nsIDocument> doc(do_QueryInterface(domdoc, &rv));  
 
  if (NS_FAILED(rv))
    PROBLEM "Failed to get Document from DOM Document" ERROR;

  nsIScriptGlobalObject *globj;
  globj = doc -> GetScriptGlobalObject();
  NS_IF_ADDREF(globj);
  return globj;
}
