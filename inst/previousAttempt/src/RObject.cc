#include "Rinternals.h"
#include "RObject.h"

#include "nsMemory.h"

NS_IMPL_ISUPPORTS1(RObjectImpl, IRObject)


SEXP
RObjectImpl::callUnary(const char * const funName)
{
    SEXP e, ans = NULL;
    int errorOccurred = 0;

    PROTECT(e = allocVector(LANGSXP, 2));
    SETCAR(e, Rf_install(funName));
    SETCAR(CDR(e), _obj);
    ans = R_tryEval(e, R_GlobalEnv, &errorOccurred);
    UNPROTECT(1);
    if(errorOccurred) 
        ans = NULL;
    return(ans);
}


RObjectImpl::~RObjectImpl()
{
    R_ReleaseObject(_obj);
}

#include "ObjectMethods.h"
ROBJECT_METHODS(RObjectImpl)






