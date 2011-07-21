#include "nsMemory.h"

#define ROBJECT_METHODS(_class) \
/* long length (); */ \
NS_IMETHODIMP  \
_class::Length(PRInt32 *_retval NS_OUTPARAM) \
{ \
    *_retval = Rf_length(_obj); \
    return(NS_OK); \
} \
 \
/* string typeof (); */ \
NS_IMETHODIMP _class::Typeof(char **_retval NS_OUTPARAM) \
{ \
    char *ans; \
    const char *val; \
    SEXP rans; \
    rans = callUnary("typeof"); \
    val = CHAR(STRING_ELT(rans, 0)); \
    ans = (char *) nsMemory::Clone(val, strlen(val) + 1); \
    *_retval = ans; \
    return(NS_OK); \
} \
 \
/* string class (); */ \
NS_IMETHODIMP  \
_class::Class(char **_retval NS_OUTPARAM) \
{ \
    char *ans; \
    const char *val; \
    SEXP rans; \
   \
    rans = callUnary("class"); \
    val = CHAR(STRING_ELT(rans, 0)); \
    ans = (char *) nsMemory::Clone(val, strlen(val) + 1); \
    *_retval = ans; \
    return(NS_OK); \
} \
 \
 \
/* string str (); */ \
NS_IMETHODIMP  \
_class::Str(char **_retval NS_OUTPARAM) \
{ \
 \
    char *ans; \
    const char *val; \
    SEXP rans; \
 \
    rans = callUnary("str"); \
    val = CHAR(STRING_ELT(rans, 0)); \
    ans = (char *) nsMemory::Clone(val, strlen(val) + 1); \
    *_retval = ans; \
    return(NS_OK); \
} \
 \
/* stringArray names (); */ \
NS_SCRIPTABLE NS_IMETHODIMP \
_class::Names(char ***ans NS_OUTPARAM, PRUint32 *len NS_OUTPARAM)  \
{ \
    SEXP rans; \
    int i, n; \
    const char *tmp; \
 \
    rans = callUnary("names"); \
    n = Rf_length(rans); \
    *len = n; \
    for(i = 0 ; i < n; i++) { \
        tmp = CHAR(STRING_ELT(rans, i)); \
        *ans[i] = (char *)nsMemory::Clone(tmp, strlen(tmp) + 1); \
    } \
    return(NS_OK); \
} \
\
NS_IMETHODIMP \
_class::AsSEXP(SEXPREC * *_retval NS_OUTPARAM) \
{ \
    *_retval = _obj; \
    return(NS_OK); \
} 
