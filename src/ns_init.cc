#include "xpcom-config.h"
#include "mozilla-config.h"
#include "nsEmbedAPI.h"
/*
#include "nspr/prtypes.h"
#include "xpcom/nscore.h"
#include "nsXPCOM.h"
#include "xulapp/nsXULAppAPI.h" 
#if 1
#include "nsEmbedCID.h" 
//#include "sdk/prtypes.h" 

#include "embed_base/nsEmbedAPI.h"
#include "nsXPCOMGlue.h" 
#include "nsDebug.h" 
#include "Gabe.h"
#include "widget/nsIBaseWindow.h" 

#include "nsIWebBrowser.h" 
#include "docshell/nsIWebNavigation.h" 
#include "nsEmbedString.h" 
#include "xulapp/nsXULAppAPI.h" 
#include "nsComponentManagerUtils.h"
#endif
*/
//#include "necko/nsICookieManager2.h"
#include "Moz191Compat.h"
#include "nsXPCOM.h"
#include "nsEmbedCID.h" 
#include "nsXPCOMGlue.h"
#include "nsDebug.h"
#include "Moz191Compat.h" //Note this is in here twice, not sure why
#include "nsIWebBrowser.h" 
#include "nsComponentManagerUtils.h"

#include "RMozilla.h"
#include "gtk/gtk.h"
//#include "RChrome.h"

#define XP_UNIX
#include "jsapi.h"
#if 0
extern "C"
SEXP
R_NS_InitEmbedding()
{
  nsresult rv;
  nsIDirectoryServiceProvider *provider;
  provider = nsnull;
  rv = NS_InitEmbedding(nsnull, provider);
  return(ScalarInteger(rv));
}
#endif

  


extern "C"
SEXP
R_XRE_InitEmbedding()
{
    nsILocalFile *libxul = nsnull;
    nsILocalFile *appDir = nsnull;
    nsresult rv;

    //    rv = XRE_InitEmbedding(libxul, appDir, 0, 0, 0);
    rv = XRE_InitEmbedding(libxul, appDir, nsnull, nsnull, 0);    
    return(ScalarInteger(rv));
}

  
nsresult (*XRE_InitEmbedding)(nsILocalFile *aLibXULDirectory,
                            nsILocalFile *aAppDirectory,
                            nsIDirectoryServiceProvider *aAppDirProvider,
                            nsStaticModuleInfo const *aStaticComponents,
                              PRUint32 aStaticComponentCount);

nsresult (*XRE_LockProfileDirectory)(nsILocalFile* aDirectory, nsISupports* *aLockObject);


/*
  http://www.mozilla.org/projects/xpcom/glue/Component_Reuse.html

*/;
extern "C"
SEXP
R_XPCOMGlueStartup()
{
/* http://www.nabble.com/XRE_InitEmbedding-gecko-1.9-c%2B%2B-td14502290.html 
  GRE_GetGREPathWithProperties (if you don't know where XULRunner is installed) 
  XPCOMGlueStartup
*/
    nsresult rv;
    //    char *xpcomPath = "/Users/duncan/Downloads/xulrunner-sdk/bin/libxpcom.dylib";
    char *xpcomPath = "/home/gmbecker/gabe/checkedout/FirefoxSource192/obj-xulrunner/dist/xulrunner-sdk/bin/libxpcom.so";
    rv = XPCOMGlueStartup (xpcomPath);

    if(NS_FAILED(rv)) {
        PROBLEM "error in XPCOMGlueStartup. Got the right directory for xpcom"
        ERROR;
    }


    static const nsDynamicFunctionLoad kXRESymbols[] = { 
        { "XRE_InitEmbedding", (NSFuncPtr*) &XRE_InitEmbedding }, 
        // If you need other XRE_ functions such as XRE_LockProfileDirectory, 
        // add them to the list here: 
        { "XRE_LockProfileDirectory", (NSFuncPtr*) &XRE_LockProfileDirectory }, 
        { nsnull, nsnull } 
    }; 

  rv = XPCOMGlueLoadXULFunctions(kXRESymbols);
  if(NS_FAILED(rv)) {
        PROBLEM "error in XPCOMGlueLoadXULFunctions."
        ERROR;
  }


  nsCOMPtr<nsILocalFile> libxul;
  //rv = NS_NewNativeLocalFile(nsEmbedCString("/Users/duncan/Download/xulrunner-sdk/bin"), PR_FALSE, 
  //                              getter_AddRefs(libxul));

  rv = NS_NewNativeLocalFile(nsEmbedCString("/home/gmbecker/gabe/checkedout/FirefoxSource192/obj-xulrunner/dist/xulrunner-sdk/bin/"), PR_FALSE, 
			     getter_AddRefs(libxul));

  if(NS_FAILED(rv))
    {
      PROBLEM "error in NS_NewNativeLocalFile"
      ERROR;
    }
  rv = XRE_InitEmbedding(libxul, 0, 0, 0, 0);
  if(NS_FAILED(rv)) {
        PROBLEM "error in XRE_InitEmbedding %d", (int) rv
        ERROR;
  }
  return(ScalarLogical(TRUE));
}

SEXP
makeExternalRef(void *ptr, const char * const className)
{
    return( R_MakeExternalPtr(ptr, Rf_install(className), R_NilValue) );
}



/*-------------------------------------------------------------------------*/


extern "C"
SEXP
R_GetServiceManager()
{
    nsCOMPtr<nsIServiceManager> servMan; 
    nsresult rv = NS_GetServiceManager(getter_AddRefs(servMan)); 
    if(NS_FAILED(rv)) {
        PROBLEM "can't get service manager"
        ERROR;
    }
    return(makeExternalRef((void*) servMan, "nsIServiceManager"));
}


extern "C"
SEXP
R_GetCookieManager(SEXP r_manager)
{
    nsCOMPtr<nsIServiceManager> servMan; 
    nsresult rv;
#if 1
    rv = NS_GetServiceManager(getter_AddRefs(servMan)); 
    if(NS_FAILED(rv)) {
        PROB("getting service manager")
    }
#else
    servMan = (nsCOMPtr<nsIServiceManager>) R_ExternalPtrAddr(r_manager);
#endif

    nsCOMPtr<nsICookieManager2> cookieManager; 
#if 0
    rv = servMan->GetServiceByContractID("@mozilla.org/cookiemanager", 
                                         NS_GET_IID(nsICookieManager2), getter_AddRefs(cookieManager)); 
#else
    cookieManager = do_CreateInstance("@mozilla.org/cookiemanager", &rv);
 #endif
    if (NS_FAILED(rv)) 
        PROB("failed to get cookie manager")


   return(makeExternalRef((void*) cookieManager, "nsICookieManager"));
}

/* aaab6710-0f2c-11d5-a53b-0010a401eb10 */




#if 1
extern "C"
SEXP
R_WB()
{
    nsresult rv;
    nsCOMPtr<nsIBaseWindow> baseWindow; 
    nsCOMPtr<nsIWebBrowser> webBrowser; 
//    nsCOMPtr<nsIWebNavigation> webNavigation;
    webBrowser = do_CreateInstance(NS_WEBBROWSER_CONTRACTID, &rv); 
    if (NS_FAILED(rv)) 
    { 
        printf("do_CreateInstance webBrowser\n"); 
    } 
    baseWindow = do_QueryInterface(webBrowser); 
#if 0
    rv = baseWindow->InitWindow(frame, 0, 0, 0, 300, 400); 
    if (NS_FAILED(rv)) 
    { 
        printf("InitWindow\n"); 
    }
#endif 
    printf("baseWindow = %p\n", (void*) baseWindow);
    rv = baseWindow->InitWindow(nsnull, 0, 0, 0, 300, 400);
    if (NS_FAILED(rv))
      {
	PROB("in InitWindow");
      }
	
    rv = baseWindow->Create(); 
    if (NS_FAILED(rv)) 
    { 
        PROB("in Create")
    } 
    rv =baseWindow->SetVisibility(PR_TRUE); 
    if (NS_FAILED(rv)) 
    { 
        printf("SetVisibility\n"); 
    }
    return(ScalarLogical(TRUE));
}
#endif

extern "C"
SEXP
R_WB2(SEXP gtkWindow)
{
  GtkWidget *aWindow = (GtkWidget *) R_ExternalPtrAddr(gtkWindow);

    nsresult rv;
    nsCOMPtr<nsIBaseWindow> baseWindow; 
    nsCOMPtr<nsIWebBrowser> webBrowser; 
//    nsCOMPtr<nsIWebNavigation> webNavigation;
    webBrowser = do_CreateInstance(NS_WEBBROWSER_CONTRACTID, &rv); 
    if (NS_FAILED(rv)) 
    { 
        printf("do_CreateInstance webBrowser\n"); 
    } 
    baseWindow = do_QueryInterface(webBrowser); 
#if 0
    rv = baseWindow->InitWindow(frame, 0, 0, 0, 300, 400); 
    if (NS_FAILED(rv)) 
    { 
        printf("InitWindow\n"); 
    }
#endif 
    printf("baseWindow = %p\n", (void*) baseWindow);
    rv = baseWindow->InitWindow(aWindow, nsnull, 0, 0, 300, 400);
    if (NS_FAILED(rv))
      {
	PROB("in InitWindow");
      }
	
    rv = baseWindow->Create(); 
    if (NS_FAILED(rv)) 
    { 
        PROB("in Create")
    } 
    rv =baseWindow->SetVisibility(PR_TRUE); 
    if (NS_FAILED(rv)) 
    { 
        printf("SetVisibility\n"); 
    }

    nsCOMPtr<nsIWebNavigation> webnav = 
      do_QueryInterface(webBrowser, &rv);
    
    if (NS_FAILED(rv))
      PROB("in QueryInterface nsIWebBrowser");
    
    const char * myURI = "www.cnn.com"; 
    webnav -> LoadURI(NS_ConvertUTF8toUTF16(myURI).get(), nsIWebNavigation::LOAD_FLAGS_NONE,
    		      nsnull, nsnull, nsnull);
    //webnav->LoadURI(NS_ConvertUTF8toUTF16(myURI).get(),
    //		    NS_ConvertUTF8toUTF16("").get(), 
    //		    NS_ConvertUTF8toUTF16("").get());

/*
    if(NS_FAILED(rv))
      {
	PROBLEM "LoadURI Failed %d", (int) rv
	  ERROR;
      }
    */
    return(R_MakeExternalPtr(webBrowser, R_NilValue, R_NilValue));
}

extern "C"
SEXP test(SEXP wbrowser)
{
  nsresult rv;
  nsCOMPtr<nsIWebBrowser> mbrowser =
    (nsIWebBrowser *) R_ExternalPtrAddr(wbrowser);
  
  
  nsCOMPtr<nsIWebNavigation> webnav(do_QueryInterface(mbrowser, &rv));
  if (NS_FAILED(rv))
    PROB("Query Interface for nsIWebNavigation");
		
  webnav->LoadURI(NS_ConvertUTF8toUTF16("www.cnn.com").get(),
		  nsIWebNavigation::LOAD_FLAGS_NONE,
		  nsnull, nsnull, nsnull);

  return(ScalarLogical(TRUE));
}

#if 0
extern "C"
SEXP test2(SEXP awindow)
  {
    nsresult rv;
    RChrome *myChrome = new RChrome;
    rv = myChrome->Init((GtkWidget *) R_ExternalPtrAddr(awindow));
    if (NS_FAILED(rv))
      {
	PROBLEM "myChrome->Init Failed"
	  ERROR;
      }

    rv = myChrome->CreateWindow();
    if(NS_FAILED(rv))
      {
	PROBLEM "myChrome->CreateWindow failed"
	  ERROR;
      }
    nsIWebBrowser *webBrowser;
    
    rv = myChrome->GetWebBrowser(&webBrowser);
    //nsCOMPtr<nsIWebBrowser> webBrowser =;
    nsCOMPtr<nsIWebNavigation> webnav(do_QueryInterface(webBrowser, &rv));
   
if (NS_FAILED(rv))
    PROB("Query Interface for nsIWebNavigation");  
      
   webnav->LoadURI(NS_ConvertUTF8toUTF16("www.cnn.com").get(),
		   nsIWebNavigation::LOAD_FLAGS_NONE,
		   nsnull, nsnull, nsnull);
   
   return(ScalarLogical(TRUE));
  }

#endif

extern "C"
SEXP gtkTest()
{
  GtkWidget *window;
  char *myguy = "R" ;
  char **argv2 = &myguy; 
  int argc =1;
  gtk_init(&argc, &argv2);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_show(window);
  gtk_main();
  
  return ScalarLogical(TRUE);
}

extern "C"
SEXP gtkTest2(SEXP window)
{
  GtkWidget *awindow = (GtkWidget *) R_ExternalPtrAddr(window);
  GtkWidget *button;
  
  //button = gtk_button_new_with_label("Hi There");
  button = gtk_button_new_with_label ("Hello World");

  gtk_container_set_border_width(GTK_CONTAINER(awindow), 10);
  gtk_container_add(GTK_CONTAINER(awindow), button);
  
  gtk_widget_show(button);
  
  return ScalarLogical(TRUE);
}
