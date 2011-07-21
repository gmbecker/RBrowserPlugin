/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM IRFunction.idl
 */

#ifndef __gen_IRFunction_h__
#define __gen_IRFunction_h__


#ifndef __gen_IRObject_h__
#include "IRObject.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    IRFunction */
#define IRFUNCTION_IID_STR "5ff701ca-cfef-4e7d-af8d-f882cc52e8d5"

#define IRFUNCTION_IID \
  {0x5ff701ca, 0xcfef, 0x4e7d, \
    { 0xaf, 0x8d, 0xf8, 0x82, 0xcc, 0x52, 0xe8, 0xd5 }}

class NS_NO_VTABLE NS_SCRIPTABLE IRFunction : public IRObject {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(IRFUNCTION_IID)

  /* IRObject call ([array, size_is (length)] in IRObject args, in unsigned long length); */
  NS_SCRIPTABLE NS_IMETHOD Call(IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(IRFunction, IRFUNCTION_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_IRFUNCTION \
  NS_SCRIPTABLE NS_IMETHOD Call(IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_IRFUNCTION(_to) \
  NS_SCRIPTABLE NS_IMETHOD Call(IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM) { return _to Call(args, length, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_IRFUNCTION(_to) \
  NS_SCRIPTABLE NS_IMETHOD Call(IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Call(args, length, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public IRFunction
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_IRFUNCTION

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, IRFunction)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* IRObject call ([array, size_is (length)] in IRObject args, in unsigned long length); */
NS_IMETHODIMP _MYCLASS_::Call(IRObject **args, PRUint32 length, IRObject **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_IRFunction_h__ */
