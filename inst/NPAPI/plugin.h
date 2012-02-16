#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <npapi.h>
#include <npruntime.h>
#include <time.h>
#include "WebR.h"

class CPlugin
{
public:
  CPlugin(NPP pNPInstance);
  ~CPlugin();

  NPBool init(NPWindow* pNPWindow);
  void shut();
  NPBool isInitialized();
  
  int handleEvent(void* event);

  void clear();
  void getVersion(char* *aVersion);

	bool IsLocked (void);
	
#ifdef XP_WIN32
	HWND GetHWND (void);
#endif

  NPObject *GetScriptableObject();

	char m_String[128];

	NPP GetNPPInstance (void);


private:
	NPP m_pNPInstance;

	bool m_bLocked;

#ifdef XP_WIN
	HWND m_hWnd; 
#endif
	
	NPStream * m_pNPStream;
	NPBool m_bInitialized;

	NPObject *m_pScriptableObject;
};

#endif // __PLUGIN_H__
