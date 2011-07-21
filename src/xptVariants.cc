#include "xpcom-config.h"
//#include "xpcom/nscore.h"
#include "nsXPCOM.h"

//#include "xpcom/nsIInterfaceInfoManager.h"
//#include "xpcom/xptinfo.h"
//#include "xpcom/xptcall.h"

#include "Moz191Compat.h"
#include "RMozilla.h"

#include "xptVariants.h"

#define CASE(x)  case nsXPTType::x :


SEXP
convertVariantToR(const nsXPTCVariant *var)
{
    SEXP ans = R_NilValue; 
    if(var->IsPtrData()) {
        return( R_MakeExternalPtr(var->ptr, Rf_install(""), R_NilValue) );
    }

    switch(var->type) {
        CASE(T_VOID)
            ans = R_NilValue;
            break;
        CASE(T_I8)
            ans = ScalarInteger(var->val.i8);
            break;
        CASE(T_I16)
            ans = ScalarInteger(var->val.i16);
            break;
        CASE(T_I32)
            ans = ScalarInteger(var->val.i32);
            break;
        CASE(T_I64)
            ans = ScalarReal(var->val.i64);
            break;
        CASE(T_U16)
            ans = ScalarInteger(var->val.u16);
            break;
        CASE(T_U32)
            ans = ScalarReal(var->val.u32);
            break;
        CASE(T_U64)
            ans = ScalarReal(var->val.u64);
            PROBLEM "may have lost precision!"
            WARN
            break;
        CASE(T_FLOAT)
            ans = ScalarReal(var->val.f);
            break;
        CASE(T_DOUBLE)
            ans = ScalarReal(var->val.d);
            break;
        CASE(T_BOOL)
            ans = ScalarLogical(var->val.b);
            break;
        CASE(T_CHAR)
        {
            char buf[2] = "";
            buf[0] = var->val.c;
            ans = mkString(buf);
        }
            break;
        CASE(T_CSTRING)
            ans = mkString((char *)var->ptr);
            break;
#if 0
        CASE(T_UTF8STRING)
            ans = mkString(var->ptr);
            break;
        CASE(T_WCHAR_STR)
            ans = mkString(var->ptr);
            break;
#endif
        default:
            PROBLEM "unhandled conversion of XPCOM variant to R (%d)", (int) var->type
              WARN 
            break;
/*
IID, DOMSTRING, INTERFACE, INTERFACE_IS, ARRAY, 
PSTRING_SIZE_IS, PWSTRING_SIZE_IS

ARRAY
 */
    }

    return(ans);
}

extern "C"
SEXP
R_convertVariantToR(SEXP rvar)
{
    const nsXPTCVariant *var;
    var = (const nsXPTCVariant *) R_ExternalPtrAddr(rvar);
fprintf(stderr, "nsXPTCVariant *=  %p\n", var);
    return(convertVariantToR(var));
}


nsresult
/* needPersistance indicates whether the R value will disappear before the variant does.
     If this is true, any pointer data needs to be copied and VAR_IS_ALLOCD needs to be set.
 */
convertRToVariant(SEXP rval, nsXPTCVariant *dest, nsXPTType target, PRBool needPersistance) 
{
    nsresult rv = NS_OK;
    
    int n = Rf_length(rval);

    if(n == 0) {
        dest->val.p = dest->ptr = nsnull;
        dest->type = nsXPTType::T_VOID;
        return(rv);
    }
    
    if(n == 1) {
        dest->val.p = dest->ptr = nsnull;
        dest->type = target;
        switch(target) {
        CASE(T_VOID)
            dest->ptr = NULL;
            break;
        CASE(T_I8)
            dest->val.i8 = INTEGER(rval)[0];
            break;
        CASE(T_I16)
            dest->val.i16 = INTEGER(rval)[0];
            break;
        CASE(T_I32)
            dest->val.i32 = INTEGER(rval)[0];
            break;
        CASE(T_I64)
            dest->val.i32 = REAL(rval)[0];
            break;
        CASE(T_U16)
            dest->val.u16 = REAL(rval)[0];
            break;
        CASE(T_U32)
            dest->val.u32 = REAL(rval)[0];
            break;
        CASE(T_U64)
            dest->val.u64 = REAL(rval)[0];
            break;
        CASE(T_FLOAT)
            dest->val.f = REAL(rval)[0];
            break;
        CASE(T_DOUBLE)
            dest->val.d = REAL(rval)[0];
            break;
        CASE(T_BOOL)
            dest->val.b = LOGICAL(rval)[0];
            break;
        CASE(T_CHAR)
            dest->val.c = CHAR(STRING_ELT(rval, 0))[0];
            break;
        CASE(T_CSTRING)
            dest->ptr = dest->val.p = (void *) CHAR(STRING_ELT(rval, 0));
            break;
          default:
              PROBLEM "unhandled type (%d) for setting variant", (int) target
                  WARN;
              break;
        }
    }

    return(rv);
}


extern "C"
SEXP
R_new_nsXPTCVariant()
{
    nsXPTCVariant *var;
    SEXP ans, klass;

    PROTECT(klass = MAKE_CLASS("nsXPTCVariant"));
    PROTECT(ans = NEW(klass));

    var = new nsXPTCVariant();    
    var->val.i32 = 302;
    var->type = nsXPTType::T_I32;

    SET_SLOT(ans, Rf_install("ref"), R_MakeExternalPtr(var, Rf_install("nsXPTCVariant"), R_NilValue));

    UNPROTECT(2);
    return(ans);
}

extern "C"
SEXP
R_set_nsXPTCVariant(SEXP rvar, SEXP rval, SEXP flags, SEXP targetType, SEXP duplicate)
{
    nsXPTCVariant *dest;
    nsresult rv;

    dest = (nsXPTCVariant *) R_ExternalPtrAddr(rvar);

    /* if rval is list() or NA, then don't set the variant.
       May want to check if PTR_IS_DATA */
    if((TYPEOF(rval) == LGLSXP && INTEGER(rval)[0] == NA_LOGICAL) || (length(rval) == 0 && TYPEOF(rval) == VECSXP)) {
        dest->type = INTEGER(targetType)[0];
        rv = NS_OK;
    } else
        rv = convertRToVariant(rval, dest, INTEGER(targetType)[0], LOGICAL(duplicate)[0]);

    if(Rf_length(flags) > 0)
        dest->flags |= INTEGER(flags)[0];

    return(ScalarInteger(rv));
}

