#include "nsXPTCUtils.h"
#include "nsMemory.h"

#include "XPCNativeWrapper.h"
#include "js/jspubtd.h"



JSContext *
R_getJSContext()
{
    // Or nsIScriptContext. 

//   nsIScriptContext *context = sgo->GetContext();
//   JSContext* jscontext = NS_REINTERPRET_CAST(JSContext*, context->GetNativeContext());

  XPCCallContext cycleCollectionContext(NATIVE_CALLER);
  JSContext *cx = mCycleCollectionContext->GetJSContext();
}
