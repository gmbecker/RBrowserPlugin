#ifndef R_XPT_VARIANTS_H
#define R_XPT_VARIANTS_H

SEXP convertVariantToR(const nsXPTCVariant *var);
nsresult convertRToVariant(SEXP rval, nsXPTCVariant *dest);

#endif
