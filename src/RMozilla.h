#ifndef __RMOZILLA_H
#define __RMOZILLA_H
#include <Rinternals.h>
#include <Rdefines.h>
#include <R.h>


#define PROB(a) \
     PROBLEM a \
      ERROR;


#define R_GET_NSIDOCUMENT(obj) \
    (nsIDocument *) R_ExternalPtrAddr(GET_SLOT((obj), Rf_install("ref")))

#define R_GET_NS_ITYPE(obj, type)           \
    (type *) R_ExternalPtrAddr(GET_SLOT((obj), Rf_install("ref")))

#define R_NS_IF_RELEASE(obj) \
  if((nsISupports *) R_ExternalPtrAddr(obj)) ((nsISupports *) R_ExternalPtrAddr(obj))->Release()


extern "C"
void R_nsISupportsFinalizer(SEXP);
#endif
