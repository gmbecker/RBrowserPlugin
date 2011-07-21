/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM IREvaluator.idl
 */

#ifndef __gen_IREvaluator_h__
#define __gen_IREvaluator_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_IRObject_h__
#include "IRObject.h"
#endif

#ifndef __gen_IRFunction_h__
#include "IRFunction.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    IREvaluator */
#define IREVALUATOR_IID_STR "bed33746-1b4d-4495-9365-7642c3186276"

#define IREVALUATOR_IID \
  {0xbed33746, 0x1b4d, 0x4495, \
    { 0x93, 0x65, 0x76, 0x42, 0xc3, 0x18, 0x62, 0x76 }}

class NS_NO_VTABLE NS_SCRIPTABLE IREvaluator : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(IREVALUATOR_IID)

  /* IRObject callFun (in string fun, [array, size_is (length)] in IRObject args, in unsigned long length); */
  NS_SCRIPTABLE NS_IMETHOD CallFun(const char *fun, IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM) = 0;

  /* IRObject call (in IRFunction fun, [array, size_is (length)] in IRObject args, in unsigned long length); */
  NS_SCRIPTABLE NS_IMETHOD Call(IRFunction *fun, IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(IREvaluator, IREVALUATOR_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_IREVALUATOR \
  NS_SCRIPTABLE NS_IMETHOD CallFun(const char *fun, IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Call(IRFunction *fun, IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_IREVALUATOR(_to) \
  NS_SCRIPTABLE NS_IMETHOD CallFun(const char *fun, IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM) { return _to CallFun(fun, args, length, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Call(IRFunction *fun, IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM) { return _to Call(fun, args, length, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_IREVALUATOR(_to) \
  NS_SCRIPTABLE NS_IMETHOD CallFun(const char *fun, IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->CallFun(fun, args, length, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Call(IRFunction *fun, IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Call(fun, args, length, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public IREvaluator
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_IREVALUATOR

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, IREvaluator)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* IRObject callFun (in string fun, [array, size_is (length)] in IRObject args, in unsigned long length); */
NS_IMETHODIMP _MYCLASS_::CallFun(const char *fun, IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* IRObject call (in IRFunction fun, [array, size_is (length)] in IRObject args, in unsigned long length); */
NS_IMETHODIMP _MYCLASS_::Call(IRFunction *fun, IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_IREvaluator_h__ */
