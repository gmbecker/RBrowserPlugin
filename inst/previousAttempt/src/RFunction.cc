#include "Rinternals.h"

#include "RFunction.h"

NS_IMPL_ISUPPORTS1(RFunctionImpl, IRFunction)
/*
NS_IMPL_ISUPPORTS2(RFunctionImpl, IRFunction, IRObject)

NS_IMPL_ADDREF(RFunctionImpl);
NS_IMPL_RELEASE(RFunctionImpl);
NS_IMPL_QUERY_INTERFACE(RFunctionImpl, IRFunction)
*/



RFunctionImpl::RFunctionImpl(SEXP fun)
     : RObjectImpl(fun)
{
    callUnary("Sys.Date");
  /* member initializers and constructor code */
}

RFunctionImpl::RFunctionImpl(const char * fun)
    : RObjectImpl(Rf_install(fun))
{
  /* member initializers and constructor code */
}


/* RObject call (in RArgList args); */
NS_IMETHODIMP
RFunctionImpl::Call(IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM) 
{
    SEXP ans, e, ptr;
    int errorOccurred = 0, nargs = length, i;
    *_retval = NULL;

    PROTECT(ptr = e = allocVector(LANGSXP, nargs));
    SETCAR(ptr, _obj); ptr = CDR(ptr);
    for(i = 0 ; i < nargs; i++) {
        IRObject *imp;
        SEXP tmp;
        imp = args[i];
        imp->AsSEXP(&tmp);
        SETCAR(ptr, tmp);
        ptr = CDR(ptr);
    }

    ans = R_tryEval(e, R_GlobalEnv, &errorOccurred);
    if(errorOccurred)
        return(NS_ERROR_FAILURE);

    *_retval = new RObjectImpl(ans);

    return(NS_OK);
}

#ifdef CANT_USE_INHERITANCE
#include "ObjectMethods.h"
ROBJECT_METHODS(RFunctionImpl)
#endif


