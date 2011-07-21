#define DONT_LOAD_STRING_ENUMERATOR
#include "Moz191Compat.h"
#include "nsXPCOMCID.h" 
#include "nsXPCOM.h" 

#include "nsIServiceManager.h"
/*
#include "xpcom/nsIInterfaceInfoManager.h"
#include "xpcom/nsIInterfaceInfo.h"
#include "xpcom/nsIInterfaceRequestorUtils.h"

#include "xpcom/xptinfo.h"
*/
#include "RMozilla.h"


static SEXP R_int_makeInterface(nsIInterfaceInfo *iface);
static SEXP R_int_getMethodsInfo(nsIInterfaceInfo *iface);
static SEXP R_int_getConstantsInfo(nsIInterfaceInfo *iface);

extern "C"
SEXP
R_getInterfaces()
{
    nsresult rv;
    nsCOMPtr<nsIInterfaceInfoManager> imgr;
#if 0
/* Where is this declaration? Not in xul-runner SDK.*/
    rv = XPTI_GetInterfaceInfoManager(getter_AddRefs(imgr));
#endif
    nsCOMPtr<nsIServiceManager> servMan; 
    rv = NS_GetServiceManager(getter_AddRefs(servMan)); 

    rv = servMan->GetServiceByContractID("@mozilla.org/xpti/interfaceinfomanager-service;1",
                                         NS_GET_IID(nsIInterfaceInfoManager), getter_AddRefs(imgr)); 

    nsCOMPtr<nsIEnumerator> enumerator;
    rv = imgr->EnumerateInterfaces(getter_AddRefs(enumerator));

    int i, n = -1;
    nsCOMPtr<nsISupports> entry;
    rv = enumerator->First();
    while(NS_SUCCEEDED(enumerator->Next()))
        n++;

    rv = imgr->EnumerateInterfaces(getter_AddRefs(enumerator));

    SEXP ans, names, rtmp = R_NilValue;
    PROTECT(ans = NEW_LIST(n));
    PROTECT(names = NEW_CHARACTER(n));

    i = n = 0;

    rv = enumerator->First();
    while(NS_SUCCEEDED(enumerator->Next())) {
        nsISupports *ientry;
        rv = enumerator->CurrentItem(&ientry);

        nsCOMPtr<nsIInterfaceInfo> iface = do_QueryInterface(ientry);
//        const nsGetInterface iface = do_GetInterface(ientry, &rv);
        char *tmp;
        rv = iface->GetName(&tmp);
        if(tmp)
            SET_STRING_ELT(names, i, mkChar(tmp));
        rtmp = R_int_makeInterface(iface);
        SET_VECTOR_ELT(ans, i, rtmp);
    }

    SET_NAMES(ans, names);
    UNPROTECT(2);
    return(ans);
}


static SEXP 
R_int_makeInterface(nsIInterfaceInfo *iface)
{
    SEXP klass, ans;
    char *tmp;
    PROTECT(klass = MAKE_CLASS("XPCOMInterfaceInfo"));
    PROTECT(ans = NEW(klass));

    iface->GetName(&tmp);
    SET_SLOT(ans, Rf_install("name"), mkString(tmp));

    nsIInterfaceInfo *parent;
    iface->GetParent(&parent);
    if(parent) {
        parent->GetName(&tmp);
        SET_SLOT(ans, Rf_install("parent"), mkString(tmp));
    }

    nsIID *iid;
    iface->GetInterfaceIID(&iid);
    if(iid) {
#ifdef XPCOM_GLUE_USE_NSPR
 // Can use this if we allow dependency on nspr. Specifcally XPCOM_GLUE_USE_NSPR
        tmp = iid->ToString();
        SET_SLOT(ans, Rf_install("iid"), mkString(tmp));
#endif
    }

    PRBool b;
    iface->IsScriptable(&b);
    SET_SLOT(ans, Rf_install("isScriptable"), ScalarLogical(b));
    iface->IsFunction(&b);
    SET_SLOT(ans, Rf_install("isFunction"), ScalarLogical(b));

    SET_SLOT(ans, Rf_install("methods"), R_int_getMethodsInfo(iface));
    SET_SLOT(ans, Rf_install("constants"), R_int_getConstantsInfo(iface));

    UNPROTECT(2);

    return(ans);
}

static SEXP
R_int_makeMethodInfo(const nsXPTMethodInfo *info)
{
    return(R_NilValue);
}

static SEXP 
R_int_getMethodsInfo(nsIInterfaceInfo *iface)
{
    PRUint16 n;
    nsresult rv;
    rv = iface->GetMethodCount(&n);
    
    const nsXPTMethodInfo *info;
 
    SEXP ans, names;
    int i;
    PROTECT(ans = NEW_LIST(n));
    PROTECT(names = NEW_CHARACTER(n));

    for(i = 0 ; i < n; i++) {
       rv = iface->GetMethodInfo(i, &info);
       if(NS_SUCCEEDED(rv)) {
           SET_VECTOR_ELT(ans, i, R_int_makeMethodInfo(info));
           SET_STRING_ELT(names, i, mkChar(info->GetName()));
       }
    }

    SET_NAMES(ans, names);
    UNPROTECT(2);
    return(ans);
}

//XXX
SEXP
XPTTypeToR(nsXPTType type)
{
    return(R_NilValue);
}

//XXX
SEXP
XPTVariantToR(const nsXPTCMiniVariant* val)
{
    return(R_NilValue);
}

static SEXP
R_int_makeConstantInfo(const nsXPTConstant *info)
{
    SEXP klass, ans;
    PROTECT(klass = MAKE_CLASS("XPTConstant"));
    PROTECT(ans = NEW(klass));

    SET_SLOT(ans, Rf_install("name"), mkString(info->GetName()));
    SET_SLOT(ans, Rf_install("type"), XPTTypeToR(info->GetType()));
    SET_SLOT(ans, Rf_install("value"), XPTVariantToR(info->GetValue()));

    UNPROTECT(2);
    return(ans);

}

static SEXP 
R_int_getConstantsInfo(nsIInterfaceInfo *iface)
{
    nsresult rv;
    PRUint16 n;

    iface->GetConstantCount(&n);
    
    const nsXPTConstant *info;
    SEXP ans, names;
    int i;
    PROTECT(ans = NEW_LIST(n));
    PROTECT(names = NEW_CHARACTER(n));

    for(i = 0 ; i < n; i++) {
       rv = iface->GetConstant(i, &info);
       if(NS_SUCCEEDED(rv)) {
           SET_VECTOR_ELT(ans, i, R_int_makeConstantInfo(info));
           SET_STRING_ELT(names, i, mkChar(info->GetName()));
       }
    }

    SET_NAMES(ans, names);
    UNPROTECT(2);
    return(ans);
}
