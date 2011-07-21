#include "nsXPTCUtils.h"
#include "nsMemory.h"


#include "Moz191Compat.h"

/* Order is important. The two above here need to be first as we redefine Realloc() in R. */

#include <Rdefines.h>
#include <Rinternals.h>

#define R_GET_NSISUPPORTS(obj) \
    (nsISupports *) R_ExternalPtrAddr(GET_SLOT((obj), Rf_install("ref")))


SEXP
R_as_nsType(SEXP r_obj, nsIID iid, const char * className)
{
    nsresult nr;
    nsISupports *ref;
    nsIDocument *doc;
    SEXP ans, klass;

    ref = R_GET_NSISUPPORTS(r_obj);

    nr = ref->QueryInterface(iid, (void **) &doc);
    if(NS_FAILED(nr)) 
        return(R_NilValue);

    NS_ADDREF(doc);
    PROTECT(klass = MAKE_CLASS(className));
    PROTECT(ans = NEW(klass));
    SET_SLOT(ans, Rf_install("ref"), R_MakeExternalPtr(doc, Rf_install(className), R_NilValue));
    UNPROTECT(2);
    return(ans);
}

/* Bring a generic JavaScript reference to an R object of class nsIDocument or nsINode*/

extern "C"
SEXP
R_as_nsIDocument(SEXP r_obj)
{
    return(R_as_nsType(r_obj, NS_GET_IID(nsIDocument), "nsIDocument"));
}

extern "C"
SEXP
R_as_nsINode(SEXP r_obj)
{
    return(R_as_nsType(r_obj, NS_GET_IID(nsINode), "nsINode"));
}


#if 0

  fprintf(stderr, "QueryInterface for nsIDocument %s\n", NS_FAILED(nr) ? "failed" : "succeeded");fflush(stderr);       
       nsINode *node;
       nr = ref->QueryInterface(NS_GET_IID(nsINode), (void **) &node);
  fprintf(stderr, "QueryInterface for nsINode %s\n", NS_FAILED(nr) ? "failed" : "succeeded");fflush(stderr);       
#endif
