
#include "Moz191Compat.h"
#include "nsXPCOM.h"
#include "nsXPCOMGlue.h"
#include "RMozilla.h"
#include <stdio.h>
extern "C"
void R_nsISupportsFinalizer(SEXP ptr)
{
  Rprintf( "Executing Finalizer" );
  R_NS_IF_RELEASE( ptr );
  return;
}
