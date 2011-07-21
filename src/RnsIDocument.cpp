#include <iostream>


#include "nsIDocument.h"
/*
#include "js/jsapi.h"
#include "js/jsobj.h"
#include "dom/nsIDOMHTMLElement.h"

//#include "xpcom/nsStringAPI.h"
#include "string/nsEmbedString.h"
*/
#include "Moz191Compat.h"
#include "RMozilla.h"

SEXP convert_nsIURI(nsIURI *uri);

//This has been removed between 191 and 192


#if MOZVER == 191
extern "C"

SEXP
R_nsIDocument_getJSObject(SEXP r_doc)
{
    nsIDocument *doc;
    doc = R_GET_NSIDOCUMENT(r_doc);
    JSObject *js;
    js = doc->GetJSObject();
    fprintf(stderr, "JSObject %p\n", js);
    JSClass *klass = STOBJ_GET_CLASS(js);
    fprintf(stderr, "JSObject class %s\n", klass->name);    

//nsCOMPtr<nsIDOMHTMLElement> element 

    return(R_NilValue);
}
#endif

extern "C"
SEXP
R_nsIDocument_getDocumentCharacterSetSource(SEXP r_doc)
{
    nsIDocument *doc;
    doc = R_GET_NSIDOCUMENT(r_doc);
    return(ScalarInteger(doc->GetDocumentCharacterSetSource()));
}

extern "C"
SEXP
R_nsIDocument_GetWindow(SEXP r_doc)
{
    nsIDocument *doc;
    doc = R_GET_NSIDOCUMENT(r_doc);
    nsAutoString str;
    nsPIDOMWindow *win = doc->GetWindow();
    if(!win)
        return(R_NilValue);
    else {
        SEXP klass, ans;
        PROTECT(klass =  MAKE_CLASS("nsPIDOMWindow"));
        PROTECT(ans = NEW(klass));
        SET_SLOT(ans, Rf_install("ref"), R_MakeExternalPtr(win, Rf_install("nsPIDOMWindow"), R_NilValue));
        UNPROTECT(2);
        return(ans);
    }
}

extern "C"
SEXP
R_nsIDocument_getContentType(SEXP r_doc)
{
    nsIDocument *doc;
    doc = R_GET_NSIDOCUMENT(r_doc);
    nsAutoString str;
    nsresult nr = doc->GetContentType(str);
    if(NS_FAILED(nr)) 
        return(NEW_CHARACTER(0));
    else {
        const char *val;
//        const nsString *ref = &str;
//        val = ToNewCString(ref);
        NS_LossyConvertUTF16toASCII tmp(str.get());
        val = tmp.get();
//        len = NS_StringGetData(str, &val, &terminated);
        return(mkString(val ? val : ""));
    }
}

extern "C"
SEXP
R_nsIDocument_GetBaseURI(SEXP r_doc)
{
    nsIDocument *doc;
    doc = R_GET_NSIDOCUMENT(r_doc);

   fprintf(stderr, "Document %p\n", doc);fflush(stderr);
   nsIURI *uri;
    nsresult nr;

    uri = doc->GetDocumentURI();
    convert_nsIURI(uri);

    uri = doc->GetBaseURI();
    return(convert_nsIURI(uri));
}

SEXP
convert_nsIURI(nsIURI *uri)
{
    nsresult nr;
    const char *val;
    PRBool terminated;
    unsigned int len;
    nsCString str;
    
    nr = uri->GetAsciiSpec(str);

    if(NS_FAILED(nr)) {
        PROBLEM "can't get URI's ascii spec"
            WARN;
        return(NEW_CHARACTER(0));
    }

//    std::cerr << uri << std::endl;

    len = NS_CStringGetData(str, &val, &terminated);
    fprintf(stderr, "CStringGetData %d %s\n", (int) len, val);fflush(stderr);
    
    if(val)
        return(mkString(val));
    else
        return(NEW_CHARACTER(0));
}








extern "C"
SEXP 
R_evalJSScript(SEXP r_doc, SEXP r_jsobj, SEXP r_script, SEXP r_file, SEXP r_line)
{
    nsIDocument *doc;
    doc = R_GET_NSIDOCUMENT(r_doc);
  
    JSObject *js_obj = NULL;
    if(r_jsobj != R_NilValue) {
        js_obj = (JSObject *)  R_GET_NSIDOCUMENT(r_jsobj) ;
    }

fprintf(stderr, "doc: %p, jsobj = %p\n", doc, js_obj);fflush(stderr);

    nsIScriptGlobalObject* globalObject = doc->GetScriptGlobalObject(); 

    nsIScriptContext *scriptContext = globalObject->GetContext();
//    NS_ENSURE_TRUE(scriptContext, PR_TRUE);

    JSContext* cx = (JSContext *) scriptContext->GetNativeContext();
//    NS_ENSURE_TRUE(cx, PR_TRUE); 

fprintf(stderr, "JSContext: %p\n", cx);fflush(stderr);

    const char *script = CHAR(STRING_ELT(r_script, 0)) ;

    jsval value;
    JSBool status;
    const char *file = CHAR(STRING_ELT(r_file, 0));
    int lineNum = INTEGER(r_line)[0];
    fprintf(stderr, "About to evaluate Javascript: %s (%s %d)\n",
            script, file, lineNum); fflush(stderr);
    status = JS_EvaluateScript(cx, js_obj, script, strlen(script),
                               file, lineNum, &value);

    fprintf(stderr, "Evaluated the JS script");
    return(R_NilValue);
}




/* http://www.mail-archive.com/mozilla-embedding@mozilla.org/msg00338.html */


#include "nsPIDOMWindow.h"

#if 1
// #include "nsDependentString.h"



nsIScriptContext * GetContextFromDocument(nsIDOMDocument * pDoc) 
{ 
    nsCOMPtr<nsIDocument> aDocument(do_QueryInterface(pDoc)); 
    nsIScriptGlobalObject * global = NULL; 
    if (aDocument == NULL) 
        return NULL; 

    global = aDocument->GetScriptGlobalObject(); 
    if (global == NULL) 
        return NULL; 

    nsIScriptContext * pContext = global->GetContext(); 
    return pContext; 
}

extern "C"
SEXP 
R_embed_evalJSScript(SEXP r_window, SEXP r_script, SEXP r_file, SEXP r_line)
{
    nsresult rv;
    // "@mozilla.org/scriptsecuritymanager;1"
    fprintf(stderr, "Getting security manager\n");fflush(stderr);
    nsCOMPtr<nsIScriptSecurityManager> pSecMan(do_GetService(NS_SCRIPTSECURITYMANAGER_CONTRACTID, &rv)); 
    if(NS_FAILED(rv) || pSecMan == NULL) {
        PROBLEM "Can't get security manager"
            ERROR;
    }
    fprintf(stderr, "Got security manager\n");fflush(stderr);
    nsCOMPtr<nsIPrincipal> principal; 
    rv = pSecMan->GetSystemPrincipal(getter_AddRefs(principal)); 
    if(principal == NULL) {
        PROBLEM "Can't get system principal"
            ERROR;
    }
    fprintf(stderr, "Got principal\n");fflush(stderr);



#if 0
    nsPIDOMWindow *win;
    win = R_GET_NS_ITYPE(r_window, nsPIDOMWindow);
    nsCOMPtr<nsIScriptGlobalObject> scriptObj(do_QueryInterface(win));
//    win->QueryInterface( NS_GET_IID(nsIScriptGlobalObject), (void **) &scriptObj);
    fprintf(stderr, "GlobalObject: %p\n", (void*) scriptObj);fflush(stderr);
    nsIScriptContext *ctxt;
    ctxt = scriptObj->GetContext();
#else
    nsIDOMDocument *doc;
    doc = R_GET_NS_ITYPE(r_window, nsIDOMDocument);
    nsIScriptContext * ctxt = GetContextFromDocument(doc);
    if(!ctxt) {
        fprintf(stderr, "Couldn't get context\n"); fflush(stderr);
        return(ScalarLogical(0));
    }
#endif


    fprintf(stderr, "Context: %p\n", ctxt);fflush(stderr);
    PRBool isUndef;
    nsAString *value;

 Rf_PrintValue(r_script);
//    nsTDependentString_CharT script(CHAR(STRING_ELT(r_script, 0)), strlen(CHAR(STRING_ELT(r_script, 0))));
    nsString script((PRUnichar *) CHAR(STRING_ELT(r_script, 0)), strlen(CHAR(STRING_ELT(r_script, 0))));

    nsresult status;
    status = ctxt->EvaluateString(script, (void *) nsnull, principal, CHAR(STRING_ELT(r_file, 0)),  
                         (PRUint32) INTEGER(r_line)[0], (PRUint32) 1, value, &isUndef);

    if(NS_FAILED(status)) {
        fprintf(stderr, "Failed to evaluate the JavaScript script %d\n", (int) status);
    } else {
        fprintf(stderr, "success in evaluating the JavaScript script %d\n", (int) status);
    }

    return(ScalarLogical(1));
}
#endif


#if 0

#include "nsIHTMLDocument.h"

extern "C"
SEXP
R_nsIDocument_GetIHTMLDocument(SEXP r_doc)
{
    nsIDocument *doc;
    doc = R_GET_NSIDOCUMENT(r_doc);
    nsIHTMLDocument *hdoc;
    nsresult status;
    status = doc->QueryInterface(NS_IHTMLDOCUMENT_IID, &hdoc);
    if(NS_FAILED(status)) {
        PROBLEM "Can't get IHTMLDocument from nsIDocument"
            ERROR;
    }
    fprintf(stderr, "Number of forms: %d\n", (int) hdoc->GetNumFormsSynchronous()); fflush(stderr);
    return(ScalarLogical(TRUE));
}

#endif

