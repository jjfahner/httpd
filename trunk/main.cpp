#include "precomp.h"
#include "httpd_com.h"

//////////////////////////////////////////////////////////////////////////

CComModule g_module;

BEGIN_OBJECT_MAP(ObjectMap)
  OBJECT_ENTRY(CLSID_Server, Server)
  OBJECT_ENTRY(CLSID_Site, Site)
  OBJECT_ENTRY(CLSID_Request, Request)
  OBJECT_ENTRY(CLSID_Response, Response)
END_OBJECT_MAP()

//////////////////////////////////////////////////////////////////////////

extern "C" BOOL  __declspec(dllexport) WINAPI
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
  switch(dwReason)
  {
  case DLL_PROCESS_ATTACH:
    g_module.Init(ObjectMap, hInstance, &LIBID_HTTPD);
    DisableThreadLibraryCalls(hInstance); // TODO read up on this
    break;
  case DLL_PROCESS_DETACH:
    g_module.Term();
    break;
  }

  return 0;
}

//////////////////////////////////////////////////////////////////////////

STDAPI DllCanUnloadNow()
{
  return g_module.GetLockCount() == 0 ? S_OK : S_FALSE;
}

//////////////////////////////////////////////////////////////////////////

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
  return g_module.GetClassObject(rclsid, riid, ppv);
}

//////////////////////////////////////////////////////////////////////////

STDAPI DllRegisterServer()
{
   return g_module.RegisterServer(TRUE);
}

//////////////////////////////////////////////////////////////////////////

STDAPI DllUnregisterServer()
{
  return g_module.UnregisterServer(TRUE);
}

//////////////////////////////////////////////////////////////////////////
