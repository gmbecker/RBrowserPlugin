#ifndef RXPCOM_H
#define RXPCOM_H

//need this for my workaround for the symbol collision issue
#define USE_RINTERNALS

//#include <stdlib.h> //added
//#include "jemalloc.h"
#include "xpcom-config.h" //added
#include "RevaluatorImpl.h"
#include "nsIVariant.h" //Added the I. 

#include <Rdefines.h>
#include <Rinternals.h>
#include <Rembedded.h>
#include "jsapi.h"

typedef bool boolean;

SEXP convertVariantToR(nsIVariant * var);
boolean convertRToVariant(SEXP ans, nsIVariant **_returnValue);

typedef int*(*R_INT_PTR)(SEXP);

JSContext *GetContextForR(int add);
//extern R_INT_PTR Rf__INTEGER;
#define Rf__INTEGER(x) ((int *) DATAPTR(x))
#endif

