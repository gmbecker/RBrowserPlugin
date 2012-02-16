//////////////////////////////////////////////////
//
// CPlugin class implementation
//
#ifdef XP_WIN
#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>
#endif

#ifdef XP_MAC
#include <TextEdit.h>
#endif

#ifdef XP_UNIX
#include <string.h>
#endif

//#include <npupp.h>
#include "plugin.h"

#include "WebR.h"

using namespace std;

CPlugin::CPlugin(NPP pNPInstance) 

{
	m_pNPInstance = pNPInstance;
	m_pNPStream = NULL;
	m_bInitialized = false;
	m_pScriptableObject = NULL;
	
#ifdef XP_WIN
  m_hWnd = NULL;
#endif

	m_bLocked = false;
	
}

CPlugin::~CPlugin()
{

}

bool CPlugin::IsLocked (void)
{
	return m_bLocked;
}


#ifdef XP_WIN
static LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);
static WNDPROC lpOldProc = NULL;
#endif

NPP CPlugin::GetNPPInstance (void)
{
	return m_pNPInstance;
}

NPBool CPlugin::init(NPWindow* pNPWindow)
{
  if(pNPWindow == NULL)
	{
    return FALSE;
	}

#ifdef XP_WIN
  m_hWnd = (HWND)pNPWindow->window;
  if(m_hWnd == NULL)
    return FALSE;

  // subclass window so we can intercept window messages and
  // do our drawing to it
  lpOldProc = SubclassWindow(m_hWnd, (WNDPROC)PluginWinProc);

  // associate window with our CPlugin object so we can access 
  // it in the window procedure
  SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

#endif

  m_bInitialized = TRUE;

  return TRUE;
}

void CPlugin::shut()
{

#ifdef XP_WIN
  // subclass it back
  SubclassWindow(m_hWnd, lpOldProc);
	SetWindowLong (m_hWnd, GWL_USERDATA, (LONG) 0);
  m_hWnd = NULL;
#endif

	if (m_pScriptableObject)
	{
		WebREngine *pObj = (WebREngine *) m_pScriptableObject;
		pObj->Detatch ();
		NPN_ReleaseObject(m_pScriptableObject);
		m_pScriptableObject = NULL;
	}

	m_bInitialized = FALSE;

}

NPBool CPlugin::isInitialized()
{
  return m_bInitialized;
}

int CPlugin::handleEvent(void* event)
{

#ifdef XP_MACOSX
	EventRecord *per = (EventRecord *) event;

	if (per->what != nullEvent)
	{
		return 0;
	}
	
	WebREngine *pObj = (WebREngine *) GetScriptableObject();

	// OSX Carbon based event processing goes here!
	
	NPN_ReleaseObject(pObj);
#endif

  return 1;
}

// this will force to draw a version string in the plugin window

// this will clean the plugin window
void CPlugin::clear()
{
  strcpy(m_String, "");

#ifdef XP_WIN
  InvalidateRect(m_hWnd, NULL, TRUE);
  UpdateWindow(m_hWnd);
#endif
}

void CPlugin::getVersion(char* *aVersion)
{
  const char *ua = NPN_UserAgent(m_pNPInstance);
  char*& version = *aVersion;
  version = (char*)NPN_MemAlloc(1 + strlen(ua));
  if (version)
    strcpy(version, ua);
}

#ifdef XP_WIN
HWND CPlugin::GetHWND (void)
{
	assert (m_hWnd);
	return m_hWnd;
}
#endif

NPObject *
CPlugin::GetScriptableObject()
{
  if (m_pScriptableObject == NULL)
	{
		m_pScriptableObject = NPN_CreateObject (m_pNPInstance, &WebREngine::_npclass);

		WebREngine *pobj = (WebREngine *) m_pScriptableObject;
  }

  if (m_pScriptableObject) 
	{
    NPN_RetainObject(m_pScriptableObject);
  }

  return m_pScriptableObject;
}

#ifdef XP_WIN
static LRESULT CALLBACK PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Windows based message processing goes here
	return DefWindowProc (hWnd, msg, wParam, lParam);
}
#endif
