/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM Revaluator.idl
 */

#ifndef __gen_Revaluator_h__
#define __gen_Revaluator_h__


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

/* starting interface:    REvaluator */
#define REVALUATOR_IID_STR "b69468e2-3894-4679-870b-42cec870001e"

#define REVALUATOR_IID \
  {0xb69468e2, 0x3894, 0x4679, \
    { 0x87, 0x0b, 0x42, 0xce, 0xc8, 0x70, 0x00, 0x1e }}

class NS_NO_VTABLE NS_SCRIPTABLE REvaluator : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(REVALUATOR_IID)

  /* boolean init ([array, size_is (length)] in string args, in unsigned long length); */
  NS_SCRIPTABLE NS_IMETHOD Init(const char **args, PRUint32 length, PRBool *_retval NS_OUTPARAM) = 0;

  /* nsIVariant call1 (in string funName, in nsIVariant arg); */
  NS_SCRIPTABLE NS_IMETHOD Call1(const char *funName, nsIVariant *arg, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant call (in string funName); */
  NS_SCRIPTABLE NS_IMETHOD Call(const char *funName, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant library (in string library); */
  NS_SCRIPTABLE NS_IMETHOD Library(const char *library, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant eval (in string cmd); */
  NS_SCRIPTABLE NS_IMETHOD Eval(const char *cmd, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant exists (in string var); */
  NS_SCRIPTABLE NS_IMETHOD Exists(const char *var, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant get (in string var); */
  NS_SCRIPTABLE NS_IMETHOD Get(const char *var, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant assign (in string var, in nsIVariant val); */
  NS_SCRIPTABLE NS_IMETHOD Assign(const char *var, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant remove (in string var); */
  NS_SCRIPTABLE NS_IMETHOD Remove(const char *var, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* readonly attribute string version; */
  NS_SCRIPTABLE NS_IMETHOD GetVersion(char * *aVersion) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(REvaluator, REVALUATOR_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_REVALUATOR \
  NS_SCRIPTABLE NS_IMETHOD Init(const char **args, PRUint32 length, PRBool *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Call1(const char *funName, nsIVariant *arg, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Call(const char *funName, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Library(const char *library, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Eval(const char *cmd, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Exists(const char *var, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Get(const char *var, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Assign(const char *var, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Remove(const char *var, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetVersion(char * *aVersion); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_REVALUATOR(_to) \
  NS_SCRIPTABLE NS_IMETHOD Init(const char **args, PRUint32 length, PRBool *_retval NS_OUTPARAM) { return _to Init(args, length, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Call1(const char *funName, nsIVariant *arg, nsIVariant **_retval NS_OUTPARAM) { return _to Call1(funName, arg, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Call(const char *funName, nsIVariant **_retval NS_OUTPARAM) { return _to Call(funName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Library(const char *library, nsIVariant **_retval NS_OUTPARAM) { return _to Library(library, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Eval(const char *cmd, nsIVariant **_retval NS_OUTPARAM) { return _to Eval(cmd, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Exists(const char *var, nsIVariant **_retval NS_OUTPARAM) { return _to Exists(var, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Get(const char *var, nsIVariant **_retval NS_OUTPARAM) { return _to Get(var, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Assign(const char *var, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM) { return _to Assign(var, val, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Remove(const char *var, nsIVariant **_retval NS_OUTPARAM) { return _to Remove(var, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetVersion(char * *aVersion) { return _to GetVersion(aVersion); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_REVALUATOR(_to) \
  NS_SCRIPTABLE NS_IMETHOD Init(const char **args, PRUint32 length, PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Init(args, length, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Call1(const char *funName, nsIVariant *arg, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Call1(funName, arg, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Call(const char *funName, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Call(funName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Library(const char *library, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Library(library, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Eval(const char *cmd, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Eval(cmd, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Exists(const char *var, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Exists(var, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Get(const char *var, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Get(var, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Assign(const char *var, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Assign(var, val, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Remove(const char *var, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Remove(var, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetVersion(char * *aVersion) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetVersion(aVersion); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public REvaluator
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_REVALUATOR

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, REvaluator)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* boolean init ([array, size_is (length)] in string args, in unsigned long length); */
NS_IMETHODIMP _MYCLASS_::Init(const char **args, PRUint32 length, PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant call1 (in string funName, in nsIVariant arg); */
NS_IMETHODIMP _MYCLASS_::Call1(const char *funName, nsIVariant *arg, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant call (in string funName); */
NS_IMETHODIMP _MYCLASS_::Call(const char *funName, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant library (in string library); */
NS_IMETHODIMP _MYCLASS_::Library(const char *library, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant eval (in string cmd); */
NS_IMETHODIMP _MYCLASS_::Eval(const char *cmd, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant exists (in string var); */
NS_IMETHODIMP _MYCLASS_::Exists(const char *var, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant get (in string var); */
NS_IMETHODIMP _MYCLASS_::Get(const char *var, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant assign (in string var, in nsIVariant val); */
NS_IMETHODIMP _MYCLASS_::Assign(const char *var, nsIVariant *val, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant remove (in string var); */
NS_IMETHODIMP _MYCLASS_::Remove(const char *var, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute string version; */
NS_IMETHODIMP _MYCLASS_::GetVersion(char * *aVersion)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_Revaluator_h__ */
