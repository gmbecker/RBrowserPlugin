/*

 */
#include "xpcom-config.h"
//#include "xpcom/nscore.h"
#include "nsXPCOM.h"

#include "nsEmbedString.h" 

/*
#include "nsEmbedAPI.h"
#include "nspr/prtypes.h"
#include "nsEmbedCID.h" 
#include "nsComponentManagerUtils.h"
*/

//#include "xpcom/nsIInterfaceInfoManager.h"
//#include "xpcom/xptinfo.h"
//#include "xpcom/xptcall.h"
#include "Moz191Compat.h"
#include "RMozilla.h"

#include "xptVariants.h"

extern "C"
SEXP
// SEXP r_args, SEXP r_inout, SEXP r_types,
R_XPT_InvokeByIndex(SEXP r_obj, SEXP methodIndex, SEXP r_variants, SEXP r_outIndices)
{
    int numArgs, methodId;

    nsXPTCVariant *args;
    numArgs = Rf_length(r_variants);
    args = (nsXPTCVariant *) R_alloc(sizeof(nsXPTCVariant), numArgs);
    for(int i = 0; i < numArgs; i++) {
        args[i] = * ( (nsXPTCVariant *) R_ExternalPtrAddr(GET_SLOT(VECTOR_ELT(r_variants, i), Rf_install("ref"))));
    }


    nsresult rv;
    nsISupports *obj;
    
    obj = (nsISupports *) R_ExternalPtrAddr(r_obj);

    methodId = INTEGER(methodIndex)[0];

    rv = NS_InvokeByIndex(obj, methodId, numArgs, args);

    if(NS_FAILED(rv)) {
        PROBLEM "failed to invoke the method %d", methodId
         ERROR;
    }

    /* Convert the out answers to R objets.*/
    SEXP ans = R_NilValue;
    int numOuts = Rf_length(r_outIndices);
    PROTECT(ans = NEW_LIST(numOuts));

    for(int i = 0; i < numOuts; i++) {
        int offset = INTEGER(r_outIndices)[i];
        SET_VECTOR_ELT(ans, i, convertVariantToR(&args[ offset ]));
    }

    UNPROTECT(1);
    return(ans);
}

