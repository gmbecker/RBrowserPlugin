#include "RObject.h"
#include "IRFunction.h"

#include "Rinternals.h"

class RFunctionImpl : public RObjectImpl, public IRFunction 
{
public:
    RFunctionImpl(SEXP val);
    RFunctionImpl(const char * funName);

    NS_DECL_ISUPPORTS
#ifdef CANT_USE_INHERITANCE
    NS_DECL_IROBJECT
#endif
    NS_DECL_IRFUNCTION 

/*    NS_FORWARD_IRFUNCTION(RObjectImpl::) */

#if 0
private:
    ~RFunctionImpl() {}
#endif
};
