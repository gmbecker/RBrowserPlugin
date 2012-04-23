/*
#include "npruntime.h"
#include "npfunctions.h"
#include "npapi.h"
#include "Rdefines.h"
*/
#include "WebR.h"

bool C_doTest(NPP inst, NPNetscapeFuncs *funcs);
extern "C" {
  SEXP R_doTest(SEXP plug);
  //success = funcs->invoke(inst, domwin, arrid, NULL, 0, vartmp2);
  SEXP R_NPAPI_Invoke(SEXP plug, SEXP Robj, SEXP Rname, SEXP Rargs, SEXP RconvArgs, SEXP RconvRet, SEXP RkeepRes );

  SEXP R_doRefClassTest();
  SEXP R_NP_GetGlobal(SEXP Rplug);
  SEXP R_NPAPI_GetProperty(SEXP plug, SEXP Robj, SEXP Rname, SEXP RconvRet);
  SEXP R_NPAPI_SetProperty(SEXP plug, SEXP Robj, SEXP Rname, SEXP Rval, SEXP RconvValue);
}

SEXP R_doRefClasWsTest()
{
  SEXP ans, klass;
  PROTECT(klass = MAKE_CLASS("MyTestRef"));
  PROTECT(ans = NEW_OBJECT(klass));
  Rf_PrintValue(ans);
  UNPROTECT(2);
  return ans;
}

SEXP R_NPAPI_Invoke(SEXP plug, SEXP Robj, SEXP Rname, SEXP Rargs, SEXP RconvArgs, SEXP RconvRet, SEXP RkeepRes )
{
 
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));

  NPVariant *obj = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( Robj , Rf_install( "ref" ) ) );
  
  if(!NPVARIANT_IS_OBJECT(*obj))
    {
      //What should we return in this case?
      Rf_error("Robj is not an NPVariant containing an NPObject.");
      return R_NilValue;
    }
  int convRet = LOGICAL(RconvRet)[0];
  
  
  int nargs = LENGTH(Rargs);
  NPVariant *args = (NPVariant *) funcs->memalloc(nargs*sizeof(NPVariant));
  
  for(int i = 0; i < nargs; i++)
    {
      ConvertRToNP(VECTOR_ELT(Rargs, i), inst, funcs, &(args[i]), LOGICAL(RconvArgs)[i]);
    }

  NPVariant *ret = (NPVariant *) funcs->memalloc(sizeof(NPVariant)); 
  
  const char *ccname = CHAR(STRING_ELT(Rname, 0));

  bool success = funcs->invoke(inst, obj->value.objectValue, funcs->getstringidentifier(ccname), args, nargs, ret);

  for(int j=0; j<nargs; j++)
    {
            if (NPVARIANT_IS_OBJECT(args[j]))
	      funcs->releaseobject(args[j].value.objectValue);
	    //funcs->releasevariantvalue(&args[j]);
    }
  funcs->memfree(args);
  
  if(!success)
    {
      Rf_error("Invoke failed.");
      return R_NilValue;
    }
    
  SEXP ans;
  PROTECT(ans = R_NilValue);
  bool canfree = ConvertNPToR(ret, inst, funcs, convRet, &ans);
  bool keepRes = LOGICAL(RkeepRes)[0];
  if(canfree || !keepRes)
    funcs->releasevariantvalue(ret);
  
  UNPROTECT(1);
  if(keepRes)
    return ans ;
  else
    return R_NilValue;
}



SEXP R_NPAPI_GetProperty(SEXP plug, SEXP Robj, SEXP Rname, SEXP RconvRet)
{
 
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));

  NPVariant *obj = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( Robj , Rf_install( "ref" ) ) );
  
  if(!NPVARIANT_IS_OBJECT(*obj))
    {
      //What should we return in this case?
      Rf_error("Robj is not an NPVariant containing an NPObject.");
      return R_NilValue;
    }
  int convRet = LOGICAL(RconvRet)[0];


  NPVariant *ret = (NPVariant *) funcs->memalloc(sizeof(NPVariant)); 
  
  const char *ccname = CHAR(STRING_ELT(Rname, 0));

  bool success = funcs->getproperty(inst, obj->value.objectValue, funcs->getstringidentifier(ccname), ret);
 
  if(!success)
    {
      Rf_error("Invoke failed.");
      return R_NilValue;
    }
    
  SEXP ans;
  PROTECT(ans = R_NilValue);
  //ConvertNPToR returns a bool which indicates whether it is safe to release the converted object.
  bool canfree = ConvertNPToR(ret, inst, funcs, convRet, &ans);

  if(canfree)
    funcs->releasevariantvalue(ret);
  
  UNPROTECT(1);
  
  return ans ;
}




SEXP R_NPAPI_SetProperty(SEXP plug, SEXP Robj, SEXP Rname, SEXP Rval, SEXP RconvValue)
{
 
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));

  NPVariant *obj = (NPVariant *) R_ExternalPtrAddr(GET_SLOT( Robj , Rf_install( "ref" ) ) );
  
  if(!NPVARIANT_IS_OBJECT(*obj))
    {
      //What should we return in this case?
      Rf_error("Robj is not an NPVariant containing an NPObject.");
      return R_NilValue;
    }
  int convVal = LOGICAL(RconvValue)[0];

  
  //NPVariant *val = (NPVariant *) funcs->memalloc(sizeof(NPVariant)); 
  NPVariant val;
  ConvertRToNP(Rval, inst, funcs, &val, convVal);
  const char *ccname = CHAR(STRING_ELT(Rname, 0));

  bool success = funcs->setproperty(inst, obj->value.objectValue, funcs->getstringidentifier(ccname), &val);
 
  if(!success)
    {
      //funcs->memfree(val);
      Rf_error("SetProperty failed.");
      return R_NilValue;
    }
  else {
    funcs->releasevariantvalue(&val);
    //funcs->memfree(val);
  }
  return ScalarLogical(success) ;
}


SEXP R_doTest(SEXP plug)
{
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( plug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(plug, Rf_install("funcs")), Rf_install("ref")));
  bool success = C_doTest(inst, funcs);
  return ScalarLogical(success);
}

bool C_doTest(NPP inst, NPNetscapeFuncs *funcs)
{

  NPObject *domwin = NULL;
 
  NPVariant *vartmp2 = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
    //NPVariant vartmp2;

  NPError res;
  bool success;
  res = funcs->getvalue(inst, NPNVWindowNPObject , &domwin);
  funcs->retainobject(domwin);
  NPIdentifier arrid = funcs->getstringidentifier("Array");
  
   
  success = funcs->invoke(inst, domwin, arrid, NULL, 0, vartmp2);
  fprintf(stderr, "\nIn doTest. success: %d", success); fflush(stderr);
  funcs->releaseobject(domwin);
  /*
  myNPNFuncs->memfree(vartmp3);
  myNPNFuncs->memfree(vartmp2);
  */  
  funcs->releasevariantvalue(vartmp2);
  return success;
}


SEXP R_NP_GetGlobal(SEXP Rplug)
{
  NPP inst = (NPP) R_ExternalPtrAddr(GET_SLOT( Rplug , Rf_install( "ref" ) ) );
  NPNetscapeFuncs *funcs = (NPNetscapeFuncs *) R_ExternalPtrAddr(GET_SLOT( GET_SLOT(Rplug, Rf_install("funcs")), Rf_install("ref")));

  NPObject *domwin = NULL;
  NPVariant *toret = (NPVariant *) funcs->memalloc(sizeof(NPVariant));
  //NPVariant vartmp2;
  
  NPError res;
  bool success;
  res = funcs->getvalue(inst, NPNVWindowNPObject , &domwin);
  funcs->retainobject(domwin);
  OBJECT_TO_NPVARIANT(domwin, *toret);
  
  SEXP ans;
  PROTECT(ans = R_NilValue);
  ConvertNPToR(toret, inst, funcs, false, &ans);
  UNPROTECT(1);
  return ans;
}

