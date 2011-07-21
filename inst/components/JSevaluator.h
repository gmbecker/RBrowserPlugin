/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM JSevaluator.idl
 */

#ifndef __gen_JSevaluator_h__
#define __gen_JSevaluator_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_nsIVariant_h__
#include "nsIVariant.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    JSevaluator */
#define JSEVALUATOR_IID_STR "1214e30c-6c09-4dc5-bd13-f46f72924fb8"

#define JSEVALUATOR_IID \
  {0x1214e30c, 0x6c09, 0x4dc5, \
    { 0xbd, 0x13, 0xf4, 0x6f, 0x72, 0x92, 0x4f, 0xb8 }}

class NS_NO_VTABLE NS_SCRIPTABLE JSevaluator : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(JSEVALUATOR_IID)

  /* nsIVariant eval (in string code); */
  NS_SCRIPTABLE NS_IMETHOD Eval(const char *code, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant callFun (in string funName, [array, size_is (numArgs)] in nsIVariant args, in unsigned long numArgs); */
  NS_SCRIPTABLE NS_IMETHOD CallFun(const char *funName, nsIVariant **args, PRUint32 numArgs, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant assign (in string varname, in nsIVariant val); */
  NS_SCRIPTABLE NS_IMETHOD Assign(const char *varname, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant get (in string varname); */
  NS_SCRIPTABLE NS_IMETHOD Get(const char *varname, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant setPropertyWithValue (in nsIVariant obj, in string prop, in nsIVariant val); */
  NS_SCRIPTABLE NS_IMETHOD SetPropertyWithValue(nsIVariant *obj, const char *prop, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant setPropertyWithVar (in nsIVariant obj, in string prop, in string val); */
  NS_SCRIPTABLE NS_IMETHOD SetPropertyWithVar(nsIVariant *obj, const char *prop, const char *val, nsIVariant **_retval NS_OUTPARAM) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(JSevaluator, JSEVALUATOR_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_JSEVALUATOR \
  NS_SCRIPTABLE NS_IMETHOD Eval(const char *code, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD CallFun(const char *funName, nsIVariant **args, PRUint32 numArgs, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Assign(const char *varname, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Get(const char *varname, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD SetPropertyWithValue(nsIVariant *obj, const char *prop, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD SetPropertyWithVar(nsIVariant *obj, const char *prop, const char *val, nsIVariant **_retval NS_OUTPARAM); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_JSEVALUATOR(_to) \
  NS_SCRIPTABLE NS_IMETHOD Eval(const char *code, nsIVariant **_retval NS_OUTPARAM) { return _to Eval(code, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD CallFun(const char *funName, nsIVariant **args, PRUint32 numArgs, nsIVariant **_retval NS_OUTPARAM) { return _to CallFun(funName, args, numArgs, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Assign(const char *varname, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM) { return _to Assign(varname, val, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Get(const char *varname, nsIVariant **_retval NS_OUTPARAM) { return _to Get(varname, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetPropertyWithValue(nsIVariant *obj, const char *prop, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM) { return _to SetPropertyWithValue(obj, prop, val, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetPropertyWithVar(nsIVariant *obj, const char *prop, const char *val, nsIVariant **_retval NS_OUTPARAM) { return _to SetPropertyWithVar(obj, prop, val, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_JSEVALUATOR(_to) \
  NS_SCRIPTABLE NS_IMETHOD Eval(const char *code, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Eval(code, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD CallFun(const char *funName, nsIVariant **args, PRUint32 numArgs, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->CallFun(funName, args, numArgs, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Assign(const char *varname, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Assign(varname, val, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Get(const char *varname, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Get(varname, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetPropertyWithValue(nsIVariant *obj, const char *prop, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPropertyWithValue(obj, prop, val, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetPropertyWithVar(nsIVariant *obj, const char *prop, const char *val, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPropertyWithVar(obj, prop, val, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public JSevaluator
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_JSEVALUATOR

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, JSevaluator)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* nsIVariant eval (in string code); */
NS_IMETHODIMP _MYCLASS_::Eval(const char *code, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant callFun (in string funName, [array, size_is (numArgs)] in nsIVariant args, in unsigned long numArgs); */
NS_IMETHODIMP _MYCLASS_::CallFun(const char *funName, nsIVariant **args, PRUint32 numArgs, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant assign (in string varname, in nsIVariant val); */
NS_IMETHODIMP _MYCLASS_::Assign(const char *varname, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant get (in string varname); */
NS_IMETHODIMP _MYCLASS_::Get(const char *varname, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant setPropertyWithValue (in nsIVariant obj, in string prop, in nsIVariant val); */
NS_IMETHODIMP _MYCLASS_::SetPropertyWithValue(nsIVariant *obj, const char *prop, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant setPropertyWithVar (in nsIVariant obj, in string prop, in string val); */
NS_IMETHODIMP _MYCLASS_::SetPropertyWithVar(nsIVariant *obj, const char *prop, const char *val, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_JSevaluator_h__ */
