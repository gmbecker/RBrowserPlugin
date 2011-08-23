#include <jsapi.h>
#include <R.h>
#include <Rdefines.h>


#ifndef ROBJ_H
#define ROBJ_H

JSClass Rfun = {
  "RFunction",
  JSCLASS_HAS_PRIVATE, //objects will have a pointer to private data.

  //No properties right now, plan is to store the SEXP itself as private data
  //all we need is the ability to pass it up to javascript with the intent of passing back down to R
  //and the ability to identify that it is an RFunction js object when we do.  
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
  JS_StrictPropertyStub, JS_EnumerateStub, JS_ResolveStub, 
  JS_ConvertStub, JS_FinalizeStub
};

JSClass Robj = {
  "RObject",
  JSCLASS_HAS_PRIVATE, //objects will have a pointer to private data.

  //No properties right now, plan is to store the SEXP itself as private data
  //all we need is the ability to pass it up to javascript with the intent of passing back down to R
  //and the ability to identify that it is an RFunction js object when we do.  
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
  JS_StrictPropertyStub, JS_EnumerateStub, JS_ResolveStub, 
  JS_ConvertStub, JS_FinalizeStub
};
  /*
   my_addProperty, my_delProperty, my_getProperty, my_setProperty,
    my_enumerate,   my_resolve,     my_convert,     my_finalize


  */

#endif
