#include "WebR.h"

#include <R_ext/GraphicsEngine.h>
#include <R_ext/GraphicsDevice.h>
//#include <Rdevices.h>
#include <Rinternals.h>

extern "C"{
SEXP
R_NPAPI_Remove_All(SEXP plug, SEXP mylist)
{
  int n = LENGTH(mylist);
  SEXP tmp;
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  bool retbool;
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));
  PROTECT(tmp = R_NilValue);
  NPVariant *obj;
  NPVariant *ret = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  bool success; 
  for(int i=0; i< n; i++)
    {
      obj = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( VECTOR_ELT(mylist, i) , Rf_install( "ref" ) ) );
      success = funcs->invoke(inst, obj->value.objectValue, funcs->getstringidentifier("remove"), NULL, 0, ret);
      if(!success)
	{
	  Rf_error("Call to remove failed.");
	}
    }
  funcs->memfree(ret);
  UNPROTECT(1);
  return(ScalarLogical(success));
}

};
