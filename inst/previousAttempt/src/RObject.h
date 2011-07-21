#include "IRObject.h"

#include "Rinternals.h"

class RObjectImpl : public IRObject
{

public:
    RObjectImpl(SEXP val) { obj(val); };

    NS_DECL_ISUPPORTS;

    NS_DECL_IROBJECT;

    SEXP obj() { return(_obj);}
    SEXP obj(SEXP val) { _obj = val;
	                 R_PreserveObject(_obj);
                         return(_obj);
                       }

protected:
    SEXP callUnary(const char * const funName);

protected:
    ~RObjectImpl();

    SEXP _obj;
};
