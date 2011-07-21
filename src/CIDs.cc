#include "nsXPCOMCID.h" 
#include "nsXPCOM.h" 

#include "nsIComponentRegistrar.h" 
#include "nsComponentManagerUtils.h"
#include "nsICategoryManager.h" 

#include "nsIServiceManager.h"

#include "nsISimpleEnumerator.h"
#include "nsISupportsPrimitives.h"
#include "nsStringAPI.h"

#include "Moz191Compat.h"

#include "RMozilla.h"


/*
 Do this as a method of the ComponentRegistrar.
 Return the 
 */
extern "C"
SEXP
R_getRegisterCIDs()
{

    nsCOMPtr<nsIComponentRegistrar> registrar; 
    nsresult rv = NS_GetComponentRegistrar(getter_AddRefs(registrar)); 
    if (NS_FAILED(rv))  {
        PROB("couldn't get ComponentRegistrar");
    }

#if 1
    nsCOMPtr<nsISimpleEnumerator> enumerator;
#else
    nsSimpleEnumerator enumerator;
#endif
    rv = registrar->EnumerateCIDs(getter_AddRefs(enumerator));
    if (NS_FAILED(rv))  {
        PROB("couldn't get EnumerateCIDs");
    }

    int i, n = 0;
    PRBool b;

    nsCOMPtr<nsISupports> entry;

    while(NS_SUCCEEDED(enumerator->GetNext(getter_AddRefs(entry))))
        n++;

    SEXP ans, names;
    PROTECT(ans = NEW_CHARACTER(n));
    PROTECT(names = NEW_CHARACTER(n));


    rv = registrar->EnumerateCIDs(getter_AddRefs(enumerator));
    if (NS_FAILED(rv))  {
        PROB("couldn't get EnumerateCIDs");
    }

    i = 0;
    while(NS_SUCCEEDED(enumerator->GetNext(getter_AddRefs(entry)))) {
        char *tmp;
        nsCOMPtr<nsISupportsID> istring(do_QueryInterface(entry));
        if(!istring) {
            PROBLEM "entry doesn't support nsID"
             WARN;
             continue;
        }

        istring->ToString(&tmp);
        if(NS_FAILED(rv)) {
            PROBLEM "can't convert ID to string"
             WARN;
            continue;
        }
        SET_STRING_ELT(ans, i, mkChar(tmp));

#if 0
        nsCOMPtr<nsISupportsCID> cidstring(do_QueryInterface(entry));
#else
        nsID *id;
        rv = istring->GetData(&id);
        if(NS_FAILED(rv)) 
            continue;
        nsCID cid(*id);
#endif

        //XXX This is not working.

        rv = registrar->CIDToContractID(cid, &tmp);
        if(NS_FAILED(rv)) {
            PROBLEM "can't convert ID (%s) to ContractID", tmp
             WARN;
            continue;
        }
        SET_STRING_ELT(names, i, mkChar(tmp));
        i++;
    }


#if 0
    for(n = 0; NS_SUCCEEDED(enumerator->HasMoreElements(&b) && b); n++) {
         ;
    }
#endif
    SET_NAMES(ans, names);

    UNPROTECT(2);
    return(ans);
}


SEXP
R_int_getCategoryEntries(const char *category, nsICategoryManager *categoryManager)
{
    nsCOMPtr<nsISimpleEnumerator> enumerator;
    categoryManager->EnumerateCategory(category, getter_AddRefs(enumerator));

    int n = 0;
    nsCOMPtr<nsISupports> entry;
    while(NS_SUCCEEDED(enumerator->GetNext(getter_AddRefs(entry))))
        n++;

    SEXP ans;
    PROTECT(ans = NEW_CHARACTER(n));
    categoryManager->EnumerateCategory(category, getter_AddRefs(enumerator));
    n = 0;

    nsCOMPtr<nsIUTF8StringEnumerator> ienum(do_QueryInterface(enumerator));
    nsCString ientry; // Not nsACString.
    while(NS_SUCCEEDED(ienum->GetNext(ientry))) {
        char *tmp;
        tmp = ToNewCString(ientry);
        SET_STRING_ELT(ans, n, mkChar(tmp));
        n++;
    }
    UNPROTECT(1);

    return(ans);
}

/*
 Returns either the names of the categories or a list
 with the names of the entries in each category.
 This is controlled by r_deep which needs to be a logical
 and we get the character vector if this is FALSE and the list
 if it is TRUE.
 */

extern "C"
SEXP
R_getCategories(SEXP r_deep)
{

    PRBool deep = INTEGER(r_deep)[0];

    nsCOMPtr<nsIServiceManager> servMan; 
    nsresult rv = NS_GetServiceManager(getter_AddRefs(servMan)); 

    nsCOMPtr<nsICategoryManager> categoryManager; 
    rv = servMan->GetServiceByContractID("@mozilla.org/categorymanager;1", 
                                         NS_GET_IID(nsICategoryManager), getter_AddRefs(categoryManager)); 

    if (NS_FAILED(rv))  {
        PROB("couldn't get CategoryManager");
    }


    nsCOMPtr<nsISimpleEnumerator> enumerator;
    rv = categoryManager->EnumerateCategories(getter_AddRefs(enumerator));
    if (NS_FAILED(rv))  {
        PROB("couldn't get EnumerateCategories");
    }

    int i, n = 0;

    nsCOMPtr<nsISupports> entry;
    while(NS_SUCCEEDED(enumerator->GetNext(getter_AddRefs(entry))))
        n++;


    SEXP ans, names;
    PROTECT(names = NEW_CHARACTER(n));
    if(deep)
        PROTECT(ans = NEW_LIST(n));

    while(NS_SUCCEEDED(enumerator->GetNext(getter_AddRefs(entry))))
        n++;


    nsCOMPtr<nsIUTF8StringEnumerator> ienum(do_QueryInterface(enumerator));
    nsCString ientry; // Not nsACString.

    i = 0;
    while(NS_SUCCEEDED(ienum->GetNext(ientry))) {
        char *tmp;
        tmp = ToNewCString(ientry);
        if(tmp)
            SET_STRING_ELT(names, i, mkChar(tmp));

        if(deep) {
            SET_VECTOR_ELT(ans, i, R_int_getCategoryEntries(tmp, categoryManager));
        }
        i++;
    }

    if(deep)  {
        SET_NAMES(ans, names);
        UNPROTECT(2);
        return(ans);
    } else {
        UNPROTECT(1);
        return(names);
    }
}
