#ifndef HTTPD_COM_H
#define HTTPD_COM_H

#include <atlbase.h>
#include <atlcom.h>
#include "httpd_h.h"

//////////////////////////////////////////////////////////////////////////

class Server : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public IDispatchImpl<IServer, &IID_IServer, &CAtlModule::m_libid, 0xFFFF, 0xFFFF>,
  public CComCoClass<Server, &IID_IServer>, 
  public ISupportErrorInfoImpl<&IID_IServer>
{
public:

  //
  // Implement IServer
  //
  virtual HRESULT STDMETHODCALLTYPE GetSite(BSTR name, ISite **site);
  virtual HRESULT STDMETHODCALLTYPE AddSite(BSTR name, ISite **site);
  virtual HRESULT STDMETHODCALLTYPE RemoveSite(BSTR name);
  virtual HRESULT STDMETHODCALLTYPE Start();
  virtual HRESULT STDMETHODCALLTYPE Stop();

};

//////////////////////////////////////////////////////////////////////////

class Site : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public IDispatchImpl<ISite, &IID_ISite, &CAtlModule::m_libid, 0xFFFF, 0xFFFF>,
  public CComCoClass<Site, &IID_ISite>, 
  public ISupportErrorInfoImpl<&IID_ISite>
{
public:

};

//////////////////////////////////////////////////////////////////////////

#endif // HTTPD_COM_H
