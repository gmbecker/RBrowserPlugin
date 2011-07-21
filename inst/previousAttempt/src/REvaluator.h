#include "IREvaluator.h"

#include "Rinternals.h"

class REvaluatorImpl : public IREvaluator
{

public:
    REvaluatorImpl();

    NS_DECL_ISUPPORTS;

    NS_DECL_IREVALUATOR;

private:
    ~REvaluatorImpl();
};
