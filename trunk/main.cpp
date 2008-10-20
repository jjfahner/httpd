#include "precomp.h"
#include "httpd_com.h"

//////////////////////////////////////////////////////////////////////////

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
  OBJECT_ENTRY(CLSID_Server, Server)
  OBJECT_ENTRY(CLSID_Site, Site)
  OBJECT_ENTRY(CLSID_Request, Request)
  OBJECT_ENTRY(CLSID_Response, Response)
END_OBJECT_MAP()

//////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
  switch(dwReason)
  {
  case DLL_PROCESS_ATTACH:
    _Module.Init(ObjectMap, hInstance, &LIBID_HTTPD);
    DisableThreadLibraryCalls(hInstance); // TODO
    break;
  case DLL_PROCESS_DETACH:
    _Module.Term();
    break;
  }
  return TRUE;
}

//////////////////////////////////////////////////////////////////////////

STDAPI DllCanUnloadNow()
{
  return _Module.GetLockCount() == 0 ? S_OK : S_FALSE;
}

//////////////////////////////////////////////////////////////////////////

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
  return _Module.GetClassObject(rclsid, riid, ppv);
}

//////////////////////////////////////////////////////////////////////////

STDAPI DllRegisterServer()
{
   return _Module.RegisterServer(TRUE);
}

//////////////////////////////////////////////////////////////////////////

STDAPI DllUnregisterServer()
{
  return _Module.UnregisterServer(TRUE);
}

//////////////////////////////////////////////////////////////////////////
