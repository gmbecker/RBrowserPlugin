/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM IRObject.idl
 */

#ifndef __gen_IRObject_h__
#define __gen_IRObject_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_IRBasicTypes_h__
#include "IRBasicTypes.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    IRObject */
#define IROBJECT_IID_STR "1ef917f6-ee8f-42b2-8ec4-9025a6bcbbe2"

#define IROBJECT_IID \
  {0x1ef917f6, 0xee8f, 0x42b2, \
    { 0x8e, 0xc4, 0x90, 0x25, 0xa6, 0xbc, 0xbb, 0xe2 }}

class NS_NO_VTABLE NS_SCRIPTABLE IRObject : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(IROBJECT_IID)

  /* long length (); */
  NS_SCRIPTABLE NS_IMETHOD Length(PRInt32 *_retval NS_OUTPARAM) = 0;

  /* string typeof (); */
  NS_SCRIPTABLE NS_IMETHOD Typeof(char **_retval NS_OUTPARAM) = 0;

  /* string class (); */
  NS_SCRIPTABLE NS_IMETHOD Class(char **_retval NS_OUTPARAM) = 0;

  /* string str (); */
  NS_SCRIPTABLE NS_IMETHOD Str(char **_retval NS_OUTPARAM) = 0;

  /* void names ([array, size_is (len)] out string ans, out unsigned long len); */
  NS_SCRIPTABLE NS_IMETHOD Names(char ***ans NS_OUTPARAM, PRUint32 *len NS_OUTPARAM) = 0;

  /* [noscript] SEXP asSEXP (); */
  NS_IMETHOD AsSEXP(SEXPREC * *_retval NS_OUTPARAM) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(IRObject, IROBJECT_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_IROBJECT \
  NS_SCRIPTABLE NS_IMETHOD Length(PRInt32 *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Typeof(char **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Class(char **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Str(char **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Names(char ***ans NS_OUTPARAM, PRUint32 *len NS_OUTPARAM); \
  NS_IMETHOD AsSEXP(SEXPREC * *_retval NS_OUTPARAM); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_IROBJECT(_to) \
  NS_SCRIPTABLE NS_IMETHOD Length(PRInt32 *_retval NS_OUTPARAM) { return _to Length(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD Typeof(char **_retval NS_OUTPARAM) { return _to Typeof(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD Class(char **_retval NS_OUTPARAM) { return _to Class(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD Str(char **_retval NS_OUTPARAM) { return _to Str(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD Names(char ***ans NS_OUTPARAM, PRUint32 *len NS_OUTPARAM) { return _to Names(ans, len); } \
  NS_IMETHOD AsSEXP(SEXPREC * *_retval NS_OUTPARAM) { return _to AsSEXP(_retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_IROBJECT(_to) \
  NS_SCRIPTABLE NS_IMETHOD Length(PRInt32 *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Length(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD Typeof(char **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Typeof(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD Class(char **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Class(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD Str(char **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Str(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD Names(char ***ans NS_OUTPARAM, PRUint32 *len NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Names(ans, len); } \
  NS_IMETHOD AsSEXP(SEXPREC * *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->AsSEXP(_retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public IRObject
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_IROBJECT

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, IRObject)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* long length (); */
NS_IMETHODIMP _MYCLASS_::Length(PRInt32 *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string typeof (); */
NS_IMETHODIMP _MYCLASS_::Typeof(char **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string class (); */
NS_IMETHODIMP _MYCLASS_::Class(char **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string str (); */
NS_IMETHODIMP _MYCLASS_::Str(char **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void names ([array, size_is (len)] out string ans, out unsigned long len); */
NS_IMETHODIMP _MYCLASS_::Names(char ***ans NS_OUTPARAM, PRUint32 *len NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] SEXP asSEXP (); */
NS_IMETHODIMP _MYCLASS_::AsSEXP(SEXPREC * *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_IRObject_h__ */
