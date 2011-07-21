#include "Rinternals.h"

#include "REvaluator.h"
#include "RFunction.h"

NS_IMPL_ISUPPORTS1(REvaluatorImpl, IREvaluator)

REvaluatorImpl::REvaluatorImpl()
{
// CallFun("library", mkString("RFirefox"))
}

REvaluatorImpl::~REvaluatorImpl()
{
}

NS_SCRIPTABLE NS_IMETHODIMP 
REvaluatorImpl::CallFun(const char *fun, IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM)
{
    RFunctionImpl rfun = RFunctionImpl(fun);
    return(rfun.Call(args, length, _retval));
}

/* RObject call (in RFunction fun, in RArgList args); */
NS_SCRIPTABLE NS_IMETHODIMP
REvaluatorImpl::Call(IRFunction *fun, IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM)
{
    return(fun->Call(args, length, _retval));
}
