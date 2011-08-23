#include "nsXPTCUtils.h"
#include "nsMemory.h"

//#include "content/nsIDocument.h"
#include "nsIDocument.h"


/* Order is important. The two above here need to be first as we redefine Realloc() in R. */
#include "RXPCOM.h"
#include "nsIXPConnect.h"
#include "RobjJS.h"
#include "nsServiceManagerUtils.h"
SEXP convertVariantToRVector(nsIVariant * var);



SEXP
R_MakeJavaScriptReference(nsISupports *ref)
{
  SEXP klass, ans;
  PROTECT(klass =  MAKE_CLASS("JavaScriptRef"));
  PROTECT(ans = NEW(klass));
  SET_SLOT(ans, Rf_install("ref"), R_MakeExternalPtr(ref, Rf_install("JavaScriptRef"), R_NilValue));
  UNPROTECT(2);
  return(ans);
}


#if 0
SEXP
MakeRString(nsAutoString *str)
{
  const char *val;
  val = str->ToNewCString();
  return(ScalarString(mkChar(val)));
}
#endif

/*
 Convert a Firefox/XPCOM variant to an R object.
 */
SEXP
convertVariantToR(nsIVariant * var)
{
  PRInt32 value;
  PRUint16 dataType;
  nsresult nr;
  SEXP ans = R_NilValue;
  nr = var->GetDataType(&dataType);
  
  fprintf(stderr, "data type %d;   ", (int) dataType);fflush(stderr);
  switch(dataType) {
    case  nsIDataType::VTYPE_INT8:
    case  nsIDataType::VTYPE_INT16:
    case  nsIDataType::VTYPE_INT32:
    case  nsIDataType::VTYPE_UINT8:
    case  nsIDataType::VTYPE_UINT16:
        var->GetAsInt32(&value);
  fprintf(stderr, "JS->R: integer variant %d\n", (int) value);fflush(stderr);
	ans = ScalarInteger(value);
	break;
    case  nsIDataType::VTYPE_BOOL:
      {
	PRBool b;
	var->GetAsBool(&b);
  fprintf(stderr, "JS->R: boolean variant %d\n", (int) b);fflush(stderr);
	ans = ScalarLogical(b);
      }
      break;
    case  nsIDataType::VTYPE_DOUBLE:
    case  nsIDataType::VTYPE_FLOAT:
    case  nsIDataType::VTYPE_UINT32:
    case  nsIDataType::VTYPE_UINT64:
    case  nsIDataType::VTYPE_INT64:
      {
	double d;
	var->GetAsDouble(&d);
  fprintf(stderr, "JS->R: double variant %lf\n", d);fflush(stderr);
	ans = ScalarReal(d);
      }
      break;
    case nsIDataType::VTYPE_EMPTY:
    case nsIDataType::VTYPE_VOID:
         ans = R_NilValue;
      break;
    case nsIDataType::VTYPE_EMPTY_ARRAY:
         ans = NEW_LOGICAL(0);
      break;
    case nsIDataType::VTYPE_CHAR:
      {
        char buf[2];
	buf[1] = '\0';
	var->GetAsChar(buf);
        ans = ScalarString(mkChar(buf));
      }
      break;
    case nsIDataType::VTYPE_CSTRING:
    case nsIDataType::VTYPE_ASTRING:
    case nsIDataType::VTYPE_CHAR_STR:
     case nsIDataType::VTYPE_WSTRING_SIZE_IS:
      {
	char *str;
  fprintf(stderr, "JS->R: string type\n");fflush(stderr);
	nr = var->GetAsString(&str);
	ans = ScalarString(mkChar(str));
      }
      break;

#if 0
     case nsIDataType::VTYPE_WCHAR:
     case nsIDataType::VTYPE_DOMSTRING:
     case nsIDataType::VTYPE_WSTRING_SIZE_IS:
{
       nsAutoString s;
       if (NS_SUCCEEDED(nr=var->GetAsAString(s))) {
	 ans = MakeRString(&s);
       }
     }
       break;
#endif
    case nsIDataType::VTYPE_INTERFACE_IS: {
       nsISupports *ref;
       nsIID *iid;
  fprintf(stderr, "JS->R: interface \n");fflush(stderr);
       nr = var->GetAsInterface(&iid, (void **) &ref);

       if(NS_FAILED(nr)) {
	 return(R_NilValue);
       }
#if 0
       nsIDocument *doc;
       nr = ref->QueryInterface(NS_GET_IID(nsIDocument), (void **) &doc);
  fprintf(stderr, "QueryInterface for nsIDocument %s\n", NS_FAILED(nr) ? "failed" : "succeeded");fflush(stderr);       
       nsINode *node;
       nr = ref->QueryInterface(NS_GET_IID(nsINode), (void **) &node);
  fprintf(stderr, "QueryInterface for nsINode %s\n", NS_FAILED(nr) ? "failed" : "succeeded");fflush(stderr);       
#endif
//       nr = var->GetAsInterface( NS_GET_IID(nsIDocument), (void **) &doc);

       NS_ADDREF(ref);

       ans = R_MakeJavaScriptReference(ref);
      }
    break;
    case nsIDataType::VTYPE_ARRAY:
      ans = convertVariantToRVector(var);
      break;
     default:
       {
	 fprintf(stderr, "JS->R: no conversion for  variant\nChecking for wrapped R object: ");fflush(stderr);
	JSContext *jscon = GetContextForR(0);
	nsresult rv;

	nsCOMPtr<nsIXPConnect> xpc_;
	xpc_ = do_GetService("@mozilla.org/js/xpc/XPConnect;1", &rv);
	JSClass *kl;
	jsval val;
	rv = xpc_->VariantToJS(jscon, JS_GetGlobalObject(jscon), var, &val);
	kl = JS_GET_CLASS(jscon, JSVAL_TO_OBJECT(val)); 
	if (kl -> name == "RFunction" || kl -> name == "RObject")
	  {
	    ans = (SEXP) JS_GetPrivate(jscon, JSVAL_TO_OBJECT(val));
	    fprintf(stderr, "%s found.\n", kl -> name); fflush(stderr);
	  }
	else
	  {
	    fprintf(stderr, "No R objects found."); fflush(stderr);
	  }
       }
       break;
  }
  return(ans);
}



/***********************************************************************************/

//#include <xpcom/nsComponentManagerUtils.h>
#include "nsComponentManagerUtils.h"
nsresult
RElementAsVariant(SEXP ans, int index, nsIVariant **ret)
{
  nsresult nr = NS_OK;
  nsCOMPtr<nsIWritableVariant> var;
  var = do_CreateInstance("@mozilla.org/variant;1", &nr);
  NS_ENSURE_SUCCESS(nr, nr);


      fprintf(stderr, "R->JS: type %d\n", TYPEOF(ans));fflush(stderr);

      switch(TYPEOF(ans)) {

      case LGLSXP:  
          fprintf(stderr, "R->JS: logical %d\n", LOGICAL(ans)[0]);fflush(stderr);
          var->SetAsBool(LOGICAL(ans)[index]);
          break;
      case INTSXP:  
	  fprintf(stderr, "R->JS: integer %d\n", Rf__INTEGER(ans) [0]);fflush(stderr);
          var->SetAsInt32(Rf__INTEGER(ans)[index]);
          break;
      case REALSXP:  
          fprintf(stderr, "R->JS: numeric %lf\n", REAL(ans)[0]);fflush(stderr);
          var->SetAsDouble(REAL(ans)[index]);
          break;
      case STRSXP:  
	fprintf(stderr, "R->JS: character %s\n", CHAR(STRING_ELT(ans, index)));fflush(stderr);
	var->SetAsString(CHAR(STRING_ELT(ans, index)));
        
	break;
      default:
	{
	//it would be much easier to just construct the js object (jsval/JSObject) we want directly but then how do we return it?? Find and call the xpconnect conversion function directly?
	//nsIVariant JSToVariant(in JSContextPtr ctx, in jsval value); from nsIXPConnect, but we'll need to get that object (which I think will give us the context we need as well).
	
	char tmp[255];
	JSContext *jscon = GetContextForR(0);
	nsresult rv;

	nsCOMPtr<nsIXPConnect> xpc_;
	xpc_ = do_GetService("@mozilla.org/js/xpc/XPConnect;1", &rv);
	JSClass *kl;
	if (TYPEOF(ans) == CLOSXP)
	  kl = &Rfun;
	else
	  kl = &Robj;
	JSObject *obj = JS_NewObject(jscon, kl,  NULL, JS_GetGlobalObject(jscon));
	JS_SetPrivate(jscon, obj, ans);
	jsval val = OBJECT_TO_JSVAL(obj);
	nsCOMPtr<nsIVariant> funvar;
	rv = xpc_ -> JSToVariant(jscon, val, getter_AddRefs(funvar));
	*ret = funvar;
	fprintf(stderr, "R->JS: No conversion found. Returning wrapped R object.");fflush(stderr);
	return(NS_OK);
	}
	break;
      }


  return var->QueryInterface(NS_GET_IID(nsIVariant), (void **) ret);
}

boolean
convertRToVariant(SEXP ans, nsIVariant **_returnValue)
{
  int n = Rf_length(ans);

  nsresult nr = NS_OK;
  nsCOMPtr<nsIWritableVariant> var;
  var = do_CreateInstance("@mozilla.org/variant;1", &nr);
  NS_ENSURE_SUCCESS(nr, nr);


  if(n == 0)  {
    if(ans == R_NilValue)
        var->SetAsVoid();
    else
        var->SetAsEmptyArray();
    return(true);
  }

  if(n >= 1) {
    // Deal with arrays.
      // See VariantUtils.cpp in extensions/python/xpcom/src.
      nsIVariant **buf = new nsIVariant *[n];
      memset(buf, 0, sizeof(nsIVariant*) * n);
      for(int i = 0; i < n; i++) {
          RElementAsVariant(ans, i, buf + i);
      }

      var->SetAsArray(nsXPTType::T_INTERFACE_IS, &NS_GET_IID(nsIVariant), n, buf);
      delete [] buf;

  } else {

  }

  var->QueryInterface(NS_GET_IID(nsIVariant), (void **)_returnValue);

  return(true);
}


SEXP
AllocRArray(PRUint16 type, PRUint32 count)
{
  SEXP obj = R_NilValue;
  switch(type) {
    case nsXPTType::T_I8:
    case nsXPTType::T_U8:
    case nsXPTType::T_U16:
    case nsXPTType::T_I32:
      obj = NEW_INTEGER(count);
      break;

    case nsXPTType::T_I64:
    case nsXPTType::T_U32:
    case nsXPTType::T_U64:
    case nsXPTType::T_FLOAT:
    case nsXPTType::T_DOUBLE:
      obj = NEW_NUMERIC(count);
      break;

   case nsXPTType::T_BOOL:
     obj = NEW_LOGICAL(count);
     break;
   case nsXPTType::T_CHAR:
   case nsXPTType::T_WCHAR:
      obj = NEW_CHARACTER(count);
     break;
    case nsXPTType::T_INTERFACE:
    case nsXPTType::T_INTERFACE_IS:
      obj = NEW_LIST(count);
      break;
   case nsXPTType::T_VOID:
      obj = R_NilValue;
     break;
    default:
      obj = R_NilValue;
      break;
  }

  return(obj);
}

#undef Free

static PRUint32 GetArrayElementSize( PRUint8 t);

SEXP 
convertVariantToRVector(nsIVariant * var)
{
 
  fprintf(stderr, "Conversion to R vector\n"); fflush(stderr);  

  nsIID iid;
  void *array_ptr;
  PRUint16 type;
  PRUint32 count;
  nsresult nr;

  nr = var->GetAsArray(&type, &iid, &count, &array_ptr);
  if(NS_FAILED(nr)) {

  }

  /* Much of this taken from VariantUtils.cpp in the extensions/python/xpcom/src directory of the mozilla build*/
  PRUint32 array_element_size = GetArrayElementSize(type);
  PRUint8 *pthis = (PRUint8 *)array_ptr;
  SEXP ans;
  int i;

  PROTECT(ans = AllocRArray(type, count));


  for(i = 0; i < count; i++, pthis += array_element_size) {

    switch(type) {
    case nsXPTType::T_BOOL:
      LOGICAL(ans)[i] = *((PRBool *)pthis);
      break;

    case nsXPTType::T_I8:
      Rf__INTEGER(ans)[i] = *((PRInt8 *)pthis);
      break;
    case nsXPTType::T_I16:
      Rf__INTEGER(ans)[i] = *((PRInt16 *)pthis);
      break;
    case nsXPTType::T_I32:
      Rf__INTEGER(ans)[i] = *((PRInt32 *)pthis);
      break;
    case nsXPTType::T_I64:
      REAL(ans)[i] = *((PRInt64 *)pthis);
      break;
    case nsXPTType::T_U16:
      Rf__INTEGER(ans)[i] = *((PRUint16 *)pthis);
      break;
    case nsXPTType::T_U32:
      REAL(ans)[i] = *((PRUint32 *)pthis);
      break;
    case nsXPTType::T_U64:
      REAL(ans)[i] = *((PRUint64 *)pthis);
      break;
    case nsXPTType::T_FLOAT:
      REAL(ans)[i] = *((float *)pthis);
      break;
    case nsXPTType::T_DOUBLE:
      REAL(ans)[i] = *((double *)pthis);
      break;

    case nsXPTType::T_INTERFACE_IS:
    case nsXPTType::T_INTERFACE: {
      nsISupports **ref = (nsISupports **)pthis;
       NS_ADDREF(*ref);
       SET_VECTOR_ELT(ans, i,  R_MakeJavaScriptReference(*ref));
    }
      break;
    default:
      break;
    }
  }
  // Can free each element as we loop through the array.
  //  FreeSingleArray(array_ptr, count, (PRUint8)type);

  // Need to find the library that gives us NS_Free_P symbol
  //  nsMemory::Free(array_ptr);
  UNPROTECT(1);
  return ans;
}




static PRUint32 GetArrayElementSize( PRUint8 t)
{
	PRUint32 ret;
	switch (t & XPT_TDP_TAGMASK) {
		case nsXPTType::T_U8:
		case nsXPTType::T_I8:
			ret = sizeof(PRInt8); 
			break;
		case nsXPTType::T_I16:
		case nsXPTType::T_U16:
			ret = sizeof(PRInt16); 
			break;
		case nsXPTType::T_I32:
		case nsXPTType::T_U32:
			ret = sizeof(PRInt32); 
			break;
		case nsXPTType::T_I64:
		case nsXPTType::T_U64:
			ret = sizeof(PRInt64); 
			break;
		case nsXPTType::T_FLOAT:
			ret = sizeof(float); 
			break;
		case nsXPTType::T_DOUBLE:
			ret = sizeof(double); 
			break;
		case nsXPTType::T_BOOL:
			ret = sizeof(PRBool); 
			break;
		case nsXPTType::T_CHAR:
			ret = sizeof(char); 
			break;
		case nsXPTType::T_WCHAR:
			ret = sizeof(PRUnichar); 
			break;
		case nsXPTType::T_IID:
		case nsXPTType::T_CHAR_STR:
		case nsXPTType::T_WCHAR_STR:
		case nsXPTType::T_INTERFACE:
		case nsXPTType::T_DOMSTRING:
		case nsXPTType::T_INTERFACE_IS:
		case nsXPTType::T_PSTRING_SIZE_IS:
		case nsXPTType::T_CSTRING:
		case nsXPTType::T_ASTRING:
		case nsXPTType::T_UTF8STRING:

			ret = sizeof( void * );
			break;
	default:
		NS_ABORT_IF_FALSE(0, "Unknown array type code!");
		ret = 0;
		break;
	}
	return ret;
}
